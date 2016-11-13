// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/client/client_async_reader_writer_impl.h>  // for ClientAsyncReaderWriterImpl<>

#include <grpc_cb/channel.h>  // for MakeSharedCall()
#include <grpc_cb/impl/client/client_async_reader_helper.h>  // for ClientAsyncReaderHelper
#include <grpc_cb/impl/client/client_async_writer_helper.h>  // for ClientAsyncWriterHelper
#include <grpc_cb/impl/client/client_init_md_cqtag.h>  // ClientInitMdCqTag
#include <grpc_cb/impl/client/client_send_close_cqtag.h>  // for ClientSendCloseCqTag
#include <grpc_cb/impl/completion_queue_sptr.h>  // for CompletionQueueSptr

namespace grpc_cb {

// Todo: BlockingGetInitMd();

ClientAsyncReaderWriterImpl::ClientAsyncReaderWriterImpl(
    const ChannelSptr& channel, const std::string& method,
    const CompletionQueueSptr& cq_sptr)
    : cq_sptr_(cq_sptr), 
    call_sptr_(channel->MakeSharedCall(method, *cq_sptr)) {
  assert(cq_sptr);
  ClientInitMdCqTag* tag = new ClientInitMdCqTag(call_sptr_);
  if (tag->Start()) return;
  delete tag;
  status_.SetInternalError("Failed to init stream.");
}

ClientAsyncReaderWriterImpl::~ClientAsyncReaderWriterImpl() {
  CloseWriting();  // XXX
}

bool ClientAsyncReaderWriterImpl::Write(const MessageSptr& msg_sptr) {
  assert(call_sptr_);
  if (!status_.ok()) return false;

  // XXX cache messages
  msg_queue_.push(msg_sptr);
  if (is_writing_) return true;
  InternalNext();
  return true;

  // XXX
  //return ClientAsyncWriterHelper::AsyncWrite(call_sptr_,
  //    *msg_sptr, status_);
}

void ClientAsyncReaderWriterImpl::CloseWriting() {
  if (writing_closed_) return;
  writing_closed_ = true;
  if (!status_.ok()) return;
  ClientSendCloseCqTag* tag = new ClientSendCloseCqTag(call_sptr_);
  if (tag->Start()) return;

  delete tag;
  status_.SetInternalError("Failed to set stream writes done.");
}

// Todo: same as ClientReader?

void ClientAsyncReaderWriterImpl::SetReadHandler(
    const ReadHandlerSptr& handler_sptr) {
  read_handler_sptr_ = handler_sptr;
  if (is_reading_) return;
  is_reading_ = true;

  auto sptr = shared_from_this();
  // XXX ClientAsyncReaderHelper::AsyncReadNext(on_read, on_end)
  // XXX ClientAsyncReaderHelper::AsyncReadNext(data_sptr_);  // XXX
}

void ClientAsyncReaderWriterImpl::Next() {
  Guard g(mtx_);
  assert(is_writing_);  // Because Next() is called from completion callback.
  InternalNext();
}

// Send messages one by one, and finally close.
void ClientAsyncReaderWriterImpl::InternalNext() {
  if (!status_.ok() || msg_queue_.empty())
  {
    is_writing_ = false;
    // Do not close before Close(handler).
    // XXX if (close_handler_sptr_)
    // XXX   CloseNow();
    return;
  }

  is_writing_ = true;
  // XXX write one message...
}

}  // namespace grpc_cb
