// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_helper.h"

#include <cassert>     // for assert()

#include <grpc_cb/status.h>  // for Status::ok()

#include "client_async_writer_send_msg_cqtag.h"  // for ClientAsyncWriterSendMsgCqTag

namespace grpc_cb {

// Todo: thread-safe

ClientAsyncWriterHelper::ClientAsyncWriterHelper(
    const CallSptr& call_sptr, const OnEnd& on_end)
    : call_sptr_(call_sptr), on_end_(on_end) {
  assert(call_sptr);
}

ClientAsyncWriterHelper::~ClientAsyncWriterHelper() {}

bool ClientAsyncWriterHelper::Queue(const MessageSptr& msg_sptr) {
  if (aborted_)  // Maybe reader failed.
    return false;
  if (is_queue_ended_)
    return true;  // ignore

  // cache messages
  msg_queue_.emplace(msg_sptr);
  if (is_writing_) return true;
  return WriteNext();
}

void ClientAsyncWriterHelper::QueueEnd() {
  if (is_queue_ended_) return;
  if (aborted_) return;
  is_queue_ended_ = true;
  if (is_writing_) return;  // call on_end() in OnWritten()
  assert(msg_queue_.empty());
  on_end_();
}

bool ClientAsyncWriterHelper::WriteNext() {
  assert(!is_writing_);
  assert(!msg_queue_.empty());

  if (aborted_) return false;  // Maybe reader failed.
  is_writing_ = true;
  MessageSptr msg_sptr = msg_queue_.front();
  msg_queue_.pop();  // may empty now but is_writing_

  assert(call_sptr_);
  auto* tag = new ClientAsyncWriterSendMsgCqTag(call_sptr_);
  auto sptr = shared_from_this();
  tag->SetOnWritten([sptr]() { sptr->OnWritten(); });
  if (tag->Start(*msg_sptr))
    return true;

  delete tag;
  status_.SetInternalError("Failed to write client stream.");
  on_end_();  // error end
  return false;
}

void ClientAsyncWriterHelper::OnWritten() {
  assert(status_.ok());
  assert(is_writing_);
  is_writing_ = false;
  if (!msg_queue_.empty()) {
    WriteNext();
    return;
  }

  if (is_queue_ended_)
    on_end_();  // normal end
}

}  // namespace grpc_cb
