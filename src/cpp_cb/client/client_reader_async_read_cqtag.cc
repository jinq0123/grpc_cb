// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_reader_async_read_cqtag.h"

#include "client_async_reader_helper.h"  // for GetCallSptr()

namespace grpc_cb {

ClientReaderAsyncReadCqTag::ClientReaderAsyncReadCqTag(
    const ClientAsyncReaderHelperSptr& reader_sptr)
    : ClientReaderReadCqTag(reader_sptr->GetCallSptr()),
      reader_sptr_(reader_sptr) {
  assert(reader_sptr);
}

void ClientReaderAsyncReadCqTag::DoComplete(bool success) {
  assert(success);
  reader_sptr_->OnRead(*this);
}

}  // namespace grpc_cb
