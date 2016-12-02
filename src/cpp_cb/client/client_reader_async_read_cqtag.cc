// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_reader_async_read_cqtag.h"

namespace grpc_cb {

ClientReaderAsyncReadCqTag::ClientReaderAsyncReadCqTag(
    const CallSptr& call_sptr,
    // XXX const MsgCallback& on_msg = MsgCallback(),
    const StatusCallback& on_end)
    : ClientReaderReadCqTag(call_sptr), on_end_(on_end) {}

void ClientReaderAsyncReadCqTag::CallOnEnd(const Status& status) {
  if (on_end_) on_end_(status);
};

void ClientReaderAsyncReadCqTag::DoComplete(bool success) {
  assert(success);

  if (!HasGotMsg()) {
    CallOnEnd(Status::OK);
    return;
  }

  // XXX use reader handler
  //Response resp;
  //Status status = GetResultMsg(resp);
  //if (status.ok()) {
  //  if (on_msg_) on_msg_(resp);
  //  return;
  //}

  // XXX CallOnEnd(status);
}

}  // namespace grpc_cb
