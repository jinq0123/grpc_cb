// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_SERVER_WRITER_IMPL_H
#define GRPC_CB_SERVER_SERVER_WRITER_IMPL_H

#include <grpc_cb/impl/call_sptr.h>  // for CallSptr
#include <grpc_cb/support/config.h>  // for GRPC_FINAL
#include <grpc_cb/support/protobuf_fwd.h>  // for Message

namespace grpc_cb {

class Status;

class ServerWriterImpl GRPC_FINAL {
 public:
  explicit ServerWriterImpl(const CallSptr& call_sptr);
  ~ServerWriterImpl();

 public:
  bool Write(const ::google::protobuf::Message& response);
  // Close() is optional. Dtr() will auto Close().
  // Redundent Close() will be ignored.
  void Close(const Status& status);
  bool IsClosed() const { return closed_; }

 private:
  CallSptr call_sptr_;
  bool closed_ = false;
  bool send_init_md = true;  // to send initial metadata once
};  // class ServerWriterImpl

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_SERVER_WRITER_IMPL_H
