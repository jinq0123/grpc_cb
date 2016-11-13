// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_READ_HANDLER_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_READ_HANDLER_H

namespace grpc_cb {

// As the base class of handler template class.
class ClientAsyncReadHandler {
 public:
  ClientAsyncReadHandler() {}
  virtual ~ClientAsyncReadHandler() {}
};  // class ClientAsyncReadHandler

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_READ_HANDLER_H
