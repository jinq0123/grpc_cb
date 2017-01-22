// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_reader_read_cqtag.h"

#include "client_async_reader_helper.h"  // for GetCallSptr()

namespace grpc_cb {

ClientAsyncReaderReadCqTag::ClientAsyncReaderReadCqTag(const CallSptr& call_sptr)
    : ClientReaderReadCqTag(call_sptr) {
}

void ClientAsyncReaderReadCqTag::DoComplete(bool success) {
  assert(success);
  if (on_read_)
    on_read_();
}

}  // namespace grpc_cb
