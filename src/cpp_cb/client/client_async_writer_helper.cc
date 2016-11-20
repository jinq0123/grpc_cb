// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_helper.h"

#include <cassert>     // for assert()

#include <grpc_cb/status.h>  // for Status::ok()

#include "client_async_send_msg_cqtag.h"  // for ClientAsyncSendMsgCqTag

namespace grpc_cb {

ClientAsyncWriterHelper::ClientAsyncWriterHelper(const CallSptr& call_sptr,
  Status& status, const OnWritten& on_written)
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

bool ClientAsyncWriterHelper::WriteNext() {
  if (!status_.ok()) return false;
  if (msg_queue_.empty()) return false;
  MessageSptr msg = msg_queue_.front();
  msg_queue_.pop();

  assert(call_sptr_);
  auto* tag = new ClientAsyncSendMsgCqTag(call_sptr_, on_written_);
  if (tag->Start(*msg)) return true;

  delete tag;
  status_.SetInternalError("Failed to write client stream.");
  return false;
}

}  // namespace grpc_cb
