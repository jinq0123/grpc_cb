// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_helper.h"

#include <cassert>     // for assert()

#include <grpc_cb/status.h>  // for Status::ok()

#include "client_async_send_msg_cqtag.h"  // for ClientAsyncSendMsgCqTag

namespace grpc_cb {

ClientAsyncWriterHelper::ClientAsyncWriterHelper(
    const CallSptr& call_sptr, const OnEnd& on_end)
    : call_sptr_(call_sptr), on_end_(on_end) {
  assert(call_sptr);
}

ClientAsyncWriterHelper::~ClientAsyncWriterHelper() {}

bool ClientAsyncWriterHelper::Write(const MessageSptr& msg_sptr) {
  if (aborted_)  // Maybe reader failed.
    return false;

  // cache messages
  msg_queue_.emplace(msg_sptr);
  if (is_writing_) return true;
  return WriteNext();
}

bool ClientAsyncWriterHelper::WriteNext() {
  if (aborted_) return false;  // Maybe reader failed.
  if (msg_queue_.empty()) return false;
  MessageSptr msg_sptr = msg_queue_.front();
  msg_queue_.pop();

  assert(call_sptr_);
  auto sptr = shared_from_this();  // Todo: Rename to ClientAsyncWriterSendMsgCqTag.
  auto* tag = new ClientAsyncSendMsgCqTag(sptr);
  if (tag->Start(*msg_sptr))
    return true;

  delete tag;
  status_.SetInternalError("Failed to write client stream.");
  on_end_();
  return false;
}

void ClientAsyncWriterHelper::OnWritten() {
    // XXX
}

}  // namespace grpc_cb
