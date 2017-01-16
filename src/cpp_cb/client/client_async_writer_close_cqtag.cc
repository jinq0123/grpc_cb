// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_close_cqtag.h"

#include "client_async_writer_impl2.h"  // for OnClose()

namespace grpc_cb {

ClientAsyncWriterCloseCqTag::ClientAsyncWriterCloseCqTag(
    const CallSptr& call_sptr) : ClientWriterCloseCqTag(call_sptr) {
}

void ClientAsyncWriterCloseCqTag::DoComplete(bool success) {
  assert(success);  // Todo
  if (on_closed_)
    on_closed_();

  // Todo: Add async client writer example.
}

}  // namespace grpc_cb
