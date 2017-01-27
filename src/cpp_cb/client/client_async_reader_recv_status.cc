// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_reader_recv_status.h"

#include <cassert>

#include <grpc_cb/impl/client/client_reader_async_recv_status_cqtag.h>  // for ClientReaderAsyncRecvStatusCqTag
#include <grpc_cb/status.h>  // for Status

namespace grpc_cb {
namespace ClientAsyncReader {

void RecvStatus(const CallSptr& call_sptr, const StatusCallback& on_status) {
  assert(call_sptr);

  auto* tag = new ClientReaderAsyncRecvStatusCqTag(call_sptr);
  tag->SetOnStatus(on_status);
  if (tag->Start()) return;

  delete tag;
  if (on_status)
    on_status(Status::InternalError("Failed to receive status."));
}

}  // namespace ClientAsyncReader
}  // namespace grpc_cb
