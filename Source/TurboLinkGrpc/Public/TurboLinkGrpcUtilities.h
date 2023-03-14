// Fill out your copyright notice in the Description page of Project Settings.

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
	static class UTurboLinkGrpcConfig* GetTurboLinkGrpcConfig();

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
};

