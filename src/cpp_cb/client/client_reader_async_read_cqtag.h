// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_ASYNC_READ_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_READER_ASYNC_READ_CQTAG_H

#include <grpc_cb/support/config.h>   // for GRPC_FINAL
#include <grpc_cb/status_callback.h>  // for StatusCallback
#include <grpc_cb/impl/client/client_reader_read_cqtag.h>  // for ClientReaderReadCqTag

namespace grpc_cb {

// Used in ClientAsyncReaderHelper.
// Todo: rename to ClientAsyncReaderReadCqTag
class ClientReaderAsyncReadCqTag GRPC_FINAL : public ClientReaderReadCqTag {
 public:
  // DEL using MsgCallback = std::function<void (const Response&)>;
  ClientReaderAsyncReadCqTag(const CallSptr& call_sptr,
                             // DEL const MsgCallback& on_msg = MsgCallback(),
                             const StatusCallback& on_end = StatusCallback());

  void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  void CallOnEnd(const Status& status);

 private:
  // Callback will be triggered on completion in DoComplete().
  // XXXX MsgCallback on_msg_;
  StatusCallback on_end_;
};  // class ClientReaderAsyncReadCqTag

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CLIENT_READER_ASYNC_READ_CQTAG_H
