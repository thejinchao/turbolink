#include "TurboLinkGrpcConfig.h"
#if WITH_EDITOR
#include "ISettingsModule.h"
#endif

UTurboLinkGrpcConfig::UTurboLinkGrpcConfig()
{

}

FString UTurboLinkGrpcConfig::GetServiceEndPoint(const FString& ServiceName) const
{
    const FString* serviceEndPoint = ServiceEndPoint.Find(ServiceName);
    if (serviceEndPoint) {
        return *serviceEndPoint;
    }
    return DefaultEndPoint;
}

FString UTurboLinkGrpcConfig::GetServerRootCerts() const
{
    return ServerRootCerts.Replace(TEXT("\\n"), TEXT("\n"));
}

#if WITH_EDITOR
#define LOCTEXT_NAMESPACE "TurboLinkGrpcModule"
bool FTurboLinkGrpcConfig::HandleSettingsSaved()
{
    this->Config->SaveConfig();
    // Prevent the section that gets automatically added by the config serialization system from being added now.
    return false;
}

void FTurboLinkGrpcConfig::RegisterSettings()
{
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        // Make the instanced version of the config
        this->Config = NewObject<UTurboLinkGrpcConfig>();
        this->Config->AddToRoot();

        this->Config->LoadConfig();

        // Register the settings
        this->SettingsSection = SettingsModule->RegisterSettings(
            "Project",
            "Game",
            "TurboLink Grpc",
            LOCTEXT("TurboLinkGrpcConfigName", "TurboLink Grpc"),
            LOCTEXT("TurboLinkGrpcConfigDescription", "Configure TurboLink Grpc Services in your game."),
            this->Config);

        if (SettingsSection.IsValid())
        {
            SettingsSection->OnModified().BindRaw(this, &FTurboLinkGrpcConfig::HandleSettingsSaved);
        }
    }
}

void FTurboLinkGrpcConfig::UnregisterSettings()
{
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->UnregisterSettings("Project", "Game", "TurboLink Grpc");
    }
}
#undef LOCTEXT_NAMESPACE

#endif