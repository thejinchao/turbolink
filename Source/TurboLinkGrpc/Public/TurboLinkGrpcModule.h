// Copyright (C) Developed by Neo Jin. All Rights Reserved.
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

	const UTurboLinkGrpcConfig* GetTurboLinkGrpcConfig() const;
	
#if WITH_EDITOR
public:
	const TMap<FName, UScriptStruct*>& GetMessageStructMap();

private:
	void RegisterAllGrpcMessageScriptStruct();
	TMap<FName, UScriptStruct*> GrpcMessageStructMap;
#endif

private:
	mutable const class UTurboLinkGrpcConfig* CachedSettings;

public:
	FTurboLinkGrpcModule();
	~FTurboLinkGrpcModule();
};
