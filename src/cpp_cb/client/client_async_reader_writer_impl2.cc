// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_reader_writer_impl2.h"

#include <grpc_cb/channel.h>                           // for MakeSharedCall()
#include <grpc_cb/impl/client/client_init_md_cqtag.h>  // ClientInitMdCqTag
#include <grpc_cb/impl/client/client_send_close_cqtag.h>  // for ClientSendCloseCqTag

#include "client_async_reader_helper.h"  // for ClientAsyncReaderHelper
#include "client_async_writer_helper.h"  // for ClientAsyncWriterHelper

namespace grpc_cb {

using Sptr = std::shared_ptr<ClientAsyncReaderWriterImpl2>;

// Todo: BlockingGetInitMd();

ClientAsyncReaderWriterImpl2::ClientAsyncReaderWriterImpl2(
    const ChannelSptr& channel, const std::string& method,
    const CompletionQueueSptr& cq_sptr, const StatusCallback& on_status)
    : cq_sptr_(cq_sptr),
      call_sptr_(channel->MakeSharedCall(method, *cq_sptr)),
      on_status_(on_status) {
  assert(cq_sptr);
  assert(call_sptr_);

  ClientInitMdCqTag* tag = new ClientInitMdCqTag(call_sptr_);
  if (tag->Start()) return;
  delete tag;
  SetInternalError("Failed to init stream.");
}

ClientAsyncReaderWriterImpl2::~ClientAsyncReaderWriterImpl2() {
  // Reader and Writer helpers share this.
  assert(!reader_sptr_);
  assert(!writer_sptr_);
  SendCloseIfNot();
}

bool ClientAsyncReaderWriterImpl2::Write(const MessageSptr& msg_sptr) {
  Guard g(mtx_);

  if (!status_.ok()) {
    assert(!reader_sptr_ && !writer_sptr_);
    return false;
  }

  if (writer_sptr_)
    return writer_sptr_->Queue(msg_sptr);
  if (writing_started_)  // but writer_sptr_ is reset
    return false;

  // new writer only once
  writing_started_ = true;
  // Impl2 and WriterHelper share each other untill OnEndOfWriting().
  auto sptr = shared_from_this();  // can not in ctr().
  writer_sptr_.reset(new ClientAsyncWriterHelper(call_sptr_,
      [sptr]() { sptr->OnEndOfWriting(); }));
  return writer_sptr_->Queue(msg_sptr);
}

void ClientAsyncReaderWriterImpl2::CloseWriting() {
  Guard g(mtx_);
  writing_started_ = true;  // Maybe without any Write().

  // End when all messages are written.
  if (writer_sptr_)
      writer_sptr_->QueueEnd();
}

// Called in dtr().
// Send close only if reading and writing are both ended.
void ClientAsyncReaderWriterImpl2::SendCloseIfNot() {
  assert(!reader_sptr_);  // Must be ended.
  assert(!writer_sptr_);  // Must be ended.
  if (!status_.ok()) return;
  if (has_sent_close_) return;
  has_sent_close_ = true;

  ClientSendCloseCqTag* tag = new ClientSendCloseCqTag(call_sptr_);
  if (tag->Start()) {
    CallOnStatus();
    return;
  }

  delete tag;
  SetInternalError("Failed to close writing.");  // calls on_status_
}

// Todo: same as ClientReader?

void ClientAsyncReaderWriterImpl2::ReadEach(
    const ReadHandlerSptr& handler_sptr) {
  Guard g(mtx_);

  read_handler_sptr_ = handler_sptr;
  if (reading_started_) return;
  reading_started_ = true;  // new reader_sptr_ once

  assert(!reader_sptr_);
  // Impl2 and ReaderHelper will share each other until OnEndOfReading().
  auto sptr = shared_from_this();
  reader_sptr_.reset(new ClientAsyncReaderHelper(
      cq_sptr_, call_sptr_, read_handler_sptr_,
      [sptr]() { sptr->OnEndOfReading(); }));
  reader_sptr_->Start();
}

void ClientAsyncReaderWriterImpl2::OnEndOfReading() {
  Guard g(mtx_);
  if (!reader_sptr_) return;
  Status r_status(reader_sptr_->GetStatus());  // before reset()
  reader_sptr_.reset();  // Stop circular sharing.

  if (!status_.ok()) return;
  status_ = r_status;
  if (!status_.ok()) {
    CallOnStatus();
    return;
  }

  assert(IsReadingEnded());
  if (IsWritingEnded())
    SendCloseIfNot();
}

void ClientAsyncReaderWriterImpl2::OnEndOfWriting() {
  Guard g(mtx_);
  if (!writer_sptr_) return;
  Status w_status(writer_sptr_->GetStatus());  // before reset()
  writer_sptr_.reset();  // Stop circular sharing.

  if (!status_.ok()) return;
  status_ = w_status;
  if (!status_.ok()) {
    CallOnStatus();
    return;
  }

  assert(IsWritingEnded());
  if (IsReadingEnded())
    SendCloseIfNot();
}

// Set status and callback and reset helpers.
void ClientAsyncReaderWriterImpl2::SetInternalError(const std::string& sError) {
  status_.SetInternalError(sError);
  CallOnStatus();

  if (reader_sptr_) {
    reader_sptr_->Abort();
    reader_sptr_.reset();
  }
  if (writer_sptr_) {
    writer_sptr_->Abort();
    writer_sptr_.reset();
  }
}

bool ClientAsyncReaderWriterImpl2::IsReadingEnded() const {
  return reading_started_ && !reader_sptr_;
}

bool ClientAsyncReaderWriterImpl2::IsWritingEnded() const {
  return writing_started_ && !writer_sptr_;
}

void ClientAsyncReaderWriterImpl2::CallOnStatus() {
  if (!on_status_) return;
  on_status_(status_);
  on_status_ = StatusCallback();
}

}  // namespace grpc_cb
