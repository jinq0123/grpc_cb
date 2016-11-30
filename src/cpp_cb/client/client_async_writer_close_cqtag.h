// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_CLOSE_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_CLOSE_CQTAG_H

#include <functional>  // for std::function

#include <grpc_cb/impl/client/client_writer_close_cqtag.h>  // for ClientWriterCloseCqTag
#include <grpc_cb/support/config.h>                         // for GRPC_FINAL

namespace grpc_cb {

class ClientAsyncWriterCloseCqTag GRPC_FINAL : public ClientWriterCloseCqTag {
 public:
  using OnComplete = std::function<void(ClientAsyncWriterCloseCqTag& tag)>;
  ClientAsyncWriterCloseCqTag(const CallSptr& call_sptr,
                              const OnComplete& on_complete);
  // XXX Use ClientAsyncWriterImpl sptr instead of OnComplete

  void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  const OnComplete on_complete_;
};  // class ClientAsyncWriterCloseCqTag

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_CLOSE_CQTAG_H
