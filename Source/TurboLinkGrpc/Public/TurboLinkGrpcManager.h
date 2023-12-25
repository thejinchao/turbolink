// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "TurboLinkGrpcClient.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "TurboLinkGrpcManager.generated.h"

class UGrpcService;
class GrpcContext;

UCLASS(ClassGroup=TurboLink, meta = (DisplayName = " TurboLink gRPC Manager"))
class TURBOLINKGRPC_API UTurboLinkGrpcManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	//override function from UGameInstanceSubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection);
	virtual void Deinitialize();

	//override from FTickableGameObject
	virtual bool IsTickable() const override { return bIsInitialized; }
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override 	{ return GetStatID(); }

	void* GetNextTag(TSharedPtr<GrpcContext> Context);
	void RemoveTag(void* Tag);

	FGrpcContextHandle GetNextContextHandle();
public:
	class Private;
	Private* const d=nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = TurboLink)
	UGrpcService* MakeService(const FString& ServiceName);

	UFUNCTION(BlueprintCallable, Category = TurboLink)
	void ReleaseService(UGrpcService* Service);

	UFUNCTION(BlueprintCallable, Category = TurboLink)
	EGrpcServiceState GetServiceState(UGrpcService* Service);

protected:
	UPROPERTY()
	TMap<FString, UGrpcService*> WorkingService;

	UPROPERTY()
	TSet<UGrpcService*> ShutingDownService;

private:
	TMap<FString, UClass*> ServiceClassMap;

	TMap<void*, TSharedPtr<GrpcContext>> GrpcContextMap;
	uint64_t NextTag = 0;

	uint32 NextContextHandle = 0;

	bool bIsInitialized = false;
	bool bIsShutdowning = false;

public:
	UTurboLinkGrpcManager();
	virtual ~UTurboLinkGrpcManager();
};