# TurboLink
![logo](https://github.com/thejinchao/turbolink/wiki/image/TurboLink.png)  
TurboLink is an unreal engine plugin enables [Google gRPC](https://grpc.io/) work with [Unreal Engine](https://www.unrealengine.com/zh-CN) using C++ and Blueprint. It is compatible with 4.27 and Unreal 5.

## Example
![example](https://github.com/thejinchao/turbolink/wiki/image/turbolink_example.png)  
An example project can be download from [this link](https://drive.google.com/file/d/1mb9lZB_ai485sbLtqrw-bk5NtlWK8fgh/view?usp=share_link)  
It is recommended that you first download this project and run it to understand how it works. This project includes a UE project that can be directly compiled and run, and a server project written in golang. If you do not have a golang runtime environment, you can also connect to the client to the server I provided (grpc.thecodway.com), I will try to keep this server running.

## Installing the plugin
1. Clone this git repository.
2. Create a `Plugins/TurboLink` folder under your project folder, then copy this repo into it.
3. Download pre-bult thirdparty binaries libraries from [here](https://github.com/thejinchao/turbolink-libraries/releases), and extract it to `Plugin/TurboLink/ThirdParty`.

