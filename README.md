# gRPC_cb
C++ [gRPC](http://www.grpc.io/) library with callback interface. Depend on grpc but not on grpc++. It is a replacement of grpc++. Easier to use.

(Not ready)

## Build
1. Copy [grpc](https://github.com/grpc/grpc) as third-party/grpc, and build grpc.
2. Use build/grpc_cb.sln or build/Makefile to build grpc_cb.

## Todo

* Check thread-safety
* Rename ...CallBack to On...
* Make impl sptr const:
  const std::shared_ptr<Impl> impl_sptr_;
* Convert grpc_byte_buffer to string, which is needed by lua.

