#include "TurboLinkGrpcContext.h"
#include "TurboLinkGrpcService.h"
#include "TurboLinkGrpcManager.h"
#include "TurboLinkGrpcModule.h"

GrpcContext::GrpcContext(FGrpcContextHandle _Handle, UGrpcService* _Service, UGrpcClient* _Client)
	: Service(_Service)
	, Client(_Client)
	, Handle(_Handle)
	, ContextState(EGrpcContextState::Ready)
{
	UE_LOG(LogTurboLink, Verbose, TEXT("Construct GrpcContext"));
}

GrpcContext::~GrpcContext()
{
	UE_LOG(LogTurboLink, Verbose, TEXT("Destruct GrpcContext"));
}

void GrpcContext::UpdateState(EGrpcContextState NewState)
{
	if (GetState() == NewState) return;

	ContextState = NewState;
	if (ContextState == EGrpcContextState::Initialing)
	{
		InitialTag = Service->TurboLinkManager->GetNextTag(AsShared());
		WriteTag = Service->TurboLinkManager->GetNextTag(AsShared());
		ReadTag = Service->TurboLinkManager->GetNextTag(AsShared());
	}
	else if (ContextState == EGrpcContextState::Done)
	{
		Service->TurboLinkManager->RemoveTag(InitialTag);
		Service->TurboLinkManager->RemoveTag(WriteTag);
		Service->TurboLinkManager->RemoveTag(ReadTag);
	}

	if (Client->OnContextStateChange.IsBound())
	{
		Client->OnContextStateChange.Broadcast(Handle, NewState);
	}
}

void GrpcContext::TryCancel()
{
	if (GetState() == EGrpcContextState::Ready || GetState() == EGrpcContextState::Done)
	{
		return;
	}

	RpcContext->TryCancel();
}

EGrpcResultCode GrpcContext::ConvertStatusCode(const grpc::Status& RpcStatus)
{
	grpc::StatusCode errorCode = RpcStatus.error_code();
	switch (errorCode)
	{
	case grpc::StatusCode::OK:
		return EGrpcResultCode::Ok;

	case grpc::StatusCode::CANCELLED:
		return EGrpcResultCode::Cancelled;

	case grpc::StatusCode::UNKNOWN:
		return EGrpcResultCode::Unknown;

	case grpc::StatusCode::INVALID_ARGUMENT:
		return EGrpcResultCode::InvalidArgument;

	case grpc::StatusCode::DEADLINE_EXCEEDED:
		return EGrpcResultCode::DeadlineExceeded;

	case grpc::StatusCode::NOT_FOUND:
		return EGrpcResultCode::NotFound;
		
	case grpc::StatusCode::ALREADY_EXISTS:
		return EGrpcResultCode::AlreadyExists;

	case grpc::StatusCode::PERMISSION_DENIED:
		return EGrpcResultCode::PermissionDenied;

	case grpc::StatusCode::UNAUTHENTICATED:
		return EGrpcResultCode::Unauthenticated;

	case grpc::StatusCode::RESOURCE_EXHAUSTED:
		return EGrpcResultCode::ResourceExhausted;

	case grpc::StatusCode::FAILED_PRECONDITION:
		return EGrpcResultCode::FailedPrecondition;

	case grpc::StatusCode::ABORTED:
		return EGrpcResultCode::Aborted;

	case grpc::StatusCode::OUT_OF_RANGE:
		return EGrpcResultCode::OutOfRange;

	case grpc::StatusCode::UNIMPLEMENTED:
		return EGrpcResultCode::Unimplemented;

	case grpc::StatusCode::INTERNAL:
		return EGrpcResultCode::Internal;

	case grpc::StatusCode::UNAVAILABLE:
		return EGrpcResultCode::Unavailable;

	case grpc::StatusCode::DATA_LOSS:
		return EGrpcResultCode::DataLoss;

	default:
		return EGrpcResultCode::NotDefined;
	}
}

FGrpcResult GrpcContext::MakeGrpcResult(const grpc::Status& RpcStatus)
{
	EGrpcResultCode errorCode = ConvertStatusCode(RpcStatus);
	FString message = UTF8_TO_TCHAR(RpcStatus.error_message().c_str());

	return FGrpcResult(errorCode, message);
}