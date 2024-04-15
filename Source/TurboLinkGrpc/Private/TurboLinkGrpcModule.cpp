// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#include "TurboLinkGrpcModule.h"
#include "TurboLinkGrpcManager.h"

#include "UObject/UObjectGlobals.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Char.h"
#include "HAL/ExceptionHandling.h"
#include "Logging/LogMacros.h"
#include "Logging/LogVerbosity.h"

#if PLATFORM_WINDOWS
#include "Windows/PreWindowsApi.h"
#endif

#include "grpc/grpc.h"
#include "grpc/support/log.h"
#include "google/protobuf/descriptor_database.h"

#if PLATFORM_WINDOWS
#include "Windows/PostWindowsApi.h"
#endif

#define LOCTEXT_NAMESPACE "FTurboLinkGrpcModule"

DEFINE_LOG_CATEGORY(LogTurboLink);

void GrpcLogEntry(gpr_log_func_args* args)
{
	FString logMessage = FString::Printf(TEXT("grpc:(%s:%d)%s"), 
		StringCast<TCHAR>((const UTF8CHAR*)(args->file)).Get(),
		args->line, 
		StringCast<TCHAR>((const UTF8CHAR*)(args->message)).Get());

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
	: CachedSettings(nullptr)
{
}

FTurboLinkGrpcModule::~FTurboLinkGrpcModule()
{
}

void FTurboLinkGrpcModule::StartupModule()
{
	UE_LOG(LogTurboLink, Log, TEXT("Startup TurboLinkGrpcModule"));

	gpr_set_log_verbosity(GPR_LOG_SEVERITY_DEBUG);
	gpr_set_log_function(GrpcLogEntry);

	//Access config via 'class default object'
	GetTurboLinkGrpcConfig();

#if WITH_EDITOR
	//register grpc message script struct
	this->RegisterAllGrpcMessageScriptStruct();
#endif
}

#if WITH_EDITOR
const TMap<FName, UScriptStruct*>& FTurboLinkGrpcModule::GetMessageStructMap()
{
	return GrpcMessageStructMap;
}

void FTurboLinkGrpcModule::RegisterAllGrpcMessageScriptStruct()
{
	google::protobuf::DescriptorDatabase* protobufDataBase = google::protobuf::DescriptorPool::internal_generated_database();
	if (protobufDataBase == nullptr) return;

	std::vector<std::string> msgNames;
	protobufDataBase->FindAllMessageNames(&msgNames);
	for (size_t i = 0; i < msgNames.size(); i++)
	{
		FString rpcMessageName(StringCast<TCHAR>((const UTF8CHAR*)msgNames[i].c_str()).Get());

		TArray<FString> subNames;
		rpcMessageName.ParseIntoArray(subNames, TEXT("."), true);

		FString grpcMessageName = "Grpc";
		for (int32 index = 0; index < subNames.Num(); index++)
		{
			FString subName = subNames[index];
			grpcMessageName += TChar<FString::ElementType>::ToUpper(subName[0]);
			grpcMessageName += subName.RightChop(1);
		}

		//add message script struct to map
#if (ENGINE_MAJOR_VERSION==5 && ENGINE_MINOR_VERSION>=1) || ENGINE_MAJOR_VERSION>5
		UScriptStruct* scriptStruct = FindFirstObject<UScriptStruct>(*grpcMessageName);
#else
		UScriptStruct* scriptStruct = FindObject<UScriptStruct>(ANY_PACKAGE, *grpcMessageName, true);
#endif
		if (scriptStruct)
		{
			FName grpcMessageTagName = FName(*(scriptStruct->GetDisplayNameText().ToString()));
			GrpcMessageStructMap.Add(grpcMessageTagName, scriptStruct);
		}
	}
}
#endif

void FTurboLinkGrpcModule::ShutdownModule()
{
	UE_LOG(LogTurboLink, Log, TEXT("Shutdown TurboLinkGrpcModule"));
}

const UTurboLinkGrpcConfig* FTurboLinkGrpcModule::GetTurboLinkGrpcConfig() const
{
	if (!CachedSettings)
	{
		CachedSettings = GetDefault<UTurboLinkGrpcConfig>();
	}
	return CachedSettings;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTurboLinkGrpcModule, TurboLinkGrpc)
