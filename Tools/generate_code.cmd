@echo off

::make sure input file and output path exist
set INPUT_PROTO_FILE=%1
if not exist %INPUT_PROTO_FILE% (
	echo Input proto file '%INPUT_PROTO_FILE%' not exist!
	exit /b 1
)

::get package name
set PACKAGE_NAME=%2

::make sure output path exist
set OUTPUT_PATH=%3
if not exist %OUTPUT_PATH% (
	echo Output path '%OUTPUT_PATH%' not exist!
	exit /b 1
)

::get turbolink plugin path
pushd %~dp0\..
set TL_UE_PLUGIN_PATH=%CD%
popd

set PROTOC_EXE_PATH=%TL_UE_PLUGIN_PATH%\ThirdParty\protobuf\bin\protoc.exe
set GRPC_CPP_PLUGIN_EXE_PATH=%TL_UE_PLUGIN_PATH%\ThirdParty\grpc\bin\grpc_cpp_plugin.exe
set TURBOLINK_PLUGIN_PATH=%TL_UE_PLUGIN_PATH%\Tools\protoc-gen-turbolink.exe
set FIX_PROTO_CPP=%TL_UE_PLUGIN_PATH%\Tools\fix_proto_cpp.txt
set FIX_PROTO_H=%TL_UE_PLUGIN_PATH%\Tools\fix_proto_h.txt

call :CallGrpc %INPUT_PROTO_FILE% 
goto :eof

:CallGrpc
set PROTO_CPP_FILE_NAME=%~n1.pb.cc
set PROTO_H_FILE_NAME=%~n1.pb.h

set CPP_OUTPUT_PATH=%OUTPUT_PATH%\Private\pb
if not exist %CPP_OUTPUT_PATH% mkdir %CPP_OUTPUT_PATH%

"%PROTOC_EXE_PATH%" ^
 --cpp_out="%CPP_OUTPUT_PATH%" ^
 --plugin=protoc-gen-grpc="%GRPC_CPP_PLUGIN_EXE_PATH%" --grpc_out=%CPP_OUTPUT_PATH% ^
 --plugin=protoc-gen-turbolink="%TURBOLINK_PLUGIN_PATH%" --turbolink_out="%OUTPUT_PATH%" ^
 %INPUT_PROTO_FILE%

::Fix compile warning
pushd %CPP_OUTPUT_PATH%

copy /b %FIX_PROTO_CPP%+%PROTO_CPP_FILE_NAME% %PROTO_CPP_FILE_NAME%.tmp
del /f %PROTO_CPP_FILE_NAME%
rename %PROTO_CPP_FILE_NAME%.tmp %PROTO_CPP_FILE_NAME%

copy /b %FIX_PROTO_H%+%PROTO_H_FILE_NAME% %PROTO_H_FILE_NAME%.tmp
del /f %PROTO_H_FILE_NAME%
rename %PROTO_H_FILE_NAME%.tmp %PROTO_H_FILE_NAME%

popd 

goto :eof
