#pragma once
#include "CoreMinimal.h"
#include "TurboLinkGrpcMessage.generated.h"

#define DECLARE_JSON_FUNCTIONS() \
      TURBOLINKGRPC_API virtual FString ToJsonString() const override; \
      TURBOLINKGRPC_API virtual bool FromJsonString(const FString& JsonString) override;

#define DEFINE_JSON_FUNCTIONS(StructName, GrpcStructName) \
FString StructName::ToJsonString() const \
{ \
      GrpcStructName message; \
      TURBOLINK_TO_GRPC(this, &message); \
      std::string json_string; \
      ::google::protobuf::util::MessageToJsonString(message,  &json_string); \
      return FString(UTF8_TO_TCHAR(json_string.c_str())); \
} \
bool StructName::FromJsonString(const FString& JsonString) \
{ \
	GrpcStructName grpcMessage; \
	if(!::google::protobuf::util::JsonStringToMessage(TCHAR_TO_UTF8(*JsonString), &grpcMessage).ok()) return false; \
	GRPC_TO_TURBOLINK(&grpcMessage, this); \
	return true; \
}

USTRUCT(BlueprintType)
struct FGrpcMessage
{
	GENERATED_BODY()
	virtual ~FGrpcMessage() = default;

	virtual FString ToJsonString() const { return FString(TEXT("{}")); }
	virtual bool FromJsonString(const FString& JsonString) { return false; }
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