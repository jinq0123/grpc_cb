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
    : call_sptr_(channel->MakeSharedCall(method, *cq_sptr)) {
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
  CallOnStatus();
}

ClientAsyncReaderImpl::~ClientAsyncReaderImpl() {}

void ClientAsyncReaderImpl::SetReadHandler(
    const ClientAsyncReadHandlerSptr& handler) {
  Guard g(mtx_);
  read_handler_sptr_ = handler;
}

void ClientAsyncReaderImpl::SetOnStatus(const StatusCallback& on_status) {
  Guard g(mtx_);
  if (set_on_status_once_) return;
  set_on_status_once_ = true;
  on_status_ = on_status;
  if (!status_.ok())
    CallOnStatus();
}

void ClientAsyncReaderImpl::Start() {
  Guard g(mtx_);
  if (reading_started_) return;
  reading_started_ = true;
  assert(!reader_sptr_);

  // Impl and Helper will share each other until the end of reading.
  auto sptr = shared_from_this();
  reader_sptr_.reset(new ClientAsyncReaderHelper(
      call_sptr_, read_handler_sptr_, [sptr]() { sptr->OnEndOfReading(); }));
  reader_sptr_->Start();
}

void ClientAsyncReaderImpl::OnEndOfReading() {
  Guard g(mtx_);
  assert(reading_started_);

  if (!reader_sptr_) return;
  Status r_status(reader_sptr_->GetStatus());  // Copy before reset()
  reader_sptr_.reset();  // Stop circular sharing.

  if (!status_.ok()) return;
  status_ = r_status;
  if (status_.ok()) {
    ClientAsyncReader::RecvStatus(call_sptr_, on_status_);
    return;
  }

  CallOnStatus();
}

void ClientAsyncReaderImpl::CallOnStatus() {
  if (!on_status_) return;
  on_status_(status_);
  on_status_ = StatusCallback();
}

}  // namespace grpc_cb
