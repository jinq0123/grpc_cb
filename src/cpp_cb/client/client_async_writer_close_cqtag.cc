// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_close_cqtag.h"

#include <grpc_cb/impl/client/client_async_writer_impl.h>  // for OnClose()

namespace grpc_cb {

ClientAsyncWriterCloseCqTag::ClientAsyncWriterCloseCqTag(
    const CallSptr& call_sptr,
    const ClientAsyncWriterImplSptr& writer_impl_sptr)
    : ClientWriterCloseCqTag(call_sptr),
    writer_impl_sptr_(writer_impl_sptr) {
  assert(writer_impl_sptr);
}

void ClientAsyncWriterCloseCqTag::DoComplete(bool success) {
  assert(success);  // Todo
  assert(writer_impl_sptr_);
  writer_impl_sptr_->OnClosed(*this);

  // Todo: Add async client writer example.
}

}  // namespace grpc_cb
