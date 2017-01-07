// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/client/client_async_reader_impl.h>

#include <cassert>     // for assert()

#include <grpc_cb/channel.h>  // for MakeSharedCall()
#include <grpc_cb/impl/client/client_reader_init_cqtag.h>  // for ClientReaderInitCqTag

#include "client_async_reader_helper.h"
#include "client_async_reader_recv_status.h"

namespace grpc_cb {

ClientAsyncReaderImpl::ClientAsyncReaderImpl(
    const ChannelSptr& channel, const std::string& method,
    const ::google::protobuf::Message& request,
    const CompletionQueueSptr& cq_sptr)
    : cq_sptr_(cq_sptr),
      call_sptr_(channel->MakeSharedCall(method, *cq_sptr)) {
  assert(cq_sptr);
  assert(channel);
  assert(call_sptr_);

  // Todo: Move to Start()
  // Todo: move ClientReaderInitCqTag to src/
  ClientReaderInitCqTag* tag = new ClientReaderInitCqTag(call_sptr_);
  if (tag->Start(request))
    return;

  delete tag;
  status_.SetInternalError("Failed to start async client reader.");
}

ClientAsyncReaderImpl::~ClientAsyncReaderImpl() {}

void ClientAsyncReaderImpl::SetReadHandler(
    const ClientAsyncReadHandlerSptr& handler) {
  Guard g(mtx_);
  read_handler_sptr_ = handler;
}

void ClientAsyncReaderImpl::SetOnStatus(const StatusCallback& on_status) {
  Guard g(mtx_);
  on_status_ = on_status;
}

void ClientAsyncReaderImpl::Start() {
  Guard g(mtx_);
  if (reading_started_) return;
  reading_started_ = true;
  assert(!reader_sptr_);

  // Impl and Helper will share each other until the end of reading.
  auto sptr = shared_from_this();
  reader_sptr_.reset(new ClientAsyncReaderHelper(
      cq_sptr_, call_sptr_, read_handler_sptr_,
      [sptr]() { sptr->OnEndOfReading(); }));
  reader_sptr_->Start();
}

void ClientAsyncReaderImpl::OnEndOfReading() {
  Guard g(mtx_);
  assert(reading_started_);

  if (!reader_sptr_) return;
  const Status status(reader_sptr_->GetStatus());  // Copy
  reader_sptr_.reset();  // Stop circular sharing.

  if (status.ok()) {
    ClientAsyncReader::RecvStatus(call_sptr_, on_status_);
    return;
  }

  if (on_status_)
    on_status_(status);
}

}  // namespace grpc_cb
