// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_CLOSE_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_CLOSE_CQTAG_H

#include <functional>  // for function<>

#include <grpc_cb/impl/client/client_writer_close_cqtag.h>  // for ClientWriterCloseCqTag
#include <grpc_cb/support/config.h>                         // for GRPC_FINAL

namespace grpc_cb {

class ClientAsyncWriterCloseCqTag GRPC_FINAL : public ClientWriterCloseCqTag {
 public:
  explicit ClientAsyncWriterCloseCqTag(const CallSptr& call_sptr);

  using OnClosed = std::function<void ()>;
  void SetOnClosed(const OnClosed& on_closed) { on_closed_ = on_closed; }
  // Todo: Change callback in ctr() to SetCb(), because callback need to reference this.

  void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  OnClosed on_closed_;
};  // class ClientAsyncWriterCloseCqTag

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_CLOSE_CQTAG_H
