// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#include "TurboLinkGrpcModule.h"
#include "TurboLinkGrpcManager.h"

#include "UObject/UObjectGlobals.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Char.h"
#include "HAL/ExceptionHandling.h"
#include "Logging/LogMacros.h"
#include "Logging/LogVerbosity.h"

#include "grpc/grpc.h"
#include "grpc/support/log.h"
#include "google/protobuf/descriptor_database.h"

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

#if WITH_EDITOR
	//Register TurboLink Grpc config page
	ConfigInstance = MakeShared<FTurboLinkGrpcConfig>();
	ConfigInstance->RegisterSettings();
#else
	ConfigInstance = NewObject<UTurboLinkGrpcConfig>();
	ConfigInstance->AddToRoot();

	ConfigInstance->LoadConfig();
#endif

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
		FString rpcMessageName(UTF8_TO_TCHAR(msgNames[i].c_str()));

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
		UScriptStruct* scriptStruct = FindObject<UScriptStruct>(ANY_PACKAGE, *grpcMessageName, true);
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

#if WITH_EDITOR
	ConfigInstance->UnregisterSettings();
#else
	ConfigInstance->RemoveFromRoot();
#endif
}

UTurboLinkGrpcConfig* FTurboLinkGrpcModule::GetTurboLinkGrpcConfig()
{
#if WITH_EDITOR
	return ConfigInstance->GetConfig();
#else
	return ConfigInstance;
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTurboLinkGrpcModule, TurboLinkGrpc)
