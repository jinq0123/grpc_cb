// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/client/client_async_writer_impl.h>

#include <cassert>  // for assert()

#include <google/protobuf/message.h>  // for Message

#include <grpc_cb/channel.h>         // for MakeSharedCall()
#include <grpc_cb/impl/call_sptr.h>  // for CallSptr
#include <grpc_cb/impl/client/client_async_writer_close_handler.h>  // for OnClose()
#include <grpc_cb/impl/client/client_async_writer_helper.h>  // for ClientAsyncWriterHelper
#include <grpc_cb/impl/client/client_init_md_cqtag.h>        // for ClientInitMdCqTag
#include <grpc_cb/impl/client/client_writer_finish_cqtag.h>  // for ClientWriterFinishCqTag
#include <grpc_cb/status.h>                                  // for Status

namespace grpc_cb {

ClientAsyncWriterImpl::ClientAsyncWriterImpl(const ChannelSptr& channel,
                                             const std::string& method,
                                             const CompletionQueueSptr& cq_sptr)
    : cq_sptr_(cq_sptr), call_sptr_(channel->MakeSharedCall(method, *cq_sptr)) {
  assert(cq_sptr);
  assert(channel);
  ClientInitMdCqTag* tag = new ClientInitMdCqTag(call_sptr_);
  if (tag->Start()) return;
  delete tag;
  status_.SetInternalError("Failed to init client stream.");
}

ClientAsyncWriterImpl::~ClientAsyncWriterImpl() {
  // XXX auto close if not. necessary?
}

bool ClientAsyncWriterImpl::Write(const MessageSptr& request_sptr) {
  Guard g(mtx_);
  if (!status_.ok()) return false;

  msg_queue_.push(request_sptr);
  if (is_writing_) return true;
  InternalNext();
  // XXX return ClientAsyncWriterHelper::AsyncWrite(call_sptr_, request, status_);
  return true;
}

void ClientAsyncWriterImpl::Close(const CloseHandlerSptr& handler_sptr) {
  Guard g(mtx_);
  if (!status_.ok()) {
    if (handler_sptr)
      handler_sptr->OnClose(status_);
    return;
  }

  close_handler_sptr_ = handler_sptr;
  if (is_writing_) return;
  assert(msg_queue_.empty());
  CloseNow();
}

// Finally close...
void ClientAsyncWriterImpl::CloseNow() {
  if (!status_.ok()) {
    if (close_handler_sptr_)
      close_handler_sptr_->OnClose(status_);
    return;
  }

  ClientWriterFinishCqTag tag(call_sptr_);
  if (!tag.Start()) {
    status_.SetInternalError("Failed to close client stream.");
    if (close_handler_sptr_)
      close_handler_sptr_->OnClose(status_);
    return;
  }

  // XXX get response...
  //// Todo: Get trailing metadata.
  //if (tag.IsStatusOk())
  //  status = tag.GetResponse(*response);
  //else
  //  status = tag.GetStatus();
}  // Close()

void ClientAsyncWriterImpl::Next() {
  Guard g(mtx_);
  assert(is_writing_);  // Because Next() is called from completion callback.
  InternalNext();
}

// Send messages one by one, and finally close.
void ClientAsyncWriterImpl::InternalNext() {
  if (!status_.ok() || msg_queue_.empty())
  {
    is_writing_ = false;
    // Do not close before Close(handler).
    if (close_handler_sptr_)
      CloseNow();
    return;
  }

  is_writing_ = true;
  // XXX write one message...
}

}  // namespace grpc_cb
