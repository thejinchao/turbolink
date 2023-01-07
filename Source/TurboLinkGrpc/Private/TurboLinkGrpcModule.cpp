#include "TurboLinkGrpcModule.h"
#include "TurboLinkGrpcManager.h"

#include "Misc/ConfigCacheIni.h"
#include "HAL/ExceptionHandling.h"
#include "Logging/LogMacros.h"
#include "Logging/LogVerbosity.h"

#include "grpc/grpc.h"
#include "grpc/support/log.h"

#define LOCTEXT_NAMESPACE "FTurboLinkGrpcModule"

DEFINE_LOG_CATEGORY(LogTurboLink);

void GrpcLogEntry(gpr_log_func_args* args)
{
	FString logMessage = FString::Printf(TEXT("grpc:(%s:%d)%s"), UTF8_TO_TCHAR(args->file), args->line, UTF8_TO_TCHAR(args->message));

    ELogVerbosity::Type logSeverity = ELogVerbosity::Type::Log;
    switch (args->severity)
    {
    case GPR_LOG_SEVERITY_DEBUG:
		UE_LOG(LogTurboLink, Verbose, TEXT("%s"), *logMessage);
		break;
    case GPR_LOG_SEVERITY_INFO:
		UE_LOG(LogTurboLink, Log, TEXT("%s"), *logMessage);
        break;
    case GPR_LOG_SEVERITY_ERROR:
		UE_LOG(LogTurboLink, Error, TEXT("%s"), *logMessage);
        break;
    }

}

FTurboLinkGrpcModule::FTurboLinkGrpcModule()
	: TurboLinkGrpcManager(nullptr)
{
}

FTurboLinkGrpcModule::~FTurboLinkGrpcModule()
{
}

void FTurboLinkGrpcModule::StartupModule()
{
	UE_LOG(LogTurboLink, Log, TEXT("Startup TurboLink Module"));

	gpr_set_log_verbosity(GPR_LOG_SEVERITY_DEBUG);
	gpr_set_log_function(GrpcLogEntry);

#if WITH_EDITOR
	//Register TurboLink Grpc config page
	ConfigInstance = MakeShared<FTurboLinkGrpcConfig>();
	ConfigInstance->RegisterSettings();
#else
	ConfigInstance = NewObject<UTurboLinkGrpcConfig>();
	ConfigInstance->AddToRoot();

	ConfigInstance->LoadConfig();
#endif
}

void FTurboLinkGrpcModule::ShutdownModule()
{
	UE_LOG(LogTurboLink, Log, TEXT("Shutdown TurboLink Module"));
	if (TurboLinkGrpcManager) 
	{
		TurboLinkGrpcManager->RemoveFromRoot();
		TurboLinkGrpcManager = nullptr;
	}

#if WITH_EDITOR
	ConfigInstance->UnregisterSettings();
#else
	ConfigInstance->RemoveFromRoot();
#endif
}

UTurboLinkGrpcManager* FTurboLinkGrpcModule::GetTurboLinkGrpcManager(void)
{
	if (TurboLinkGrpcManager)
	{
		return TurboLinkGrpcManager;
	}

	TurboLinkGrpcManager = NewObject<UTurboLinkGrpcManager>();
	TurboLinkGrpcManager->AddToRoot();
	return TurboLinkGrpcManager;
}

UTurboLinkGrpcConfig* FTurboLinkGrpcModule::GetTurboLinkGrpcConfig()
{
#if WITH_EDITOR
	return ConfigInstance->GetConfig();
#else
	return ConfigInstance;
#endif
}

void FTurboLinkGrpcModule::Tick(float DeltaTime)
{
	if (TurboLinkGrpcManager)
	{
		TurboLinkGrpcManager->Tick(DeltaTime);
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTurboLinkGrpcModule, TurboLinkGrpc)
