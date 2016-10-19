// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_READER_ASYNC_RECV_STATUS_CQTAG_H
#define GRPC_CB_CLIENT_READER_ASYNC_RECV_STATUS_CQTAG_H

#include <grpc_cb/impl/call_sptr.h>   // for CallSptr
#include <grpc_cb/impl/client/client_reader_recv_status_cqtag.h>  // for ClientReaderRecvStatusCqTag
#include <grpc_cb/status_callback.h>  // for StatusCallback
#include <grpc_cb/support/config.h>   // for GRPC_FINAL

namespace grpc_cb {

// Recv status asynchronously for ClientReader and ClientReaderWriter.
class ClientReaderAsyncRecvStatusCqTag GRPC_FINAL
    : public ClientReaderRecvStatusCqTag {
 public:
  explicit ClientReaderAsyncRecvStatusCqTag(
      const CallSptr& call_sptr,
      const StatusCallback& on_status = StatusCallback())
      : ClientReaderRecvStatusCqTag(call_sptr), on_status_(on_status) {
    assert(call_sptr);
  }

 public:
  inline void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  StatusCallback on_status_;
};  // class ClientReaderAsyncRecvStatusCqTag

void ClientReaderAsyncRecvStatusCqTag::DoComplete(bool success) {
  assert(success);

  if (on_status_)
    on_status_(GetStatus());
}

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_READER_ASYNC_RECV_STATUS_CQTAG_H
