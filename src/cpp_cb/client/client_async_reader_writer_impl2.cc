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
      status_ok_sptr_(new std::atomic_bool{ true }),
      on_status_(on_status) {
  assert(cq_sptr);
  assert(call_sptr_);

  ClientInitMdCqTag* tag = new ClientInitMdCqTag(call_sptr_);
  if (tag->Start()) return;
  delete tag;
  status_.SetInternalError("Failed to init stream.");
  *status_ok_sptr_ = false;
}

ClientAsyncReaderWriterImpl2::~ClientAsyncReaderWriterImpl2() {
  CloseWritingNow();
}

bool ClientAsyncReaderWriterImpl2::Write(const MessageSptr& msg_sptr) {
  Guard g(mtx_);

  if (!status_.ok())
    return false;

  assert(*status_ok_sptr_);
  if (!writer_sptr_) {
    // Impl2 and WriterHelper share each other untill OnEndOfWriting().
    auto sptr = shared_from_this();
    writer_sptr_.reset(new ClientAsyncWriterHelper(
        call_sptr_, status_ok_sptr_, [sptr]() { sptr->OnEndOfWriting(); }));
  }

  writer_sptr_->Write(msg_sptr);
  return true;
}

void ClientAsyncReaderWriterImpl2::CloseWriting() {
  Guard g(mtx_);
  // Set to send close when all messages are written.
  can_close_writing_ = true;
}

// private
void ClientAsyncReaderWriterImpl2::CloseWritingNow() {
  if (!status_.ok()) return;
  assert(*status_ok_sptr_);
  if (writer_sptr_->IsWritingClosed()) return;
  writer_sptr_->SetWritingClosed();

  ClientSendCloseCqTag* tag = new ClientSendCloseCqTag(call_sptr_);
  if (tag->Start()) return;

  delete tag;
  status_.SetInternalError("Failed to close writing.");
  *status_ok_sptr_ = false;
  // XXX extract SetInternalError()
}

// Todo: same as ClientReader?

void ClientAsyncReaderWriterImpl2::ReadEach(
    const ReadHandlerSptr& handler_sptr) {
  Guard g(mtx_);

  read_handler_sptr_ = handler_sptr;
  if (reading_started_) return;
  reading_started_ = true;

  assert(!reader_sptr_);
  // Impl and Helper will share each other.
  auto sptr = shared_from_this();
  reader_sptr_.reset(new ClientAsyncReaderHelper(
      cq_sptr_, call_sptr_, status_ok_sptr_, read_handler_sptr_,
      [sptr]() { sptr->OnEndOfReading(); }));
}

void ClientAsyncReaderWriterImpl2::OnEndOfReading() {
  Guard g(mtx_);
  // XXXX recv status if writing is closed
  reader_sptr_.reset();  // Stop circular sharing.
}

// XXX Write Next in WriterHelper. Don't callback on Impl.

void ClientAsyncReaderWriterImpl2::OnWritten() {
  Guard g(mtx_);

  // Called from the write completion callback.
  assert(writer_sptr_);
  assert(writer_sptr_->IsWriting());
  WriteNext();
}

// Send messages one by one, and finally close.
void ClientAsyncReaderWriterImpl2::WriteNext() {
  assert(writer_sptr_);
  if (writer_sptr_->WriteNext())
    return;  // XXX Get status...

  if (can_close_writing_)
    CloseWritingNow();
}

void ClientAsyncReaderWriterImpl2::OnEndOfWriting() {
  // XXX Check status and call on_status...
  assert(writer_sptr_->IsWritingClosed());
  writer_sptr_.reset();  // Stop circular sharing.
}

}  // namespace grpc_cb
