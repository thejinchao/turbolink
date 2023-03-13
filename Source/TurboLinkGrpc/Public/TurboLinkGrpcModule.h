#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "TurboLinkGrpcManager.h"
#include "TurboLinkGrpcConfig.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTurboLink, Log, All);

class TURBOLINKGRPC_API FTurboLinkGrpcModule : public IModuleInterface
{
	friend class UTurboLinkGrpcManager;
public:
	// IModuleInterface implementation
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	UTurboLinkGrpcConfig* GetTurboLinkGrpcConfig();
	
private:
#if WITH_EDITOR
	TSharedPtr<FTurboLinkGrpcConfig> ConfigInstance;
#else
	UTurboLinkGrpcConfig* ConfigInstance;
#endif

public:
	FTurboLinkGrpcModule();
	~FTurboLinkGrpcModule();
};
