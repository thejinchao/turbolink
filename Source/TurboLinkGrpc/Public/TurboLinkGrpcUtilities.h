// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TurboLinkGrpcMessage.h"
#include "TurboLinkGrpcClient.h"
#include "TurboLinkGrpcUtilities.generated.h"

UCLASS()
class TURBOLINKGRPC_API UTurboLinkGrpcUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "TurboLink", meta = (WorldContext = "WorldContextObject"))
	static class UTurboLinkGrpcManager* GetTurboLinkGrpcManager(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "TurboLink")
	static const class UTurboLinkGrpcConfig* GetTurboLinkGrpcConfig();

	//wrap class for uint64
	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "Make UInt64", meta = (AdvancedDisplay = "1"))
	static FUInt64 MakeUInt64(FString Value, int Base = 10)
	{
		return FUInt64{ FCString::Strtoui64(*Value, nullptr, Base) };
	}

	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "Break UInt64")
	static void BreakUInt64(const FUInt64& UInt64, FString& Value)
	{
		Value = FString::Printf(TEXT("%llu"), UInt64.Value);
	}

	//wrap class for uint32
	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "Make UInt32", meta = (AdvancedDisplay = "1"))
	static FUInt32 MakeUInt32(FString Value, int Base = 10)
	{
		return FUInt32{ static_cast<uint32>((FCString::Strtoui64(*Value, nullptr, Base) & 0xFFFFFFFFull)) };
	}

	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "Break UInt32")
	static void BreakUInt32(const FUInt32& UInt32, FString& Value)
	{
		Value = FString::Printf(TEXT("%ud"), UInt32.Value);
	}

	//wrap class for double
	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "Make Double64")
	static FDouble64 MakeDouble64(FString Value)
	{
		return FDouble64{ FCString::Atod(*Value) };
	}

	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "Break Double64")
	static void BreakDouble64(const FDouble64& Double64, FString& Value)
	{
		Value = FString::Printf(TEXT("%lf"), Double64.Value);
	}

	// Returns true if A is equal to B (A == B)
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (GrpcContextHandle)", CompactNodeTitle = "==", Keywords = "== equal", ScriptOperator = "=="), Category = "TurboLink")
	static bool EqualEqual_GrpcContextHandle(const FGrpcContextHandle& A, const FGrpcContextHandle& B)
	{
		return A.Value == B.Value;
	}

	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "Grpc Result To String")
	static FString GrpcResultToString(const FGrpcResult& GrpcResult)
	{
		return GrpcResult.GetMessageString();
	}

	// Generate grpc message from json string
	template<typename T>
	static TSharedPtr<T> NewGrpcMessageFromJsonString(const FString& JsonString)
	{
		UScriptStruct* scriptStruct = T::StaticStruct();
		bool bIsGrpcMessage = scriptStruct->IsChildOf(FGrpcMessage::StaticStruct());
		if (!bIsGrpcMessage) return TSharedPtr<T>();

		T* newMessage = (T*)FMemory::Malloc(scriptStruct->GetStructureSize());
		scriptStruct->InitializeStruct(newMessage);
		bool isSuccess = newMessage->FromJsonString(JsonString);

		//delete memory if failed!
		if (!isSuccess)
		{
			FMemory::Free(newMessage);
			return TSharedPtr<T>();
		}
		return MakeShareable<T>(newMessage);
	}
};

UCLASS()
class TURBOLINKGRPC_API UGrpcMessageToJsonFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, CustomThunk, meta = (BlueprintInternalUseOnly = "true", CustomStructureParam = "GrpcMessage"))
	static FString GrpcMessageToJsonInternal(UStruct* GrpcMessage, bool PrettyMode);

	DECLARE_FUNCTION(execGrpcMessageToJsonInternal);
	static FString GrpcMessageToJson_Impl(void* StructPtr, FStructProperty* StructProperty, bool bPrettyMode);

	UFUNCTION(BlueprintCallable, CustomThunk, meta = (BlueprintInternalUseOnly = "true", CustomStructureParam = "ReturnMessage"))
	static bool JsonToGrpcMessageInternal(const FString& JsonString, UStruct*& ReturnMessage);

	DECLARE_FUNCTION(execJsonToGrpcMessageInternal);
	static bool JsonToGrpcMessage_Impl(const FString& JsonString, void* StructPtr, FStructProperty* StructProperty);
};
