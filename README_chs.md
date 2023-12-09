# TurboLink
[English](README.md) | 简体中文  
![logo](https://github.com/thejinchao/turbolink/wiki/image/TurboLink.png)  
TurboLink 是一个Unreal Engine插件, 能够在[Unreal Engine](https://www.unrealengine.com/) 中通过C++或者蓝图使用[Google gRPC](https://grpc.io/). 目前可以兼容UE4.27和UE5

## 特性
* 跨平台（目前已支持Windows, Linux, Android, iOS, Mac 和 PlayStation5)
* 在C++和蓝图中都可以支持异步调用gRPC函数
* 在C++中支持lambda回调函数
* 在蓝图中支持通过异步节点调用gRPC函数
* 支持流式gRPC函数
* 支持TLS加密链接
* 以protoc插件方式[代码生成工具](https://github.com/thejinchao/protoc-gen-turbolink) ，生成可以在蓝图中直接使用的代码
* 在蓝图中直接构造protobuf消息
* 支持复杂的protobuf消息结构，例如`oneof`字段以及自我引用的消息
* 所有头文件都没有引用gRPC和其他类库头文件，避免工程文件引用过多的头文件

## 范例
![example](https://github.com/thejinchao/turbolink/wiki/image/turbolink_example.png)  
有两个范例工程提供，[simple.demo.zip](https://www.dropbox.com/scl/fi/8tuipginb7dx99a0e2i2k/turbolink.simple.demo.5.3.zip?rlkey=92rsjs1b29qnd72n2a4ct0nq2&dl=0) 和 [full.demo.zip](https://www.dropbox.com/scl/fi/6hqovw8ggo6kb49d9ugbd/turbolink.full.demo.5.3.zip?rlkey=93b2y34vtnjcqjz0zq43csk0l&dl=0) ，建议您首先编译运行范例工程，以了解这个插件是如何运行的。范例工程都包含了可以直接编译运行的UE工程，以及用go语言编写的服务器工程。
### 运行本地服务器
1. 安装golang运行环境1.19
2. 确保当前目录为`TurboLink.example/Server`，然后运行命令`go mod tidy`以更新所有模块
3. 通过运行命令行`go run main.go`启动服务器
4. 在UE编辑器中，打开turbolink的设置窗口，然后设置缺省服务器为`localhost:5050`，不能使用`127.0.0.1:5050`，因为范例工程的服务器证书里没有包含这个地址
### 使用测试服务器
如果你没有合适的go语言运行环境，可以把客户端链接到我提供的一个互联网上的服务器(grpc.thecodeway.com)，我会尽量保证上面的程序一直处于运行状态。

## 入门

### 1. 安装插件
1. 从[这里](https://github.com/thejinchao/turbolink/releases) 下载一份插件的release版本
2. 你也可以用git从这个仓库克隆一份代码到本地，但需要从[这里](https://github.com/thejinchao/turbolink-libraries/releases) 下载一份已经编译好的第三方代码库，然后解压到`Source/ThirdParty`目录里
3. 在UE工程中创建`Plugin/TurboLink`这个目录，然后把这个插件代码拷贝到里面

### 2. 配置服务器地址
打开编辑器的工程设置界面(TurboLink Grpc/Services Config)，在这里可以设置不同的gRPC服务器地址  
![project-setting](https://github.com/thejinchao/turbolink/wiki/image/project-config.png)  
对于没有设置地址的服务，会使用缺省的服务器地址

### 3. 配置TLS证书
TurboLink支持服务器端的tls加密链接。 如果你要启用该功能，需要在设置界面(TurboLink Grpc/Services Config)输入服务器的证书文件内容(PEM格式)。由于UE的编辑的文本输入框只支持单行文本，所以需要把证书里的换行符用'\n'代替  
![tls-setting](https://github.com/thejinchao/turbolink/wiki/image/tls-config.png)

## 使用方法

### 1. 生成协议文件
加入你的工程里使用的协议文件`hello.proto`内容如下:
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
如果需要使用该gRPC服务，除了使用`protoc`生成`*.pb.cc` and `*.grpc.pb.cc`外，还需要生成TurboLink插件所需要的一些代码。

生成这种文件可以采用两种不同的方法：
### A. 使用Github actions
运行 [此Action](https://github.com/thejinchao/turbolink/actions/workflows/compile_proto.yml) -
点击 `Run Workflow` -> `Run Workflow`. 位于`.github/protos`目录下所有的proto文件就会被自动编译。  
编译结束后，在Summary页面的底部的Artifacts一节中会有生成的C++代码的下载连接。  
如果要编译你自己的proto文件，需要fork本工程，然后把协议文件上传到`.github/protos`目录中

### B. 在本地运行编译工具
在插件的`tools`目录，通过批处理`generate_code.cmd`可以直接生成所有这些文件。在使用这个批处理前，需要确保已经把插件拷贝到工程中，并且已经把编译后的三方库文件拷贝到插件中。运行批处理的命令格式如下:
```
generate_code.cmd <proto_file> <output_path>
```
在上面的例子中，需要以下步骤生成代码:
1. 运行如下批处理命令：`generate_code.cmd hello.proto .\output_path`
2. 把生成的代码目录`output_path`中的`Private`和`Public`目录拷贝到插件目录`YourProject/Plugins/TurboLink/Source/TurboLinkGrpc`中
3. 重新生成工程文件并编译

这个批处理使用的是protoc的插件`protoc-gen-turbolink`来生成代码，这个插件的源码在[这里](https://github.com/thejinchao/protoc-gen-turbolink). 不要把工程放在包含有空格的路径之内，以免批处理执行出现错误。  
如果你的工程中包含了多个proto文件，并且文件之间有依赖关系，那么你应该有一个根目录保存这些文件，然后以这个目录作为当前的工作路径来运行`generate_code.cmd`

### 2. 链接gRPC服务器
在C++代码中，使用如下代码连接到gRPC服务器
```cpp
UTurboLinkGrpcManager* TurboLinkManager = UTurboLinkGrpcUtilities::GetTurboLinkGrpcManager();

UGreeterService* GreeterService = Cast<UGreeterService>(TurboLinkManager->MakeService("GreeterService"));
GreeterService->Connect();
```
以上代码在蓝图中都有对应的节点可以直接使用

### 3. 调用gRPC函数
有如下几种不同的调用gRPC函数的方法

#### 3.1 通过Client对象调用
首先创建服务对应的Client对象，然后通过这个Client对象设置gRPC对应的代理委托函数
```cpp
GreeterServiceClient = GreeterService->MakeClient();
GreeterServiceClient->OnHelloResponse.AddUniqueDynamic(this, &UTurboLinkDemoCppTest::OnHelloResponse);
```
然后再调用gRPC函数
```cpp
FGrpcContextHandle CtxHello = GreeterServiceClient->InitHello();

FGrpcGreeterHelloRequest HelloRequest;
HelloRequest.Name = TEXT("Neo");

GreeterServiceClient->Hello(CtxHello, HelloRequest);
```
以上函数都有对应的蓝图节点  
![make_client](https://github.com/thejinchao/turbolink/wiki/image/make_client.png)
![call_grpc](https://github.com/thejinchao/turbolink/wiki/image/call_grpc.png)

#### 3.2 Lambda回调函数
如果是一次性调用的gRPC函数，可以再服务创建链接之后，使用使用Lambda回调函数来更快捷使用gRPC接口
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
需要注意的是，如果是客户端流式类型的函数，是无法使用Lambda回调函数的

#### 3.3 异步蓝图节点
在蓝图中，还可以使用异步蓝图节点的方式来更快捷的调用gRPC函数, 使用异步蓝图节点可以一次性完成服务链接，客户端调用回调等过程  
![async-node](https://github.com/thejinchao/turbolink/wiki/image/async-node.png)  
当前异步蓝图节点还无法支持客户端流式以及服务器端流式类型的gRPC函数

### 4. 和json互转
在某些情况下，我们需要将protobuf消息和json字符串互相转换，通过turbolink插件，这种转换也可以在蓝图中操作
#### 4.1 Grpc message to json string
![message-to-json](https://github.com/thejinchao/turbolink/wiki/image/message_to_json.png)  
结果是`{"name" : "neo"}`

#### 4.2 Json string to Grpc message
![json-to-message](https://github.com/thejinchao/turbolink/wiki/image/json_to_message.png)  


## 尚不支持的特性
TurboLink的设计目的之一，就是为了能够在蓝图中使用gRPC函数，为了达到这一目的，某些`proto3`中的特性还不被支持。
* 不要使用[`optional`](https://protobuf.dev/programming-guides/proto3/#specifying-field-rules) 类型的字段. 主要是我觉得在蓝图中实现'has_xxx'或者'clean_xxx'这样的特性会导致代码的复杂度大幅度增加，所以暂时没有实现这一特性的计划
* 同样的原因，[`any`](https://protobuf.dev/programming-guides/proto3/#any) 这样的字段目前也没有计划支持
