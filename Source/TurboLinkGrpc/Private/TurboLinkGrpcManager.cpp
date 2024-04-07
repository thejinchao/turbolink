// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#include "TurboLinkGrpcManager.h"
#include "TurboLinkGrpcManager_Private.h"
#include "TurboLinkGrpcContext.h"
#include "TurboLinkGrpcModule.h"
#include "UObject/UObjectHash.h"

UTurboLinkGrpcManager::UTurboLinkGrpcManager()
	: d (new UTurboLinkGrpcManager::Private())
	, NextTag(0)
{
}

UTurboLinkGrpcManager::~UTurboLinkGrpcManager()
{
	for (auto& CurrentService : WorkingService)
	{
		CurrentService.Value->Shutdown();
	}

	delete d;
}

void UTurboLinkGrpcManager::Initialize(FSubsystemCollectionBase& Collection)
{
	bIsShutdowning = false;
	if (bIsInitialized) return;
	UE_LOG(LogTurboLink, Log, TEXT("Initialize TurboLinkGrpcManager"));

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

void UTurboLinkGrpcManager::Deinitialize()
{
	if (bIsShutdowning) return;
	bIsShutdowning = true;
	UE_LOG(LogTurboLink, Log, TEXT("Deinitialize TurboLinkGrpcManager"));

	//Shutdown all service
	for (auto& element : WorkingService)
	{
		element.Value->Shutdown();
	}
	WorkingService.Empty();
	ShutingDownService.Empty();

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
			EGrpcServiceState newState = Private::GrpcStateToServiceState(serviceChannel->ChannelState);
			for (auto& serviceElement : serviceChannel->AttachedServices)
			{
				serviceElement->OnServiceStateChanged.Broadcast(newState);
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

	//get config instance
	FTurboLinkGrpcModule* turboLinkModule = FModuleManager::GetModulePtr<FTurboLinkGrpcModule>("TurboLinkGrpc");
	const UTurboLinkGrpcConfig* config = turboLinkModule->GetTurboLinkGrpcConfig();
	int keepServiceAliveWithoutRefrenceSeconds = config->KeepServiceAliveWithoutRefrenceSeconds;

	//Tick working service
	for (auto it = WorkingService.CreateIterator(); it; ++it)
	{
		UGrpcService* service = it.Value();
		service->Tick(DeltaTime);

		//pending shutdown?
		if (service->RefrenceCounts <= 0 && service->StartPendingShutdown > 0.0 && keepServiceAliveWithoutRefrenceSeconds > 0)
		{
			double secondsNow = FPlatformTime::Seconds();
			if (secondsNow - service->StartPendingShutdown > keepServiceAliveWithoutRefrenceSeconds)
			{
				//remove from working map and add to shutingdown set
				it.RemoveCurrent();
				ShutingDownService.Add(service);

				//call real shutdown function
				service->StartPendingShutdown = 0.0;
				service->Shutdown();

				UE_LOG(LogTurboLink, Log, TEXT("Shutdown service[%s]"), *(service->GetName()));
			}
		}
	}
	WorkingService.Compact();

	//Tick shuting down service
	for (auto it = ShutingDownService.CreateIterator(); it; ++it)
	{
		UGrpcService* service = *it;
		service->Tick(DeltaTime);

		if (service->bIsShutingDown && service->ClientSet.Num() == 0)
		{
			//Remove shutdowned service
			it.RemoveCurrent();
		}
	}
	ShutingDownService.Compact();
}

UGrpcService* UTurboLinkGrpcManager::MakeService(const FString& ServiceName)
{
	UGrpcService** workingService = WorkingService.Find(ServiceName);

	//find existent working service 
	if (workingService != nullptr)
	{
		//add refrence
		(*workingService)->RefrenceCounts += 1;
		(*workingService)->StartPendingShutdown = 0.0;

		UE_LOG(LogTurboLink, Log, TEXT("MakeService service[%s], RefrenceCounts=[%d]"), *ServiceName, (*workingService)->RefrenceCounts);
		return *workingService;
	}

	//create new service object
	UClass** serviceClass = ServiceClassMap.Find(ServiceName);
	if (serviceClass == nullptr)
	{
		UE_LOG(LogTurboLink, Error, TEXT("Can't find service class[%s]"), *ServiceName);
		return nullptr;
	}

	UGrpcService* service = NewObject<UGrpcService>(this, *serviceClass);
	service->TurboLinkManager = this;
	service->RefrenceCounts = 1;
	WorkingService.Add(ServiceName, service);
	return service;
}

void UTurboLinkGrpcManager::ReleaseService(UGrpcService* Service)
{
	check(Service);

	//release refrence
	if (Service->RefrenceCounts > 1) 
	{
		Service->RefrenceCounts -= 1;
		UE_LOG(LogTurboLink, Log, TEXT("ReleaseService service[%s], RefrenceCounts=[%d]"), *(Service->GetName()), Service->RefrenceCounts);
		return;
	}

	//pending to shutdown
	Service->RefrenceCounts = 0;
	Service->StartPendingShutdown = FPlatformTime::Seconds();

	UE_LOG(LogTurboLink, Log, TEXT("ReleaseService service[%s], StartPendingShutdown..."), *(Service->GetName()));
}

EGrpcServiceState UTurboLinkGrpcManager::GetServiceState(UGrpcService* Service)
{
	check(Service != nullptr);

	return Service->GetServiceState();
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
