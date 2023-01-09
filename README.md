# TurboLink
![logo](https://github.com/thejinchao/turbolink/wiki/image/TurboLink.png)  
TurboLink is an unreal engine plugin enables [Google gRPC](https://grpc.io/) work with [Unreal Engine](https://www.unrealengine.com/zh-CN) using C++ and Blueprint. It is compatible with UE 4.27 and 5.

## Features
* Cross-platform ready.(Windows, Linux, Android, iOS, Mac and PlayStation5)
* Call rpc functions asynchronously in C++ and blueprint.
* Support lambda callback and delegate function in C++.
* Support async blueprint node to quickly call rpc functions in blueprint.
* Support streaming grpc methods.
* Support TLS connection.
* A [protoc-plugin code generation tool](https://github.com/thejinchao/protoc-gen-turbolink) for generating protobuf code wrappers that can be used directly in blueprints.
* All public header files in the plugin do not include grpc and protobuf library header files, so that your project avoids including too many header files.

## Example
![example](https://github.com/thejinchao/turbolink/wiki/image/turbolink_example.png)  
An example project can be download from [this link](https://drive.google.com/file/d/1mb9lZB_ai485sbLtqrw-bk5NtlWK8fgh/view?usp=share_link)  
It is recommended that you first download this project and run it to understand how it works. This project includes a UE project that can be directly compiled and run, and a server project written in golang. If you do not have a golang runtime environment, you can also connect to the client to the server I provided (grpc.thecodway.com), I will try to keep this server running.

## Geting started  

### 1. Installing the plugin
1. Clone this git repository.
2. Create a `Plugins/TurboLink` folder under your project folder, then copy this repo into it.
3. Download pre-bult thirdparty binaries libraries from [here](https://github.com/thejinchao/turbolink-libraries/releases), and extract it to `Plugin/TurboLink/ThirdParty`.

### 2. Config service endpoint
Open the project setting window (TurboLink Grpc/Services Config) to set the server endpoint to different grpc services.  
![project-setting](https://github.com/thejinchao/turbolink/wiki/image/project-config.png)  
For services that do not have an endpoint set, turbolink will use the default endpoint to connect.

### 3. Config TLS certificate
Turbolink support server-side tls connection type. If you want to enable this function, you need set the server certificate file(PEM format) in the settings windows (TurboLink Grpc/Services Config). Because UE's setting window only supports single-line text, you need to replace the newline character in the certificate file with '\n'.  
![tls-setting](https://github.com/thejinchao/turbolink/wiki/image/tls-config.png)

## Usage

### 1. Generate code from grpc file
For example, a simple grpc service `hello.proto` is as follows:
```protobuf
syntax = "proto3";

package Greeter;
option go_package = "./Greeter";

message HelloRequest {
	string name = 1;
}
message HelloResponse {
	string reply_message = 1;
}
service GreeterService {
	rpc Hello (HelloRequest) returns (HelloResponse);
}
```
To use this service, in addition to using `protoc` to generate `*.pb.cc` and `*.grpc.pb.cc` files, you also need to generate the code files required by turbolink. In the `tools` directory of the plugin, there is a batch file called `generate_code.cmd` that is used to generate all the grpc code files. Before using it, make sure you have installed the plugin into your project and all third-party library files are installed. The command line is:
```
generate_code.cmd <proto_file> <package_name> <output_path>
```
In the proto file above, package name is `Greeter`, Use the following steps to generate code files:
1. Generate code file with command line: `generate_code.cmd hello.proto Greeter .\output_path`
2. Copy generated directory `Private` and `Public` from `output_path` to `YourProject/Plugins/TurboLink/Source/TurboLinkGrpc`.
3. Re-generate your project solution and build it.

This batch file generates code through a protoc plugin named `protoc-gen-turbolink`, the code of this plugin can be found [here](https://github.com/thejinchao/protoc-gen-turbolink)

### 2. Connect to gRPC service
Use the following c++ code to link to the gRPC services.
```cpp
UTurboLinkGrpcManager* TurboLinkManager = UTurboLinkGrpcUtilities::GetTurboLinkGrpcManager();

UGreeterService* GreeterService = Cast<UGreeterService>(TurboLinkManager->MakeService("GreeterService"));
GreeterService->Connect();
```
The above functions can be called directly in the blueprint.

### 3. Call gRPC methods
There are several different ways of calling grpc methods.

#### 3.1 Client object
First create the client object, and set the delegate function.
```cpp
GreeterServiceClient = GreeterService->MakeClient();
GreeterServiceClient->OnHelloResponse.AddUniqueDynamic(this, &UTurboLinkDemoCppTest::OnHelloResponse);
```
Then create a context object and call the grpc method.
```cpp
FGrpcContextHandle CtxHello = GreeterServiceClient->InitHello();

FGrpcGreeterHelloRequest HelloRequest;
HelloRequest.Name = TEXT("Neo");

GreeterServiceClient->Hello(CtxHello, HelloRequest);
```
The above functions can be called directly in the blueprint.

#### 3.2 Lambda callback
If the grpc call is one-off, you can use a lambda function as a callback after service connected.
```cpp
FGrpcGreeterHelloRequest HelloRequest;
HelloRequest.Name = TEXT("Neo");

GreeterService->CallHello(HelloRequest, [this](const FGrpcResult& Result, const FGrpcGreeterHelloResponse& Response) {
    if (Result.Code == EGrpcResultCode::Ok)
    {
        //Do something
    }
});
```
It should be noted that if it is a function of client stream type, lambda callback cannot be used.

#### 3.3 Async blueprint node
In the blueprint, if you need to quickly test some grpc functions, or use some one-off functions, you can use asynchronous blueprint node, which can automatically complete the service link and callback processing.  
![async-node](https://github.com/thejinchao/turbolink/wiki/image/async-node.png)  
Currently, async node cannot support grpc functions of client stream and server stream types.
