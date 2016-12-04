// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_helper.h"

#include <cassert>     // for assert()

#include <grpc_cb/status.h>  // for Status::ok()

#include "client_async_send_msg_cqtag.h"  // for ClientAsyncSendMsgCqTag

namespace grpc_cb {

ClientAsyncWriterHelper::ClientAsyncWriterHelper(const CallSptr& call_sptr,
                                                 const AtomicBoolSptr& status_ok_sptr)
    : call_sptr_(call_sptr), status_ok_sptr_(status_ok_sptr) {
  assert(call_sptr);
  assert(status_ok_sptr);
}

ClientAsyncWriterHelper::~ClientAsyncWriterHelper() {}

bool ClientAsyncWriterHelper::Write(const MessageSptr& msg_sptr) {
  if (!(*status_ok_sptr_))
    return false;

  // cache messages
  msg_queue_.emplace(msg_sptr);
  if (is_writing_) return true;
  return WriteNext();
}

bool ClientAsyncWriterHelper::WriteNext() {
  if (!(*status_ok_sptr_)) return false;
  if (msg_queue_.empty()) return false;
  MessageSptr msg_sptr = msg_queue_.front();
  msg_queue_.pop();

  assert(call_sptr_);
  // XXX Use shared from this in CqTag... Rename to ClientAsyncWriterSendMsgCqTag.
  auto* tag = new ClientAsyncSendMsgCqTag(call_sptr_, [](){});  // XXX OnWritten
  if (tag->Start(*msg_sptr))
    return true;

  delete tag;
  // XXX Return status to parent... OnWriteError
  status_.SetInternalError("Failed to write client stream.");
  *status_ok_sptr_ = false;
  return false;
}

}  // namespace grpc_cb
