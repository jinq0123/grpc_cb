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
  assert(reader_sptr_);
  assert(writer_sptr_);
  CloseWritingNow();  // XXX CloseWriting()?
}

bool ClientAsyncReaderWriterImpl2::Write(const MessageSptr& msg_sptr) {
  Guard g(mtx_);

  if (!status_.ok()) {
    assert(!reader_sptr_ && !writer_sptr_);
    return false;
  }

  if (!writer_sptr_) {
    // Impl2 and WriterHelper share each other untill OnEndOfWriting().
    auto sptr = shared_from_this();
    writer_sptr_.reset(new ClientAsyncWriterHelper(call_sptr_,
        [sptr]() { sptr->OnEndOfWriting(); }));
  }

  return writer_sptr_->Write(msg_sptr);
}

void ClientAsyncReaderWriterImpl2::CloseWriting() {
  Guard g(mtx_);
  // Set to send close when all messages are written.
  can_close_writing_ = true;  // XXX DEL just writer_sptr_->Close()

  // XXX writer->SetCanClose
}

// private
void ClientAsyncReaderWriterImpl2::CloseWritingNow() {
  if (!status_.ok()) return;
  assert(writer_sptr_);  // XXX assert(!writer_sptr)
  if (writer_sptr_->IsWritingClosed()) return;
  writer_sptr_->SetWritingClosed();

  // XXX Send close cq tag in OnEndOfWriting()?
  ClientSendCloseCqTag* tag = new ClientSendCloseCqTag(call_sptr_);
  if (tag->Start()) return;

  delete tag;
  SetInternalError("Failed to close writing.");
}

// Todo: same as ClientReader?

void ClientAsyncReaderWriterImpl2::ReadEach(
    const ReadHandlerSptr& handler_sptr) {
  Guard g(mtx_);

  read_handler_sptr_ = handler_sptr;
  if (reading_started_) return;
  reading_started_ = true;

  assert(!reader_sptr_);
  // Impl2 and Helper will share each other.
  auto sptr = shared_from_this();
  reader_sptr_.reset(new ClientAsyncReaderHelper(
      cq_sptr_, call_sptr_, read_handler_sptr_,
      [sptr]() { sptr->OnEndOfReading(); }));
  reader_sptr_->Start();
}

void ClientAsyncReaderWriterImpl2::OnEndOfReading() {
  Guard g(mtx_);

  if (!reader_sptr_) return;
  const Status& status = reader_sptr_->GetStatus();

  // XXX check status
  // XXXX recv status if writing is closed
  reader_sptr_.reset();  // Stop circular sharing.
}

// XXX Write Next in WriterHelper. Don't callback on Impl.
// DEL
//void ClientAsyncReaderWriterImpl2::OnWritten() {
//  Guard g(mtx_);
//
//  // Called from the write completion callback.
//  assert(writer_sptr_);
//  assert(writer_sptr_->IsWriting());
//  WriteNext();
//}

// DEL
// Send messages one by one, and finally close.
//void ClientAsyncReaderWriterImpl2::WriteNext() {
//  assert(writer_sptr_);
//  if (writer_sptr_->WriteNext())
//    return;  // XXX Get status...
//
//  if (can_close_writing_)
//    CloseWritingNow();
//}

void ClientAsyncReaderWriterImpl2::OnEndOfWriting() {
  Guard g(mtx_);

  if (!writer_sptr_) return;
  const Status& status = writer_sptr_->GetStatus();

  // XXX Check status and call on_status...
  assert(writer_sptr_->IsWritingClosed());
  writer_sptr_.reset();  // Stop circular sharing.

  // XXX CloseWritingNow()
}

void ClientAsyncReaderWriterImpl2::SetInternalError(const std::string& sError) {
  status_.SetInternalError(sError);
  if (reader_sptr_) {
    reader_sptr_->Abort();
    reader_sptr_.reset();
  }
  if (writer_sptr_) {
    writer_sptr_->Abort();
    writer_sptr_.reset();
  }
}

}  // namespace grpc_cb
