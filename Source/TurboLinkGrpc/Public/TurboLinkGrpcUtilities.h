// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TurboLinkGrpcMessage.h"
#include "TurboLinkGrpcUtilities.generated.h"


UCLASS()
class TURBOLINKGRPC_API UTurboLinkGrpcUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "TurboLink")
	static class UTurboLinkGrpcManager* GetTurboLinkGrpcManager();

	UFUNCTION(BlueprintPure, Category = "TurboLink")
	static class UTurboLinkGrpcConfig* GetTurboLinkGrpcConfig();

	//wrap class for uint64
	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "Make UInt64", meta = (AdvancedDisplay = "1"))
	static FUInt64 MakeUInt64(FString Value, int Base = 10)
	{
		return FUInt64{ FCString::Strtoui64(*Value, nullptr, Base) };
	}

	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "UInt64 -> String")
	static FString CastUInt64ToString(const FUInt64& In)
	{
		return FString::Printf(TEXT("%llu"), In.Value);
	}

	//wrap class for uint32
	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "Make UInt32", meta = (AdvancedDisplay = "1"))
	static FUInt32 MakeUInt32(FString Value, int Base = 10)
	{
		return FUInt32{ static_cast<uint32>((FCString::Strtoui64(*Value, nullptr, Base) & 0xFFFFFFFFull)) };
	}

	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "UInt32 -> String")
	static FString CastUInt32ToString(const FUInt32& In, int Base = 10)
	{
		return FString::Printf(TEXT("%ud"), In.Value);
	}

	//wrap class for double
	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "Create Double64")
	static FDouble64 CreateDouble64(FString Value)
	{
		return FDouble64{ FCString::Atod(*Value) };
	}

	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "Double64 -> String")
	static FString CastDouble64ToString(const FDouble64& In)
	{
		return FString::Printf(TEXT("%lf"), In.Value);
	}

	// Returns true if A is equal to B (A == B)
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (GrpcContextHandle)", CompactNodeTitle = "==", Keywords = "== equal", ScriptOperator = "=="), Category = "TurboLink")
	static bool EqualEqual_GrpcContextHandle(const FGrpcContextHandle& A, const FGrpcContextHandle& B)
	{
		return A.Value == B.Value;
	}
};
