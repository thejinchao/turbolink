#pragma once

#include "CoreMinimal.h"
#include "TurboLinkGrpcService.generated.h"

class UTurboLinkGrpcManager;
class UGrpcClient;

UENUM(BlueprintType)
enum class EGrpcServiceState : uint8
{
	NotCreate = 0,

	Idle = 1,
	Connecting = 2,
	Ready = 3,
	TransientFailure = 4,
	Shutdown = 5,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServiceStateChanged, EGrpcServiceState, ServiceState);

UCLASS(ClassGroup = TurboLink, BlueprintType, Abstract)
class TURBOLINKGRPC_API UGrpcService : public UObject
{
	GENERATED_BODY()
	friend class UTurboLinkGrpcManager;
	friend class UGrpcClient;

public:
	virtual void Connect() {
		check(0 && "Must override this");
	}
	
	virtual EGrpcServiceState GetServiceState() const {
		check(0 && "Must override this");
		return EGrpcServiceState::NotCreate;
	}

	UFUNCTION(BlueprintCallable)
	virtual void Shutdown();

protected:
	template<typename T>
	T* MakeClient()
	{
		auto client = NewObject<T>(this);
		client->Service = this;

		AddClient(client);
		return client;
	}

	void AddClient(UGrpcClient* Client);
	void RemoveClient(UGrpcClient* Client);

	void Tick(float DeltaTime);

public:
	UPROPERTY()
	UTurboLinkGrpcManager* TurboLinkManager;
	
	bool bIsShutdowning = false;

	UPROPERTY(BlueprintAssignable)
	FOnServiceStateChanged OnServiceStateChanged;

	UPROPERTY()
	TSet<UGrpcClient*> ClientSet;

public:
	UGrpcService();
	virtual ~UGrpcService();
};
