// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_READER_READ_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_READER_READ_CQTAG_H

#include <grpc_cb/support/config.h>   // for GRPC_FINAL
#include <grpc_cb/impl/client/client_reader_read_cqtag.h>  // for ClientReaderReadCqTag

namespace grpc_cb {

// Used in ClientAsyncReaderHelper.
class ClientAsyncReaderReadCqTag GRPC_FINAL : public ClientReaderReadCqTag {
 public:
  explicit ClientAsyncReaderReadCqTag(const CallSptr& call_sptr);

  using OnRead = std::function<void()>;
  void SetOnRead(const OnRead& on_read) { on_read_ = on_read; }

  void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  OnRead on_read_;
};  // class ClientAsyncReaderReadCqTag

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_READER_READ_CQTAG_H
