#include "TurboLinkGrpcService.h"
#include "TurboLinkGrpcUtilities.h"
#include "TurboLinkGrpcManager_Private.h"
#include "TurboLinkGrpcModule.h"

UGrpcService::UGrpcService()
{
	UE_LOG(LogTurboLink, Log, TEXT("Construct GrpcService[%p]"), this);
}

UGrpcService::~UGrpcService()
{
	UE_LOG(LogTurboLink, Log, TEXT("Destruct GrpcService[%p]"), this);
}

void UGrpcService::Tick(float DeltaTime)
{
	TArray<UGrpcClient*> needRemove;
	for (UGrpcClient* client : ClientSet)
	{
		client->Tick(DeltaTime);

		//All context has stoped
		if (client->bIsShutdowning && client->ContextMap.Num() == 0)
		{
			needRemove.Add(client);
		}
	}
	for (UGrpcClient* client : needRemove)
	{
		ClientSet.Remove(client);
	}
}

void UGrpcService::Shutdown()
{
	//already in shutdown progress?
	if (bIsShutdowning) return;

	OnServiceStateChanged.Clear();

	//Disconnect all clients
	for (UGrpcClient* client : ClientSet)
	{
		client->Shutdown();
	}
	bIsShutdowning = true;
}

void UGrpcService::AddClient(UGrpcClient* Client)
{
	ClientSet.Add(Client);
}

void UGrpcService::RemoveClient(UGrpcClient* Client)
{
	ClientSet.Remove(Client);
}
