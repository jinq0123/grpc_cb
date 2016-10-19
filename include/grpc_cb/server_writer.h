// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_SERVER_WRITER_H
#define GRPC_CB_SERVER_SERVER_WRITER_H

#include <grpc_cb/impl/server/server_writer_impl.h>  // for ServerWriterImpl

namespace grpc_cb {

// Copyable.
template <class Response>
class ServerWriter GRPC_FINAL {
 public:
  inline ServerWriter(const CallSptr& call_sptr);

 public:
  inline bool Write(const Response& response) const;
  // Close() is optional. Dtr() will auto Close().
  // Redundent Close() will be ignored.
  inline void Close(const Status& status) const { impl_sptr_->Close(status); }
  inline bool IsClosed() const { return impl_sptr_->IsClosed(); }

 private:
  std::shared_ptr<ServerWriterImpl> impl_sptr_;
};  // class ServerWriter<>

template <class Response>
ServerWriter<Response>::ServerWriter(const CallSptr& call_sptr)
    : impl_sptr_(new ServerWriterImpl(call_sptr)) {
  assert(call_sptr);
}

template <class Response>
bool ServerWriter<Response>::Write(const Response& response) const {
  return impl_sptr_->Write(response);
}

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_SERVER_WRITER_H
