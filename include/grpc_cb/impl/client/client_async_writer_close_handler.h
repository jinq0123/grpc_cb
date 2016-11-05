// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_WRITER_CLOSE_HANDLER_H
#define GRPC_CB_CLIENT_ASYNC_WRITER_CLOSE_HANDLER_H

#include <grpc_cb/support/protobuf_fwd.h>  // for Message

namespace grpc_cb {

class Status;

// Base class for close handler template class.
class ClientAsyncWriterCloseHandler {
 public:
  virtual ~ClientAsyncWriterCloseHandler() {};

  using Message = ::google::protobuf::Message;
  virtual Message& GetMessage() = 0;
  virtual void OnClose(const Status& status) = 0;
};  // class ClientAsyncWriterCloseHandler

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_ASYNC_WRITER_CLOSE_HANDLER_H
