// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_reader_async_read_cqtag.h"

#include "client_async_reader_helper.h"  // for GetCallSptr()

namespace grpc_cb {

// XXX Rename As ClientAsyncWriterCloseCqTag

ClientReaderAsyncReadCqTag::ClientReaderAsyncReadCqTag(const CallSptr& call_sptr)
    : ClientReaderReadCqTag(call_sptr) {
}

void ClientReaderAsyncReadCqTag::DoComplete(bool success) {
  assert(success);
  if (on_read_)
    on_read_();
}

}  // namespace grpc_cb
