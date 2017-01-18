# gRPC_cb
C++ [gRPC](http://www.grpc.io/) library with callback interface. Depend on grpc but not on grpc++. It is a replacement of grpc++. Easier to use.

(Not ready)

## Build
1. Copy [grpc](https://github.com/grpc/grpc) as third-party/grpc, and build grpc.
	* To ease the building on Windows: [plasticbox/grpc-windows](https://github.com/plasticbox/grpc-windows)
2. Use build/grpc_cb.sln or build/Makefile to build grpc_cb.

## Done
* grpc_cpp_cb_plugin
* grpc_cb library
* helloworld example
* route_guide example

## Todo

1. Check thread-safety
1. Rename ...CallBack to On...
1. Make impl sptr const:

		const std::shared_ptr<Impl> impl_sptr_;

1. Convert grpc_byte_buffer to string, which is needed by lua.
1. Support message type other than protobuffer.

## Tutorial
### Generating client and server code
examples/protos/generate.bat is an example to generate client and server interfaces from .proto file, which runs:

		protoc -I . --cpp_out=../cpp_cb/route_guide route_guide.proto
		protoc -I . --grpc_out=../cpp_cb/route_guide --plugin=protoc-gen-grpc=grpc_cpp_cb_plugin.exe route_guide.proto

This generates the following files in directory examples/cpp_cb/route_guide
* `route_guide.pb.h`, generated message classes header
* `route_guide.pb.cc`, the implementation of message classes
* `route_guide.grpc_cb.pb.h`, generated service classes header
* `route_guide.grpc_cb.pb.cc`, the implementation of service classes

The generated namespace ```RouteGuide``` contains
* a ```Stub``` class for clients to call.
* a ```Service``` class for servers to implement.
