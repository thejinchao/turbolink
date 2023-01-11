# TurboLink
![logo](https://github.com/thejinchao/turbolink/wiki/image/TurboLink.png)  
[English](README.md) | 简体中文  
TurboLink 是一个Unreal Engine插件, 能够在[Unreal Engine](https://www.unrealengine.com/) 中通过C++或者蓝图使用[Google gRPC](https://grpc.io/). 目前可以兼容UE4.27和UE5

## 特性
* 跨平台（目前已支持Windows, Linux, Android, iOS, Mac 和 PlayStation5)
* 在C++和蓝图中都可以支持异步调用gRPC函数
* 在C++中支持lambda回调函数
* 在蓝图中支持通过异步节点调用gRPC函数
* 支持流式gRPC函数
* 支持TLS加密链接
* 以protoc插件方式[代码生成工具](https://github.com/thejinchao/protoc-gen-turbolink) ，生成可以在蓝图中直接使用的代码
* 所有头文件都没有引用gRPC和其他类库头文件，避免工程文件引用过多的头文件

## 范例
![example](https://github.com/thejinchao/turbolink/wiki/image/turbolink_example.png)  
从如下链接可以下载一个Demo工程，建议您首先编译运行该工程，以了解这个插件是如何运行的。这个工程包含了一个可以直接编译运行的UE工程，一个用go语言编写的服务器工程。如果你没有合适的go语言运行环境，可以把客户端链接到我提供的一个互联网上的服务器(grpc.thecodeway.com)，我会尽量保证上面的程序一直处于运行状态。

## 入门

### 1. 安装插件
1. 下载本git工程代码
2. 在你的UE工程中创建`Plugin/TurboLink`这个目录，然后把这个插件代码拷贝到里面
3. 下载插件所需要的第三方代码库，可以从[这里](https://github.com/thejinchao/turbolink-libraries/releases) 下载一份已经编译好的库文件，然后解压到`Plugin/TurboLink/ThirdParty`目录里


