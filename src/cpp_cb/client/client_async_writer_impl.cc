// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/client/client_async_writer_impl.h>

#include <cassert>  // for assert()

#include <google/protobuf/message.h>  // for Message

#include <grpc_cb/channel.h>         // for MakeSharedCall()
#include <grpc_cb/impl/call_sptr.h>  // for CallSptr
#include <grpc_cb/impl/client/client_async_writer_close_handler.h>  // for OnClose()
#include <grpc_cb/impl/client/client_async_writer_helper.h>  // for ClientAsyncWriterHelper
#include <grpc_cb/impl/client/client_writer_finish_cqtag.h>  // for ClientWriterFinishCqTag
#include <grpc_cb/status.h>                                  // for Status

#include "async_client_init_md_cqtag.h"  // for AsyncClientInitMdCqTag

namespace grpc_cb {

ClientAsyncWriterImpl::ClientAsyncWriterImpl(const ChannelSptr& channel,
                                             const std::string& method,
                                             const CompletionQueueSptr& cq_sptr)
    : cq_sptr_(cq_sptr), call_sptr_(channel->MakeSharedCall(method, *cq_sptr)) {
  assert(cq_sptr);
  assert(channel);
}

// Do not Init() in ctr() because we need enable_from_this().
void ClientAsyncWriterImpl::Init() {
  Guard g(mtx_);

  assert(is_idle_);
  is_idle_ = false;

  AsyncClientInitMdCqTag* tag = new AsyncClientInitMdCqTag(call_sptr_);
  auto sptr = shared_from_this();
  tag->SetCompleteCb([sptr]() { sptr->Next(); });
  if (tag->Start()) return;
  delete tag;
  status_.SetInternalError("Failed to init client stream.");
  is_idle_ = true;
}

bool ClientAsyncWriterImpl::Write(const MessageSptr& request_sptr) {
  Guard g(mtx_);
  if (!status_.ok()) return false;

  msg_queue_.push(request_sptr);
  if (is_idle_)
    InternalNext();
  return true;

  // XXX return ClientAsyncWriterHelper::AsyncWrite(call_sptr_, request, status_);
}

void ClientAsyncWriterImpl::Close(const CloseHandlerSptr& handler_sptr) {
  Guard g(mtx_);
  if (!status_.ok()) return;

  close_handler_sptr_ = handler_sptr;
  if (is_idle_)
    InternalNext();

  // XXX Finally close...
  //if (!status_.ok()) {
  //  (*handler_sptr).OnClose(status_);
  //  return;
  //}

  //ClientWriterFinishCqTag tag(call_sptr_);
  //if (!tag.Start()) {
  //  status_.SetInternalError("Failed to close client stream.");
  //  (*handler_sptr).OnClose(status_);
  //  return;
  //}

  // XXX

  //data.cq_sptr->Pluck(&tag);

  //// Todo: Get trailing metadata.
  //if (tag.IsStatusOk())
  //  status = tag.GetResponse(*response);
  //else
  //  status = tag.GetStatus();
}  // Close()

void ClientAsyncWriterImpl::Next() {
  Guard g(mtx_);
  assert(!is_idle_);  // Because Next() is called from completion callback.
  InternalNext();
}

void ClientAsyncWriterImpl::InternalNext() {
  // XXX check status?
  is_idle_ = false;
  // XXX
}

}  // namespace grpc_cb
