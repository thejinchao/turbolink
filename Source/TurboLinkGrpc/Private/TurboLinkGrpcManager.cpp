#include "TurboLinkGrpcManager.h"
#include "TurboLinkGrpcManager_Private.h"
#include "TurboLinkGrpcContext.h"
#include "TurboLinkGrpcModule.h"

UTurboLinkGrpcManager::UTurboLinkGrpcManager()
	: d (new UTurboLinkGrpcManager::Private())
	, NextTag(0)
{
	UE_LOG(LogTurboLink, Log, TEXT("Construct TurboLinkManager[%p]"), this);
}

UTurboLinkGrpcManager::~UTurboLinkGrpcManager()
{
	UE_LOG(LogTurboLink, Log, TEXT("Destruct TurboLinkManager[%p]"), this);
	delete d;

	//Notify TurboLink Module, the turbolink manager be destroyed.
	FTurboLinkGrpcModule* turboLinkModule = FModuleManager::GetModulePtr<FTurboLinkGrpcModule>("TurboLinkGrpc");
	if (turboLinkModule)
	{
		turboLinkModule->TurboLinkGrpcManager = nullptr;
	}
}

void UTurboLinkGrpcManager::InitManager()
{
	bIsShutdowning = false;
	if (bIsInitialized) return;

	//Registe All Service Classes
	TArray<UClass*> seriviceClasses;
	GetDerivedClasses(UGrpcService::StaticClass(), seriviceClasses, false);

	for (int32 i = 0; i < seriviceClasses.Num(); i++)
	{
		UClass* serviceClass = seriviceClasses[i];
		ServiceClassMap.Add(serviceClass->GetName(), serviceClass);
	}
	//Create global completion queue
	d->CompletionQueue = std::make_unique<grpc::CompletionQueue>();
	bIsInitialized = true;
}

void UTurboLinkGrpcManager::Shutdown()
{
	if (bIsShutdowning) return;
	bIsShutdowning = true;

	//Shutdown all service
	for (UGrpcService* service : ServiceSet)
	{
		service->Shutdown();
	}
	ServiceSet.Empty();

	//Shutdown and drain the completion queue
	d->ShutdownCompletionQueue();
	d->CompletionQueue = nullptr;
	GrpcContextMap.Empty();

	//clean class map
	ServiceClassMap.Empty();
	bIsInitialized = false;
}

void UTurboLinkGrpcManager::Tick(float DeltaTime)
{
	if (bIsShutdowning || !bIsInitialized) return;

	//Check channel state
	for (auto& channelElement : d->ChannelMap)
	{
		std::shared_ptr<Private::ServiceChannel> serviceChannel = channelElement.second;
		if (serviceChannel->UpdateState()) 
		{
			for (auto& serviceElement : serviceChannel->AttachedServices)
			{
				serviceElement->OnServiceStateChanged.Broadcast(Private::GrpcStateToServiceState(serviceChannel->ChannelState));
			}
		}
	}

	// Loop while listening for completed event.
	gpr_timespec deadline;
	deadline.clock_type = GPR_CLOCK_MONOTONIC;
	deadline.tv_nsec = 1;
	deadline.tv_sec = 0;

	while (true)
	{
		void* event_tag=nullptr;
		bool ok;
		auto result = d->CompletionQueue->AsyncNext(&event_tag, &ok, deadline);

		if (result == grpc::CompletionQueue::NextStatus::GOT_EVENT)
		{
			TSharedPtr<GrpcContext>* grpcContext = GrpcContextMap.Find(event_tag);
			if (grpcContext != nullptr)
			{
				(*grpcContext)->OnRpcEvent(ok, event_tag);
			}
		}
		else
		{
			//time out
			break;
		}
	}

	//Tick service
	TArray<UGrpcService*> needRemove;
	for (UGrpcService* service : ServiceSet)
	{
		service->Tick(DeltaTime);

		if (service->bIsShutdowning && service->ClientSet.Num() == 0)
		{
			needRemove.Add(service);
		}
	}

	//Remove shutdown service
	for (UGrpcService* service : needRemove)
	{
		ServiceSet.Remove(service);
	}
}

UGrpcService* UTurboLinkGrpcManager::MakeService(const FString& ServiceName)
{
	UClass** serviceClass = ServiceClassMap.Find(ServiceName);
	if (serviceClass == nullptr)
	{
		return nullptr;
	}

	UGrpcService* service = NewObject<UGrpcService>(this, *serviceClass);
	service->TurboLinkManager = this;
	ServiceSet.Add(service);
	return service;
}

void* UTurboLinkGrpcManager::GetNextTag(TSharedPtr<GrpcContext> Context)
{
	void* nextTag = (void*)(++NextTag);
	GrpcContextMap.Add(nextTag, Context);
	return nextTag;
}

void UTurboLinkGrpcManager::RemoveTag(void* Tag)
{
	GrpcContextMap.Remove(Tag);
}

FGrpcContextHandle UTurboLinkGrpcManager::GetNextContextHandle()
{
	FGrpcContextHandle nextHandle(++NextContextHandle);
	return nextHandle;
}
