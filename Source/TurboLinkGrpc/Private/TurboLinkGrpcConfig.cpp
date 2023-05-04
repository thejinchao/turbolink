// Copyright (C) Developed by Neo Jin. All Rights Reserved.
#include "TurboLinkGrpcConfig.h"

UTurboLinkGrpcConfig::UTurboLinkGrpcConfig()
{

}

FString UTurboLinkGrpcConfig::GetServiceEndPoint(const FString& ServiceName) const
{
    const FString* serviceEndPoint = ServiceEndPoint.Find(ServiceName);
    if (serviceEndPoint) {
        return *serviceEndPoint;
    }
    return DefaultEndPoint;
}

FString UTurboLinkGrpcConfig::GetServerRootCerts() const
{
    return ServerRootCerts.Replace(TEXT("\\n"), TEXT("\n"));
}
