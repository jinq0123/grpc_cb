# gRPC_cb
C++ [gRPC](http://www.grpc.io/) library with callback interface.

(Not ready)

## Build
1. Copy [grpc](https://github.com/grpc/grpc) as third-party/grpc, and build grpc.
2. Use build/grpc_cb.sln or build/Makefile to build grpc_cb.

## Todo

* ClientSyncReader 				DONE
* ClientSyncReaderHelper		DONE
* ClientSyncWriter				DONE
* ClientSyncWriterHelper		DONE
* ClientSyncReaderWriter		DONE
* ClientSyncReaderWriterImpl	DONE
* ClientAsyncReader				DONE
* ClientAsyncReaderHelper		DONE
* ClientAsyncWriter
* ClientAsyncWriterImpl
* ClientAsyncWriterHelper
* ClientAsyncReaderWriter
* ClientAsyncReaderWriterImpl
* ServerWriter

* Check thread-safety
* Rename ...CallBack to On...
* Make impl sptr const:
  const std::shared_ptr<Impl> impl_sptr_;

