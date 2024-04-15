// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#include "TurboLinkGrpcUtilities.h"
#include "TurboLinkGrpcModule.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"

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

const UTurboLinkGrpcConfig* UTurboLinkGrpcUtilities::GetTurboLinkGrpcConfig()
{
    FTurboLinkGrpcModule* turboLinkModule = FModuleManager::GetModulePtr<FTurboLinkGrpcModule>("TurboLinkGrpc");
    if (turboLinkModule == nullptr)
    {
        turboLinkModule = &(FModuleManager::LoadModuleChecked<FTurboLinkGrpcModule>("TurboLinkGrpc"));
    }

    return turboLinkModule->GetTurboLinkGrpcConfig();
}

FString UGrpcMessageToJsonFunctionLibrary::GrpcMessageToJsonInternal(UStruct* GrpcMessage, bool bPrettyMode)
{
	checkf(false, TEXT("This function should not be called!"));
	return FString(TEXT(""));
}

DEFINE_FUNCTION(UGrpcMessageToJsonFunctionLibrary::execGrpcMessageToJsonInternal)
{
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* StructPtr = Stack.MostRecentPropertyAddress;
	FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);

	P_GET_UBOOL(PrettyMode);

	P_FINISH;
	P_NATIVE_BEGIN;
	*((FString*)RESULT_PARAM) = GrpcMessageToJson_Impl(StructPtr, StructProperty, PrettyMode);
	P_NATIVE_END;
}

FString UGrpcMessageToJsonFunctionLibrary::GrpcMessageToJson_Impl(void* StructPtr, FStructProperty* StructProperty, bool bPrettyMode)
{
	bool bIsGrpcMessage = StructProperty->Struct->IsChildOf(FGrpcMessage::StaticStruct());
	if (!bIsGrpcMessage)
	{
		return FString(TEXT("<errortype>"));
	}
	return StaticCast<FGrpcMessage*>(StructPtr)->ToJsonString(bPrettyMode);
}

bool UGrpcMessageToJsonFunctionLibrary::JsonToGrpcMessageInternal(const FString& JsonString, UStruct*& ReturnMessage)
{
	checkf(false, TEXT("This function should not be called!"));
	return false;
}

DEFINE_FUNCTION(UGrpcMessageToJsonFunctionLibrary::execJsonToGrpcMessageInternal)
{
	P_GET_PROPERTY(FStrProperty, JsonString);

	Stack.StepCompiledIn<FStructProperty>(NULL);
	FStructProperty* StructProperty = ExactCastField<FStructProperty>(Stack.MostRecentProperty);
	void* StructPtr = Stack.MostRecentPropertyAddress;

	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)RESULT_PARAM = JsonToGrpcMessage_Impl(JsonString, StructPtr, StructProperty);
	P_NATIVE_END;
}

bool UGrpcMessageToJsonFunctionLibrary::JsonToGrpcMessage_Impl(const FString& JsonString, void* StructPtr, FStructProperty* StructProperty)
{
	UScriptStruct* scriptStruct = StructProperty->Struct;
	bool bIsGrpcMessage = scriptStruct->IsChildOf(FGrpcMessage::StaticStruct());
	if (!bIsGrpcMessage) return false;

	return StaticCast<FGrpcMessage*>(StructPtr)->FromJsonString(JsonString);
}
