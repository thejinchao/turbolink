// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#include "TurboLinkGrpcService.h"
#include "TurboLinkGrpcUtilities.h"
#include "TurboLinkGrpcManager_Private.h"
#include "TurboLinkGrpcModule.h"

UGrpcService::UGrpcService()
{
}

UGrpcService::~UGrpcService()
{
}

void UGrpcService::Tick(float DeltaTime)
{
	for (auto it = ClientSet.CreateIterator(); it; ++it)
	{
		UGrpcClient* client = *it;

		client->Tick(DeltaTime);

		//All context has stoped
		if (client->bIsShutdowning && client->ContextMap.Num() == 0)
		{
			it.RemoveCurrent();
		}
	}
}

void UGrpcService::Shutdown()
{
	//already in shutdown progress?
	if (bIsShutingDown) return;

	OnServiceStateChanged.Clear();

	//Disconnect all clients
	for (UGrpcClient* client : ClientSet)
	{
		client->Shutdown();
	}
	bIsShutingDown = true;
}

void UGrpcService::RemoveClient(UGrpcClient* Client)
{
	ClientSet.Remove(Client);
}
