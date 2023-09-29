// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TurboLinkGrpcConfig.generated.h"

UCLASS(config = Game, DefaultConfig) // DefaultConfig means 'Save object config only to Default INIs, never to local INIs.'
class TURBOLINKGRPC_API UTurboLinkGrpcConfig : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	//Enables retry functionality. 
	//Defaults to true.  When enabled, configurable retries are enabled when they are configured via the service config.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Channel Config")
	bool EnableRetries = true;

	// After a duration of this time the client/server pings its peer to see if the
	// transport is still alive.Int valued, milliseconds.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Channel Config")
	int KeepAliveTime=45000;

	// After waiting for a duration of this time, if the keepalive ping sender does
	// not receive the ping ack, it will close the transport.Int valued, milliseconds.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Channel Config")
	int KeepAliveTimeOut=30000;

	//How many pings can we send before needing to send a
	// data / header frame ? (0 indicates that an infinite number of
	// pings can be sent without sending a data frame or header frame)
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Channel Config")
	int Http2MaxPingsWithoutData = 0;

	//Is it permissible to send keepalive pings without any outstanding streams.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Channel Config")
	bool KeepAlivePermitWithoutCalls = true;

	//Enable server-side tls connection type. Only sever needs to provide it's certificate to client.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Connection Config")
	bool EnableServerSideTLS = false;

	// The buffer containing the PEM encoding of the server root certificates.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Connection Config",
		DisplayName="PEM encoding server root certs(\\n to separate lines)",
		meta = (EditCondition = "EnableServerSideTLS", EditConditionHides))
	FString ServerRootCerts;

	//Default grpc services endpoint.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Services Config")
	FString DefaultEndPoint;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Services Config")
	TMap<FString, FString> ServiceEndPoint;

	//After waiting for a duration of this times(seconds), 
	// the gRPC service object will be deleted if no other object references it.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Services Config")
	int KeepServiceAliveWithoutRefrenceSeconds = 60;

public:
	UFUNCTION(BlueprintCallable, Category = TurboLink)
	FString GetServiceEndPoint(const FString& ServiceName) const;

	UFUNCTION(BlueprintCallable, Category = TurboLink)
	FString GetServerRootCerts() const;

public:
	UTurboLinkGrpcConfig();
};
