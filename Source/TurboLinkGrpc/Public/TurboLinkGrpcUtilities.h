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

	//----- FInt64
	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "Create Int64")
	static FInt64 CreateInt64(int32 Value)
	{
		return FInt64{ static_cast<int64>(Value) };
	}

	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "Int64 -> String")
	static FString CastInt64ToString(const FInt64& In)
	{
		return FString::Printf(TEXT("%lld"), In.Value);
	}

	//----- FUInt64
	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "Create UInt64")
	static FUInt64 CreateUInt64(int32 Value)
	{
		return FUInt64{ static_cast<uint64>(Value) };
	}

	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "UInt64 -> String")
	static FString CastUInt64ToString(const FUInt64& In)
	{
		return FString::Printf(TEXT("%llu"), In.Value);
	}

	//----- FUInt32
	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "Create UInt32")
	static FUInt32 CreateUInt32(int32 Value)
	{
		return FUInt32{ static_cast<uint32>(Value) };
	}

	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "UInt32 -> String")
	static FString CastUInt32ToString(const FUInt32& In)
	{
		return FString::Printf(TEXT("%ud"), In.Value);
	}

	//----- FDouble64
	UFUNCTION(BlueprintPure, Category = "TurboLink", DisplayName = "Create Double64")
	static FDouble64 CreateDouble64(float Value)
	{
		return FDouble64{ static_cast<double>(Value) };
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
