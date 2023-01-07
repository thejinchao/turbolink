#include "TurboLinkGrpcUtilities.h"
#include "TurboLinkGrpcModule.h"

UTurboLinkGrpcManager* UTurboLinkGrpcUtilities::GetTurboLinkGrpcManager()
{
    FTurboLinkGrpcModule* turboLinkModule = FModuleManager::GetModulePtr<FTurboLinkGrpcModule>("TurboLinkGrpc");
    if (turboLinkModule == nullptr)
    {
        turboLinkModule = &(FModuleManager::LoadModuleChecked<FTurboLinkGrpcModule>("TurboLinkGrpc"));
    }

    return turboLinkModule->GetTurboLinkGrpcManager();
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
