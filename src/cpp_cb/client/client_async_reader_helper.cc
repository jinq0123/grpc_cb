// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_reader_helper.h"

#include <cassert>  // for assert()

#include <grpc_cb/impl/client/client_async_read_handler.h>  // for ClientAsyncReadHandler
#include <grpc_cb/impl/client/client_reader_async_recv_status_cqtag.h>  // for ClientReaderAsyncRecvStatusCqTag
#include <grpc_cb/status.h>  // for Status

#include "client_reader_async_read_cqtag.h"  // for ClientReaderAsyncReadCqTag

namespace grpc_cb {

using Sptr = ClientAsyncReaderHelperSptr;

ClientAsyncReaderHelper::ClientAsyncReaderHelper(
    CompletionQueueSptr cq_sptr, CallSptr call_sptr, const StatusSptr& status_sptr,
    const ClientAsyncReadHandlerSptr& read_handler_sptr,
    const StatusCallback& on_status)
    : cq_sptr_(cq_sptr),
      call_sptr_(call_sptr),
      status_sptr_(status_sptr),
      read_handler_sptr_(read_handler_sptr),
      on_status_(on_status) {
  assert(cq_sptr);
  assert(call_sptr);
  assert(status_sptr);
}

ClientAsyncReaderHelper::~ClientAsyncReaderHelper() {}

// Setup next async read.
void ClientAsyncReaderHelper::AsyncReadNext() {
  if (!status_sptr_->ok()) return;

  Sptr sptr = shared_from_this();
  auto* tag = new ClientReaderAsyncReadCqTag(sptr);
  if (tag->Start()) return;

  delete tag;
  status_sptr_->SetInternalError("Failed to async read server stream.");
  if (on_status_) on_status_(*status_sptr_);
}

void ClientAsyncReaderHelper::OnRead(ClientReaderAsyncReadCqTag& tag) {
  if (!status_sptr_->ok())
    return;
  if (!tag.HasGotMsg()) {
    // End of read.
    AsyncRecvStatus();
    // XXXX Do not recv status in Reader. Do it after all reading and writing.
    return;
  }

  Status& status = *status_sptr_;
  status = tag.GetResultMsg(read_handler_sptr_->GetMsg());
  if (status.ok()) {
    read_handler_sptr_->HandleMsg();
    return;
  }

  // XXX CallOnEnd(status);
}

void ClientAsyncReaderHelper::AsyncRecvStatus() {
  assert(status_sptr_->ok());

  // XXX input status_sptr_ to CqTag? To abort writing?
  auto* tag = new ClientReaderAsyncRecvStatusCqTag(call_sptr_, on_status_);
  if (tag->Start()) return;

  delete tag;
  status_sptr_->SetInternalError("Failed to receive status.");
  if (on_status_) on_status_(*status_sptr_);
}

#if 0
template <class Response>
inline void OnReadEach(const Response& msg,
    const ClientReaderDataSptr<Response>& data_sptr) {
  Status& status = data_sptr->status;
  assert(status.ok());

  std::function<void(const Response&)>& on_msg = data_sptr->on_msg;
  if (on_msg) on_msg(msg);

  AsyncReadNext(data_sptr);
  // Old tag will be deleted after return in BlockingRun().
}

template <class Response>
inline void OnEnd(const Status& status,
    const ClientReaderDataSptr<Response>& data_sptr) {
  StatusCallback& on_status = data_sptr->on_status;
  if (status.ok()) {
    AsyncRecvStatus(data_sptr->call_sptr,
        data_sptr->status, on_status);
    return;
  }

  if (on_status) on_status(status);
}
#endif

}  // namespace grpc_cb
