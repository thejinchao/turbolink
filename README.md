# TurboLink
![logo](https://github.com/thejinchao/turbolink/wiki/image/TurboLink.png)  
TurboLink is an unreal engine plugin enables [Google gRPC](https://grpc.io/) work with [Unreal Engine](https://www.unrealengine.com/zh-CN) using C++ and Blueprint. It is compatible with 4.27 and Unreal 5.

## Features
* Cross-platform ready.(Windows, Linux, Android, iOS, Mac and PlayStation5)
* Call rpc functions asynchronously in C++ and blueprint.
* Support lambda callback and delegate function in C++.
* Support async blueprint node to quickly call rpc functions in blueprint.
* Support TLS connection.
* A protoc-plugin code generation tool for generating protobuf code wrappers that can be used directly in blueprints.
* All public header files in the plugin do not include grpc and protobuf library header files, so that your project avoids including too many header files.

## Example
![example](https://github.com/thejinchao/turbolink/wiki/image/turbolink_example.png)  
An example project can be download from [this link](https://drive.google.com/file/d/1mb9lZB_ai485sbLtqrw-bk5NtlWK8fgh/view?usp=share_link)  
It is recommended that you first download this project and run it to understand how it works. This project includes a UE project that can be directly compiled and run, and a server project written in golang. If you do not have a golang runtime environment, you can also connect to the client to the server I provided (grpc.thecodway.com), I will try to keep this server running.

## Installing the plugin
1. Clone this git repository.
2. Create a `Plugins/TurboLink` folder under your project folder, then copy this repo into it.
3. Download pre-bult thirdparty binaries libraries from [here](https://github.com/thejinchao/turbolink-libraries/releases), and extract it to `Plugin/TurboLink/ThirdParty`.

## Generate code
In the `tools` directory of the plugin, there is a batch file called `generate_code.cmd` that is used to generate all the grpc code files. Before using it, make sure you have installed the plugin into your project and all third-party library files are installed. The command line is:
```
generate_code.cmd <proto_file> <package_name> <output_path>
```
For example, there is a grpc protocol file named `hello.proto`, package name is `Greeter`, Use the following command to generate code files:
```
generate_code.cmd hello.proto Greeter .\output_path
```
Copy generated directory `Private` and `Public` from `output_path` to `YourProject/Plugins/TurboLink/Source/TurboLinkGrpc`.
