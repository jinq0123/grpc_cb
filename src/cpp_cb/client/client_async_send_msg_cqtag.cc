// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_send_msg_cqtag.h"

namespace grpc_cb {

ClientAsyncSendMsgCqTag::ClientAsyncSendMsgCqTag(const CallSptr& call_sptr,
                                                 const OnComplete& on_complete)
    : ClientSendMsgCqTag(call_sptr), on_complete_(on_complete) {
  assert(call_sptr);
  assert(on_complete);
}

void ClientAsyncSendMsgCqTag::DoComplete(bool success) {
  assert(on_complete_);
  assert(success);  // Todo: check it
  on_complete_();
}

}  // namespace grpc_cb
