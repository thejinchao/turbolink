#pragma once
#include "CoreMinimal.h"
#include "TurboLinkGrpcMessage.generated.h"

USTRUCT()
struct FGrpcMessage
{
	GENERATED_USTRUCT_BODY()
};

USTRUCT(BlueprintType)
struct FInt64
{
	GENERATED_BODY()

	int64 Value;

	operator int64() const { return Value; }

	FInt64() { this->Value = 0; }
	FInt64(const int64& _Value) { this->Value = _Value; }
};

USTRUCT(BlueprintType)
struct FUInt64
{
	GENERATED_BODY()

	uint64 Value;

	operator uint64() const { return Value; }

	FUInt64() { this->Value = 0; }
	FUInt64(const uint64& _Value) { this->Value = _Value; }
};

USTRUCT(BlueprintType)
struct FDouble64
{
	GENERATED_BODY()

	double Value;

	operator double() const { return Value; }

	FDouble64() { this->Value = 0.0; }
	FDouble64(const double& _Value) { this->Value = _Value; }
};

USTRUCT(BlueprintType)
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