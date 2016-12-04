// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_helper.h"

#include <cassert>     // for assert()

#include <grpc_cb/status.h>  // for Status::ok()

#include "client_async_send_msg_cqtag.h"  // for ClientAsyncSendMsgCqTag

namespace grpc_cb {

ClientAsyncWriterHelper::ClientAsyncWriterHelper(const CallSptr& call_sptr,
                                                 std::atomic_bool& is_status_ok)
    : call_sptr_(call_sptr), is_status_ok_(is_status_ok) {
  assert(call_sptr);
}

ClientAsyncWriterHelper::~ClientAsyncWriterHelper() {}

bool ClientAsyncWriterHelper::Write(const MessageSptr& msg_sptr,
                                    const OnWritten& on_written) {
  if (!is_status_ok_) return false;

  // cache messages
  queue_.emplace(WritingTask{msg_sptr, on_written});
  if (is_writing_) return true;
  return WriteNext();
}

bool ClientAsyncWriterHelper::WriteNext() {
  if (!is_status_ok_) return false;
  if (queue_.empty()) return false;
  // Keep a copy of on_written to delay dtr().
  WritingTask task = queue_.front();
  queue_.pop();

  assert(call_sptr_);
  // XXX Use shared from this in CqTag... Rename to ClientAsyncWriterSendMsgCqTag.
  auto* tag = new ClientAsyncSendMsgCqTag(call_sptr_, task.on_written);
  if (tag->Start(*task.msg_sptr))
    return true;

  delete tag;
  // XXX Return status to parent... OnWriteError
  status_.SetInternalError("Failed to write client stream.");
  is_status_ok_ = false;
  return false;
}

}  // namespace grpc_cb
