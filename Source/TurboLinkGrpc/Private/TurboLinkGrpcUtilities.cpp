#include "TurboLinkGrpcUtilities.h"
#include "TurboLinkGrpcModule.h"
#include "Kismet/GameplayStatics.h"

UTurboLinkGrpcManager* UTurboLinkGrpcUtilities::GetTurboLinkGrpcManager(UObject* WorldContextObject)
{
	if (WorldContextObject == nullptr)
	{
		TIndirectArray<FWorldContext> Worlds = GEngine->GetWorldContexts();
		for (auto World : Worlds)
		{
			if (World.WorldType == EWorldType::Game || World.WorldType == EWorldType::PIE)
			{
				WorldContextObject = World.World();
				break;
			}
		}
	}
	if (WorldContextObject == nullptr)	return nullptr;

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	if (GameInstance == nullptr) return nullptr;

	return GameInstance->GetSubsystem<UTurboLinkGrpcManager>();
}

UTurboLinkGrpcConfig* UTurboLinkGrpcUtilities::GetTurboLinkGrpcConfig()
{
    FTurboLinkGrpcModule* turboLinkModule = FModuleManager::GetModulePtr<FTurboLinkGrpcModule>("TurboLinkGrpc");
    if (turboLinkModule == nullptr)
    {
        turboLinkModule = &(FModuleManager::LoadModuleChecked<FTurboLinkGrpcModule>("TurboLinkGrpc"));
    }

    return turboLinkModule->GetTurboLinkGrpcConfig();
}
