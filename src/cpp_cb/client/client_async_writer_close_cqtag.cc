// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_close_cqtag.h"

namespace grpc_cb {

ClientAsyncWriterCloseCqTag::ClientAsyncWriterCloseCqTag(
    const CallSptr& call_sptr, const OnComplete& on_complete)
    : ClientWriterCloseCqTag(call_sptr), on_complete_(on_complete) {
}

void ClientAsyncWriterCloseCqTag::DoComplete(bool success) {
  assert(success);  // Todo
  if (on_complete_)
    on_complete_(*this);

  // Todo: Add async client writer example.
}

}  // namespace grpc_cb
