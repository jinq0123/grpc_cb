// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_ASYNC_READ_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_READER_ASYNC_READ_CQTAG_H

#include <grpc_cb/support/config.h>   // for GRPC_FINAL
#include <grpc_cb/status_callback.h>  // for StatusCallback
#include <grpc_cb/impl/client/client_reader_read_cqtag.h>  // for ClientReaderReadCqTag

namespace grpc_cb {

// For ClientReader and  ClientReaderWriter.
template <class Response>
class ClientReaderAsyncReadCqTag GRPC_FINAL : public ClientReaderReadCqTag {
 public:
  using MsgCallback = std::function<void (const Response&)>;
  inline explicit ClientReaderAsyncReadCqTag(const CallSptr& call_sptr,
                                        const MsgCallback& on_msg = MsgCallback(),
                                        const StatusCallback& on_end = StatusCallback())
      : ClientReaderReadCqTag(call_sptr), on_msg_(on_msg), on_end_(on_end) {}

  inline void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  void CallOnEnd(const Status& status) {
    if (on_end_) on_end_(status);
  };

 private:
  // Callback will be triggered on completion in DoComplete().
  MsgCallback on_msg_;
  StatusCallback on_end_;
};  // class ClientReaderAsyncReadCqTag

template <class Response>
void ClientReaderAsyncReadCqTag<Response>::DoComplete(bool success) {
  assert(success);

  if (!HasGotMsg()) {
    CallOnEnd(Status::OK);
    return;
  }

  Response resp;
  Status status = GetResultMsg(resp);
  if (status.ok()) {
    if (on_msg_) on_msg_(resp);
    return;
  }

  CallOnEnd(status);
}

};  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_ASYNC_READ_CQTAG_H
