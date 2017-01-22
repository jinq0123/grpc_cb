// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_send_msg_cqtag.h"

#include "client_async_writer_helper.h"  // for GetCallSptr()

namespace grpc_cb {

ClientAsyncSendMsgCqTag::ClientAsyncSendMsgCqTag(const CallSptr& call_sptr)
    : ClientSendMsgCqTag(call_sptr) {}

void ClientAsyncSendMsgCqTag::DoComplete(bool success) {
  assert(success);  // Todo: check it
  if (on_written_)
    on_written_();
}

}  // namespace grpc_cb
