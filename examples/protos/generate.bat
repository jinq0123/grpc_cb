REM Generate source files from proto files.

set PROTOC=..\..\third_party\grpc\third_party\protobuf\cmake\Release\protoc.exe
%PROTOC% -I. --cpp_out=..\cpp_cb\helloworld helloworld.proto
%PROTOC% -I. --cpp_out=..\cpp_cb\route_guide route_guide.proto

pause
