#pragma once

#include "CoreMinimal.h"
#include "TurboLinkGrpcClient.h"
#include "TurboLinkGrpcModule.h"

#include <grpcpp/grpcpp.h>

class UGrpcService;
class UGrpcClient;

class GrpcContext : public TSharedFromThis<GrpcContext>
{
	friend class UTurboLinkGrpcManager;
	friend class UGrpcClient;
public:
    FGrpcContextHandle GetHandle() const
    {
        return Handle;
    }

    EGrpcContextState GetState() const
    {
        return ContextState;
    }

    void TryCancel();

public:
    UGrpcService* const Service;
    UGrpcClient* const Client;

    // Context for the client. It could be used to convey extra information to the server and/or tweak certain RPC behaviors.
    std::unique_ptr<grpc::ClientContext> RpcContext;
    // Storage for the status of the RPC upon completion.
    grpc::Status RpcStatus;

	// Convert grpc status code to EGrpcResultCode
	static EGrpcResultCode ConvertStatusCode(const grpc::Status& RpcStatus);

	// Build grpc Result
	static FGrpcResult MakeGrpcResult(const grpc::Status& RpcStatus);

    //async tag
    void* InitialTag = nullptr;
    void* WriteTag = nullptr;
    void* ReadTag = nullptr;

protected:
    void UpdateState(EGrpcContextState NewState);
	virtual void OnRpcEvent(bool Ok, const void* EventTag) = 0;

protected:
    FGrpcContextHandle Handle;
    EGrpcContextState ContextState;

public:
	GrpcContext(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client);
    virtual ~GrpcContext();
};

template<typename T, typename R>
class TGrpcContext : public GrpcContext
{
public:
	typedef std::function<void(const FGrpcResult&, R*)> FRpcCallbackFunc;

	//Reader and Writer
	std::unique_ptr<T> RpcReaderWriter;
	//Response Message
	R RpcResponse;

public:
	TGrpcContext(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client)
		: GrpcContext(_Handle, _Service, _Client)
	{
	}
};

template<typename T, typename R>
class GrpcContext_Ping_Pong : public TGrpcContext<T, R>
{
	typedef TGrpcContext<T, R> Super;
	
protected:
	void OnRpcEventInternal(bool Ok, const void* EventTag, typename Super::FRpcCallbackFunc RpcCallbackFunc)
	{
		if (!Ok)
		{
			Super::RpcReaderWriter->Finish(&(Super::RpcResponse), &(Super::RpcStatus), Super::ReadTag);
			return;
		}

		FGrpcResult result = GrpcContext::MakeGrpcResult(Super::RpcStatus);
		if (Super::RpcStatus.ok())
		{
			if (Super::GetState() == EGrpcContextState::Initialing)
			{
				check(EventTag == Super::InitialTag);

				Super::RpcReaderWriter->Finish(&(Super::RpcResponse), &(Super::RpcStatus), Super::ReadTag);
				Super::UpdateState(EGrpcContextState::Busy);
			}
			else
			{
				if (RpcCallbackFunc)
				{
					RpcCallbackFunc(result, &(Super::RpcResponse));
				}
				Super::UpdateState(EGrpcContextState::Done);
				return;
			}
		}
		else
		{
			UE_LOG(LogTurboLink, Error, TEXT("CallRpcError: %s"), *result.GetMessageString());

			if (RpcCallbackFunc)
			{
				RpcCallbackFunc(result, nullptr);
			}
			Super::UpdateState(EGrpcContextState::Done);
			return;
		}
	}

public:
	GrpcContext_Ping_Pong(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client)
		: Super(_Handle, _Service, _Client)
	{
	}
};

template<typename T, typename R>
class GrpcContext_Ping_Stream : public TGrpcContext<T, R>
{
	typedef TGrpcContext<T, R> Super;
protected:
	void OnRpcEventInternal(bool Ok, const void* EventTag, typename Super::FRpcCallbackFunc RpcCallbackFunc)
	{
		if (!Ok)
		{
			Super::RpcReaderWriter->Finish(&(Super::RpcStatus), Super::ReadTag);
			Super::UpdateState(EGrpcContextState::Done);
			return;
		}

		FGrpcResult result = GrpcContext::MakeGrpcResult(Super::RpcStatus);
		if (Super::RpcStatus.ok())
		{
			if (Super::GetState() == EGrpcContextState::Initialing)
			{
				check(EventTag == Super::InitialTag);

				Super::RpcReaderWriter->Read(&(Super::RpcResponse), Super::ReadTag);
				Super::UpdateState(EGrpcContextState::Busy);
			}
			else
			{
				if (RpcCallbackFunc)
				{
					RpcCallbackFunc(result, &(Super::RpcResponse));
				}
				Super::RpcReaderWriter->Read(&(Super::RpcResponse), Super::ReadTag);
			}
		}
		else
		{
			UE_LOG(LogTurboLink, Error, TEXT("CallRpcError: %s"), *result.GetMessageString());

			if (RpcCallbackFunc)
			{
				RpcCallbackFunc(result, nullptr);
			}
			Super::UpdateState(EGrpcContextState::Done);
			return;
		}
	}
public:
	GrpcContext_Ping_Stream(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client)
		: Super(_Handle, _Service, _Client)
	{
	}
};

/*TODO: Not complete yet...

template<typename T, typename R>
class GrpcContext_Stream_Pong : public TGrpcContext<T, R>
{
};
*/

template<typename T, typename R, typename S>
class GrpcContext_Stream_Stream : public TGrpcContext<T, R>
{
	typedef TGrpcContext<T, R> Super;
protected:
	std::vector<S> SendQueue;
	bool CanSend = false;

protected:
	void OnRpcEventInternal(bool Ok, const void* EventTag, typename Super::FRpcCallbackFunc RpcCallbackFunc)
	{
		if (!Ok)
		{
			Super::RpcReaderWriter->Finish(&(Super::RpcStatus), Super::ReadTag);
			return;
		}

		FGrpcResult result = GrpcContext::MakeGrpcResult(Super::RpcStatus);
		if (Super::RpcStatus.ok())
		{
			if (Super::GetState() == EGrpcContextState::Initialing)
			{
				check(EventTag == Super::InitialTag);

				if (SendQueue.empty())
				{
					CanSend = true;
				}
				else
				{
					Super::RpcReaderWriter->Write(SendQueue.front(), Super::WriteTag);
					SendQueue.erase(SendQueue.begin());
				}
				Super::RpcReaderWriter->Read(&(Super::RpcResponse), Super::ReadTag);
				Super::UpdateState(EGrpcContextState::Busy);
			}
			else
			{
				if (EventTag == Super::ReadTag)
				{
					if (RpcCallbackFunc)
					{
						RpcCallbackFunc(result, &(Super::RpcResponse));
					}
					Super::RpcReaderWriter->Read(&(Super::RpcResponse), Super::ReadTag);
				}
				else if (EventTag == Super::WriteTag)
				{
					if (SendQueue.empty())
					{
						CanSend = true;
					}
					else
					{
						Super::RpcReaderWriter->Write(SendQueue.front(), Super::WriteTag);
						SendQueue.erase(SendQueue.begin());
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTurboLink, Error, TEXT("CallRpcError: %s"), *result.GetMessageString());

			if (RpcCallbackFunc)
			{
				RpcCallbackFunc(result, &(Super::RpcResponse));
			}
			Super::UpdateState(EGrpcContextState::Done);
			return;
		}
	}

public:
	GrpcContext_Stream_Stream(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client)
		: Super(_Handle, _Service, _Client)
	{
	}
};
