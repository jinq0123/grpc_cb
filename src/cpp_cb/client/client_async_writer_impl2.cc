// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_writer_impl2.h"

#include <cassert>  // for assert()

#include <grpc_cb/channel.h>  // for MakeSharedCall()
#include <grpc_cb/impl/client/client_async_writer_close_handler.h>  // for OnClose()
#include <grpc_cb/impl/client/client_send_init_md_cqtag.h>  // for ClientSendInitMdCqTag

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
  ClientSendInitMdCqTag* tag = new ClientSendInitMdCqTag(call_sptr_);
  if (tag->Start()) return;
  delete tag;
  SetInternalError("Failed to init client stream.");
  // Call close handler when Close(CloseHandler)
}

ClientAsyncWriterImpl2::~ClientAsyncWriterImpl2() {
  // Have done CallCloseHandler().
}

bool ClientAsyncWriterImpl2::Write(const MessageSptr& request_sptr) {
  Guard g(mtx_);
  if (!status_.ok())
    return false;
  if (writer_sptr_)
    return writer_sptr_->Queue(request_sptr);

  if (writing_started_)
    return false;  // Writer ended
  writing_started_ = true;

  // Impl2 and WriterHelper shared each other untill OnEnd().
  auto sptr = shared_from_this();
  writer_sptr_.reset(new ClientAsyncWriterHelper(call_sptr_,
      [sptr]() { sptr->OnEndOfWriting(); }));
  return writer_sptr_->Queue(request_sptr);
}

void ClientAsyncWriterImpl2::Close(const CloseHandlerSptr& handler_sptr) {
  Guard g(mtx_);

  if (close_handler_set_) return;  // already done
  close_handler_set_ = true;
  close_handler_sptr_ = handler_sptr;  // reset after CallCloseHandler()
  writing_started_ = true;  // Maybe without any Write().

  if (!status_.ok()) {
    CallCloseHandler();
    return;
  }

  if (writer_sptr_)
      writer_sptr_->QueueEnd();  // May trigger OnEndOfWriting().
  else
      SendCloseIfNot();
}

// Finally close...
void ClientAsyncWriterImpl2::SendCloseIfNot() {
  assert(!writer_sptr_);  // Must be ended.
  if (!status_.ok())
    return;

  if (has_sent_close_) return;
  has_sent_close_ = true;
  auto sptr = shared_from_this();
  auto* tag = new ClientAsyncWriterCloseCqTag(call_sptr_);
  tag->SetOnClosed([sptr, tag]() {
    sptr->OnClosed(*tag);
  });
  if (tag->Start())
    return;

  delete tag;
  SetInternalError("Failed to close client stream.");  // Calls CallCloseHandler();
}  // Close()

void ClientAsyncWriterImpl2::CallCloseHandler() {
  if (!close_handler_sptr_) return;
  close_handler_sptr_->OnClose(status_);
  close_handler_sptr_.reset();
}

// Callback of ClientAsyncWriterCloseCqTag
void ClientAsyncWriterImpl2::OnClosed(ClientAsyncWriterCloseCqTag& tag) {
  Guard g(mtx_);

  // Todo: Get trailing metadata.
  if (tag.IsStatusOk()) {
    if (close_handler_sptr_) {
      status_ = tag.GetResponse(close_handler_sptr_->GetMsg());
    } else {
      status_.SetInternalError("Response is ignored.");
    }
  } else {
    status_ = tag.GetStatus();
  }

  CallCloseHandler();
}

void ClientAsyncWriterImpl2::OnEndOfWriting() {
  Guard g(mtx_);

  if (!writer_sptr_) return;
  Status w_status(writer_sptr_->GetStatus());  // copy before reset()
  writer_sptr_.reset();  // Stop circular sharing.

  if (!status_.ok()) return;
  status_ = w_status;
  if (status_.ok())
    SendCloseIfNot();
  else
    CallCloseHandler();
}

void ClientAsyncWriterImpl2::SetInternalError(const std::string& sError) {
  status_.SetInternalError(sError);
  CallCloseHandler();
  if (writer_sptr_) {
    writer_sptr_->Abort();
    writer_sptr_.reset();
  }
}

}  // namespace grpc_cb
