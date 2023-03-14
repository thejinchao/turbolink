#pragma once
#include "CoreMinimal.h"
#include "TurboLinkGrpcMessage.generated.h"

USTRUCT(BlueprintType)
struct FGrpcMessage
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType, meta = (
	HasNativeMake = "TurboLinkGrpc.TurboLinkGrpcUtilities.MakeUInt64", 
	HasNativeBreak = "TurboLinkGrpc.TurboLinkGrpcUtilities.BreakUInt64"))
struct FUInt64
{
	GENERATED_BODY()

	uint64 Value;

	operator uint64() const { return Value; }

	FUInt64() { this->Value = 0; }
	FUInt64(const uint64& _Value) { this->Value = _Value; }
};

USTRUCT(BlueprintType, meta = (
	HasNativeMake = "TurboLinkGrpc.TurboLinkGrpcUtilities.MakeDouble64",
	HasNativeBreak = "TurboLinkGrpc.TurboLinkGrpcUtilities.BreakDouble64"))
struct FDouble64
{
	GENERATED_BODY()

	double Value;

	operator double() const { return Value; }

	FDouble64() { this->Value = 0.0; }
	FDouble64(const double& _Value) { this->Value = _Value; }
};

USTRUCT(BlueprintType, meta = (
	HasNativeMake = "TurboLinkGrpc.TurboLinkGrpcUtilities.MakeUInt32", 
	HasNativeBreak = "TurboLinkGrpc.TurboLinkGrpcUtilities.BreakUInt32"))
struct FUInt32
{
	GENERATED_BODY()

	uint32 Value;

	operator uint32() const { return Value; }

	FUInt32() { this->Value = 0; }
	FUInt32(const uint32& _Value) { this->Value = _Value; }
};

USTRUCT(BlueprintType)
struct FBytes
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<uint8> Value;

	FBytes() {  }
	FBytes(const uint8* _Value, int32 _Length) { Value.Append(_Value, _Length); }
};