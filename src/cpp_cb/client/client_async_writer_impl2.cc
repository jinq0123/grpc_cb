// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_impl2.h"

#include <cassert>  // for assert()

#include <grpc_cb/channel.h>  // for MakeSharedCall()
#include <grpc_cb/impl/client/client_async_writer_close_handler.h>  // for OnClose()
#include <grpc_cb/impl/client/client_init_md_cqtag.h>  // for ClientInitMdCqTag

#include "client_async_writer_close_cqtag.h"  // for ClientAsyncWriterCloseCqTag
#include "client_async_writer_helper.h"       // for ClientAsyncWriterHelper

namespace grpc_cb {

ClientAsyncWriterImpl2::ClientAsyncWriterImpl2(const ChannelSptr& channel,
                                             const std::string& method,
                                             const CompletionQueueSptr& cq_sptr)
    : cq_sptr_(cq_sptr),
      call_sptr_(channel->MakeSharedCall(method, *cq_sptr)) {
  assert(cq_sptr);
  assert(channel);
  ClientInitMdCqTag* tag = new ClientInitMdCqTag(call_sptr_);
  if (tag->Start()) return;
  delete tag;
  status_.SetInternalError("Failed to init client stream.");
}

ClientAsyncWriterImpl2::~ClientAsyncWriterImpl2() {
  // XXX Have done CallCloseHandler().
  // XXX assert(writer_uptr_->IsWritingClosed());
}

bool ClientAsyncWriterImpl2::Write(const MessageSptr& request_sptr) {
  Guard g(mtx_);
  if (!status_.ok())
    return false;

  if (!writer_sptr_) {
    // Impl2 and WriterHelper shared each other untill OnEnd().
    auto sptr = shared_from_this();
    writer_sptr_.reset(new ClientAsyncWriterHelper(call_sptr_,
        [sptr]() { sptr->OnEndOfWriting(); }));
  }

  return writer_sptr_->Write(request_sptr);
}

void ClientAsyncWriterImpl2::Close(const CloseHandlerSptr& handler_sptr) {
  Guard g(mtx_);

  if (close_handler_sptr_) return;  // already done
  close_handler_sptr_ = handler_sptr;

  if (!status_.ok()) {
    CallCloseHandler();
    return;
  }

  if (writer_sptr_)
      writer_sptr_->Close();  // May trigger OnEndOfWriting().

  // XXX Just SetWritingClosed()... Delete IsWriting()
  //if (writer_sptr_ && writer_sptr_->IsWriting())
  //  return;
  //CloseNow();
}

// Finally close...
void ClientAsyncWriterImpl2::CloseNow() {
  if (!status_.ok()) {
    CallCloseHandler();
    return;
  }

  auto sptr = shared_from_this();
  using Tag = ClientAsyncWriterCloseCqTag;
  Tag* tag = new Tag(call_sptr_, [sptr, tag]() {
      sptr->OnClosed(*tag);
  });
  if (tag->Start())
      return;

  delete tag;
  status_.SetInternalError("Failed to close client stream.");
  // XXX *status_ok_sptr_ = false;  // Todo: extract SetInternalError()
  CallCloseHandler();
}  // Close()

void ClientAsyncWriterImpl2::CallCloseHandler() {
  if (!close_handler_sptr_)
    return;
  // XXX Not in CallCloseHandler()
  //if (writer_sptr_->IsWritingClosed())
  //  return;
  //writer_sptr_->SetWritingClosed();

  // XXX OnClose() in OnEndOfWriting()?
  close_handler_sptr_->OnClose(status_);
}

void ClientAsyncWriterImpl2::OnClosed(ClientAsyncWriterCloseCqTag& tag) {
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

  // XXX
  //if (!status_.ok())
  //  *status_ok_sptr_ = false;

  CallCloseHandler();
}

void ClientAsyncWriterImpl2::OnEndOfWriting() {
  Guard g(mtx_);

  if (!writer_sptr_) return;
  const Status& status = writer_sptr_->GetStatus();

  // XXX to close, call on_status() ...
  writer_sptr_.reset();  // Stop circular sharing.
}

}  // namespace grpc_cb
