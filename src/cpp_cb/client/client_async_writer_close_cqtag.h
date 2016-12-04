// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_CLOSE_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_CLOSE_CQTAG_H

#include <grpc_cb/impl/client/client_writer_close_cqtag.h>  // for ClientWriterCloseCqTag
#include <grpc_cb/support/config.h>                         // for GRPC_FINAL

#include "client_async_writer_impl2_sptr.h"  // for ClientAsyncWriterImpl2Sptr

namespace grpc_cb {

class ClientAsyncWriterCloseCqTag GRPC_FINAL : public ClientWriterCloseCqTag {
 public:
  ClientAsyncWriterCloseCqTag(const CallSptr& call_sptr,
      const ClientAsyncWriterImpl2Sptr& writer_impl_sptr);
  // XXX Use ClientAsyncWriterHelperSptr instead

  void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  const ClientAsyncWriterImpl2Sptr writer_impl2_sptr_;
};  // class ClientAsyncWriterCloseCqTag

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_CLOSE_CQTAG_H
