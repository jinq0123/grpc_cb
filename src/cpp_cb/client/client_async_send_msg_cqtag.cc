// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_send_msg_cqtag.h"

#include "client_async_writer_helper.h"  // for GetCallSptr()

namespace grpc_cb {

// XXX Use callback instead of reader_sptr. As ClientAsyncWriterCloseCqTag

ClientAsyncSendMsgCqTag::ClientAsyncSendMsgCqTag(
    const ClientAsyncWriterHelperSptr writer_sptr)
    : ClientSendMsgCqTag(writer_sptr->GetCallSptr()),
      writer_sptr_(writer_sptr) {
  assert(writer_sptr);
}

void ClientAsyncSendMsgCqTag::DoComplete(bool success) {
  assert(writer_sptr_);
  assert(success);  // Todo: check it
  writer_sptr_->OnWritten();
}

}  // namespace grpc_cb
