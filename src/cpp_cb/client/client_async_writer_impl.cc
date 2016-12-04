// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/client/client_async_writer_impl.h>

#include <cassert>  // for assert()

#include <grpc_cb/channel.h>  // for MakeSharedCall()
#include <grpc_cb/impl/client/client_async_writer_close_handler.h>  // for OnClose()
#include <grpc_cb/impl/client/client_init_md_cqtag.h>  // for ClientInitMdCqTag

#include "client_async_writer_close_cqtag.h"  // for ClientAsyncWriterCloseCqTag
#include "client_async_writer_helper.h"       // for ClientAsyncWriterHelper

namespace grpc_cb {

ClientAsyncWriterImpl::ClientAsyncWriterImpl(const ChannelSptr& channel,
                                             const std::string& method,
                                             const CompletionQueueSptr& cq_sptr)
    : cq_sptr_(cq_sptr),
      call_sptr_(channel->MakeSharedCall(method, *cq_sptr)),
      status_ok_sptr_(new std::atomic_bool{ true }) {
  assert(cq_sptr);
  assert(channel);
  ClientInitMdCqTag* tag = new ClientInitMdCqTag(call_sptr_);
  if (tag->Start()) return;
  delete tag;
  status_.SetInternalError("Failed to init client stream.");
  *status_ok_sptr_ = false;
}

ClientAsyncWriterImpl::~ClientAsyncWriterImpl() {
  // XXX Have done CallCloseHandler().
  // XXX assert(writer_uptr_->IsWritingClosed());
}

bool ClientAsyncWriterImpl::Write(const MessageSptr& request_sptr) {
  Guard g(mtx_);
  if (!status_.ok())
    return false;

  if (!writer_uptr_) {
    writer_uptr_.reset(new ClientAsyncWriterHelper(call_sptr_, is_status_ok_));
  }

  // sptr will live until written.
  auto sptr = shared_from_this();
  writer_uptr_->Write(request_sptr, [sptr]() {
    sptr->WriteNext();  // XXX no need?
  });
  return true;
}

void ClientAsyncWriterImpl::Close(const CloseHandlerSptr& handler_sptr) {
  Guard g(mtx_);

  close_handler_sptr_ = handler_sptr;

  if (!status_.ok()) {
    CallCloseHandler();
    return;
  }

  if (writer_uptr_ && writer_uptr_->IsWriting())
    return;
  CloseNow();
}

// Finally close...
void ClientAsyncWriterImpl::CloseNow() {
  if (!status_.ok()) {
    CallCloseHandler();
    return;
  }

  auto sptr = shared_from_this();
  ClientAsyncWriterCloseCqTag tag(call_sptr_, sptr);
  if (!tag.Start()) {
    status_.SetInternalError("Failed to close client stream.");
    *status_ok_sptr_ = false;  // Todo: extract SetInternalError()
    CallCloseHandler();
    return;
  }
}  // Close()

void ClientAsyncWriterImpl::WriteNext() {
  Guard g(mtx_);

  // Called from the write completion callback.
  assert(writer_uptr_);
  assert(writer_uptr_->IsWriting());
  InternalNext();
}

// Send messages one by one, and finally close.
void ClientAsyncWriterImpl::InternalNext() {
  assert(writer_uptr_);
  if (writer_uptr_->WriteNext())
    return;

  // Do not close before Close(handler).
  if (close_handler_sptr_)
    CloseNow();
}

void ClientAsyncWriterImpl::CallCloseHandler() {
  if (!close_handler_sptr_)
    return;
  if (writer_uptr_->IsWritingClosed())
    return;
  writer_uptr_->SetWritingClosed();

  close_handler_sptr_->OnClose(status_);
}

void ClientAsyncWriterImpl::OnClosed(ClientAsyncWriterCloseCqTag& tag) {
  Guard g(mtx_);

  // Todo: Get trailing metadata.
  if (tag.IsStatusOk()) {
    if (close_handler_sptr_) {
      status_ = tag.GetResponse(close_handler_sptr_->GetMsg());
    } else {
      status_.InternalError("Response is ignored.");
    }
  } else {
    status_ = tag.GetStatus();
  }

  if (!status_.ok())
    *status_ok_sptr_ = false;

  CallCloseHandler();
}

}  // namespace grpc_cb
