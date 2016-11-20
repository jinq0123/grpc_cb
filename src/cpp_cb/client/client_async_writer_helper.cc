// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_helper.h"

#include <cassert>     // for assert()

#include <grpc_cb/impl/call_sptr.h>                    // for CallSptr
#include <grpc_cb/impl/client/client_send_msg_cqtag.h>    // for ClientSendMsgCqTag
#include <grpc_cb/status.h>                 // for Status

namespace grpc_cb {

ClientAsyncWriterHelper::ClientAsyncWriterHelper(const CallSptr& call_sptr,
  Status& status,const OnWritten& on_written)
    : call_sptr_(call_sptr), status_(status), on_written_(on_written) {
  assert(call_sptr);
  assert(on_written);
}

ClientAsyncWriterHelper::~ClientAsyncWriterHelper() {}

bool ClientAsyncWriterHelper::Write(const MessageSptr& msg_sptr) {
  if (!status_.ok()) return false;

  // cache messages
  msg_queue_.push(msg_sptr);
  if (is_writing_) return true;
  return WriteNext();
}

static bool AsyncWrite(
    const CallSptr& call_sptr,
    const ::google::protobuf::Message& msg,
    Status& status) {
  assert(call_sptr);
  assert(status.ok());

  ClientSendMsgCqTag* tag = new ClientSendMsgCqTag(call_sptr);
  if (tag->Start(msg)) return true;

  delete tag;
  status.SetInternalError("Failed to write client stream.");
  return false;
}  // SayncWrite()

bool ClientAsyncWriterHelper::WriteNext() {
  if (!status_.ok()) return false;
  if (msg_queue_.empty()) return false;
  MessageSptr msg = msg_queue_.front();
  msg_queue_.pop();
  return AsyncWrite(call_sptr_, *msg, status_);
}

}  // namespace grpc_cb
