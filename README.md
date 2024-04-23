# TurboLink
English | [简体中文](README_chs.md)  
![logo](https://github.com/thejinchao/turbolink/wiki/image/TurboLink.png)  
TurboLink is an unreal engine plugin that enables [Google gRPC](https://grpc.io/) to work with [Unreal Engine](https://www.unrealengine.com/) using C++ or Blueprint. It is compatible with UE 4.27 and 5.

## Features
* Cross-platform ready. (Windows, Linux, Android, iOS, Mac, and PlayStation5)
* Call gRPC functions asynchronously in C++ and blueprint.
* Support lambda callback and delegate function in C++.
* Support async blueprint node to quickly call gRPC functions in a blueprint.
* Support streaming gRPC methods.
* Support TLS connection.
* A [protoc-plugin code generation tool](https://github.com/thejinchao/protoc-gen-turbolink) for generating protobuf code wrappers that can be used directly in blueprint.
* Construct protobuf message through native make nodes in blueprints.
* Support complex protobuf structures such as `oneof` field and self-nesting struct.
* All public header files in the plugin do not include gRPC and protobuf library header files so that your project avoids including too many header files.

## Example
![example](https://github.com/thejinchao/turbolink/wiki/image/turbolink_example.png)  
Two example projects can be downloaded, [simple.demo.zip](https://www.dropbox.com/scl/fi/8tuipginb7dx99a0e2i2k/turbolink.simple.demo.5.3.zip?rlkey=92rsjs1b29qnd72n2a4ct0nq2&dl=0) and [full.demo.zip](https://www.dropbox.com/scl/fi/6hqovw8ggo6kb49d9ugbd/turbolink.full.demo.5.3.zip?rlkey=93b2y34vtnjcqjz0zq43csk0l&dl=0)
It is recommended that first download example projects and run them to understand how the plugin works. All demo projects include UE projects and server projects that can be run directly.  

### Run local server
1. Installl golang enviroment 1.19
2. Make sure currennt directory is `TurboLink.example/Server`, and run `go mod tidy` to update all module needed.
3. Run grpc service with command `go run main.go`
4. Open turbolink setting windows in UE editor, set default Endpoint as `localhost:5050`. Do not use `127.0.0.1:5050` because the certificate file in the sample project does not include this domain
### Public test server
If you do not have a golang runtime environment, you can connect the client to the server I provided (grpc.thecodeway.com). I will try to keep this server running.

## Geting started  

### 1. Installing the plugin
1. Download a release version from [here](https://github.com/thejinchao/turbolink/releases).
2. You can also clone this repo locally through git, but you also need to download pre-built third party binaries libraries from [here](https://github.com/thejinchao/turbolink-libraries/releases), and extract it to `Source/ThirdParty`.
3. Create a `Plugins/TurboLink` folder under your project folder, then copy this repo into it.

### 2. Config service endpoint
Open the project setting window (TurboLink Grpc/Services Config) to set the server endpoint to different gRPC services.  
![project-setting](https://github.com/thejinchao/turbolink/wiki/image/project-config.png)  
For services that do not have an endpoint set, turbolink will use the default endpoint to connect.

### 3. Config TLS certificate
Turbolink support server-side tls connection type. If you want to enable this function, you need to set the server certificate file(PEM format) in the settings windows (TurboLink Grpc/Services Config). Because UE's setting window only supports single-line text, you need to replace the newline character in the certificate file with `\n`.  
![tls-setting](https://github.com/thejinchao/turbolink/wiki/image/tls-config.png)

## Usage

### 1. Generate code from gRPC file
For example, a simple gRPC service `hello.proto` is as follows:
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
To use this service, in addition to using `protoc` to generate `*.pb.cc` and `*.grpc.pb.cc` files, you also need to generate the code files required by turbolink.

There are 2 options to do so:
### A. Using Github actions
Follow [this](https://github.com/thejinchao/turbolink/actions/workflows/compile_proto.yml) -
Click on `Run Workflow` -> `Run Workflow`. This will build all proto files in the `.github/protos` directory.  
Then at the end of the build - at the bottom of the summary page you will see the "Generated C++ Code" download link under Artifacts section.  
To use your own .proto - you can clone/fork this repo and add your proto files in the `.github/protos` directory.  

### B. Running the tool locally
In the `tools` directory of the plugin, there is a batch file called `generate_code.cmd` that is used to generate all the gRPC code files. Before using it, make sure you have installed the plugin into your project and all third-party library files are installed. The command line is:
```
generate_code.cmd <proto_file> <output_path>
```
In the proto file above, Use the following steps to generate code files:
1. Generate code file with command line: `generate_code.cmd hello.proto .\output_path`
2. Copy generated directories `Private` and `Public` from `output_path` to `YourProject/Plugins/TurboLink/Source/TurboLinkGrpc`
3. Re-generate your project solution and build it.

This batch file generates code through a protoc plugin named `protoc-gen-turbolink`, the code of this plugin can be found [here](https://github.com/thejinchao/protoc-gen-turbolink). Do not put the project in the path containing spaces to avoid errors in execution.  
If your project contains multiple proto files, and there are dependencies between files, then you should have a root directory to save these files, and then use this directory as the current working path to run `generate_code.cmd`

### 2. Connect to gRPC service
Use the following c++ code to link to the gRPC services.
```cpp
UTurboLinkGrpcManager* TurboLinkManager = UTurboLinkGrpcUtilities::GetTurboLinkGrpcManager();

UGreeterService* GreeterService = Cast<UGreeterService>(TurboLinkManager->MakeService("GreeterService"));
GreeterService->Connect();
```
The above functions can be called directly in the blueprint.

### 3. Call gRPC methods
There are several different ways of calling gRPC methods.

#### 3.1 Client object
First, create the client object, and set the delegate function.
```cpp
GreeterServiceClient = GreeterService->MakeClient();
GreeterServiceClient->OnHelloResponse.AddUniqueDynamic(this, &UTurboLinkDemoCppTest::OnHelloResponse);
```
Then create a context object and call the gRPC method.
```cpp
FGrpcContextHandle CtxHello = GreeterServiceClient->InitHello();

FGrpcGreeterHelloRequest HelloRequest;
HelloRequest.Name = TEXT("Neo");

GreeterServiceClient->Hello(CtxHello, HelloRequest);
```
The above functions can be called directly in the blueprint.
![make_client](https://github.com/thejinchao/turbolink/wiki/image/make_client.png)
![call_grpc](https://github.com/thejinchao/turbolink/wiki/image/call_grpc.png)

#### 3.2 Lambda callback
If the gRPC call is a one-off, you can use a lambda function as a callback after the service is connected.
```cpp
FGrpcGreeterHelloRequest HelloRequest;
HelloRequest.Name = TEXT("Neo");

GreeterService->CallHello(HelloRequest, 
    [this](const FGrpcResult& Result, const FGrpcGreeterHelloResponse& Response) 
    {
        if (Result.Code == EGrpcResultCode::Ok)
        {
            //Do something
        }
    }
);
```
It should be noted that if it is a function of client stream type, lambda callback cannot be used.

#### 3.3 Async blueprint node
In the blueprint, if you need to quickly test some gRPC functions, or use some one-off functions, you can use an asynchronous blueprint node, which can automatically complete the service link and callback processing.  
![async-node](https://github.com/thejinchao/turbolink/wiki/image/async-node.png)  
Currently, the async node cannot support gRPC functions of client stream and server stream types.

### 4. Conver with json string
In some cases, we need to convert protobuf messages and json strings to each other. Through the turbolink, this conversion can also be operated in the blueprint
#### 4.1 Grpc message to json string
![message-to-json](https://github.com/thejinchao/turbolink/wiki/image/message_to_json.png)  
The result is `{"name" : "neo"}`

#### 4.2 Json string to Grpc message
![json-to-message](https://github.com/thejinchao/turbolink/wiki/image/json_to_message.png)  


## Feature not yet implemented
One of the design purposes of TurboLink is to be able to use the gRPC directly in the blueprint, so some `proto3` features cannot be implemented in TurboLink yet.
* Do not use [`optional`](https://protobuf.dev/programming-guides/proto3/#specifying-field-rules) field. And I have no plan to support functions like 'has_xxx' or 'clean_xxx' in the blueprint, which will greatly increase the complexity of the generated code.
* Similarly, [`any`](https://protobuf.dev/programming-guides/proto3/#any) message type cannot be used in TurboLink either.

## Buy me a coffee
Turbolink is a completely free and open source project. I maintain it in my own free time. If you get help from it, you can consider buying me a cup of coffee. Thank you!  
<a href="https://www.buymeacoffee.com/neojin" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/default-blue.png" alt="Buy Me A Coffee" height="41" width="174"></a>
