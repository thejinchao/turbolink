#pragma once

#include "CoreMinimal.h"
#if WITH_EDITOR
#include "ISettingsSection.h"
#endif
#include "TurboLinkGrpcConfig.generated.h"

UCLASS(config = Game, DefaultConfig)
class TURBOLINKGRPC_API UTurboLinkGrpcConfig : public UObject
{
	GENERATED_BODY()

public:
	//Enables retry functionality. 
	//Defaults to true.  When enabled, configurable retries are enabled when they are configured via the service config.
	UPROPERTY(Config, EditAnywhere, Category = "Channel Config")
	bool EnableRetries = true;

	// After a duration of this time the client/server pings its peer to see if the
	// transport is still alive.Int valued, milliseconds.
	UPROPERTY(Config, EditAnywhere, Category = "Channel Config")
	int KeepAliveTime=45000;

	// After waiting for a duration of this time, if the keepalive ping sender does
	// not receive the ping ack, it will close the transport.Int valued, milliseconds.
	UPROPERTY(Config, EditAnywhere, Category = "Channel Config")
	int KeepAliveTimeOut=30000;

	//How many pings can we send before needing to send a
	// data / header frame ? (0 indicates that an infinite number of
	// pings can be sent without sending a data frame or header frame)
	UPROPERTY(Config, EditAnywhere, Category = "Channel Config")
	int Http2MaxPingsWithoutData = 0;

	//Is it permissible to send keepalive pings without any outstanding streams.
	UPROPERTY(Config, EditAnywhere, Category = "Channel Config")
	bool KeepAlivePermitWithoutCalls = true;

	//Enable server-side tls connection type. Only sever needs to provide it's certificate to client.
	UPROPERTY(Config, EditAnywhere, Category = "Connection Config")
	bool EnableServerSideTLS = false;

	// The buffer containing the PEM encoding of the server root certificates.
	UPROPERTY(Config, EditAnywhere, Category = "Connection Config", 
		DisplayName="PEM encoding server root certs(\\n to separate lines)",
		meta = (EditCondition = "EnableServerSideTLS", EditConditionHides))
	FString ServerRootCerts;

	//Default grpc services endpoint.
	UPROPERTY(Config, EditAnywhere, Category = "Services Config")
	FString DefaultEndPoint;

	UPROPERTY(Config, EditAnywhere, Category = "Services Config")
	TMap<FString, FString> ServiceEndPoint;

	//After waiting for a duration of this times(seconds), 
	// the gRPC service object will be deleted if no other object references it.
	UPROPERTY(Config, EditAnywhere, Category = "Services Config")
	int KeepServiceAliveWithoutRefrenceSeconds = 60;

public:
	UFUNCTION(BlueprintCallable)
	FString GetServiceEndPoint(const FString& ServiceName) const;

	UFUNCTION(BlueprintCallable)
	FString GetServerRootCerts() const;

public:
	UTurboLinkGrpcConfig();
};

#if WITH_EDITOR
class FTurboLinkGrpcConfig
{
public:
	UTurboLinkGrpcConfig* GetConfig() {
		return Config.Get();
	}
private:
	TWeakObjectPtr<UTurboLinkGrpcConfig> Config;
	ISettingsSectionPtr SettingsSection;
	bool HandleSettingsSaved();
public:
	void RegisterSettings();
	void UnregisterSettings();
};
#endif
