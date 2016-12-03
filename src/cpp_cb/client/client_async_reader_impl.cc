// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/client/client_async_reader_impl.h>

#include <cassert>     // for assert()

#include <grpc_cb/channel.h>  // for MakeSharedCall()
#include <grpc_cb/impl/client/client_reader_init_cqtag.h>  // for ClientReaderInitCqTag

#include "client_async_reader_helper.h"

namespace grpc_cb {

ClientAsyncReaderImpl::ClientAsyncReaderImpl(
    const ChannelSptr& channel, const std::string& method,
    const ::google::protobuf::Message& request,
    const CompletionQueueSptr& cq_sptr)
    : cq_sptr_(cq_sptr),
      call_sptr_(channel->MakeSharedCall(method, *cq_sptr)),
      status_ok_sptr_(new std::atomic_bool{ true }) {
  assert(cq_sptr);
  assert(channel);
  assert(call_sptr_);

  // Todo: move ClientReaderInitCqTag to src/
  ClientReaderInitCqTag* tag = new ClientReaderInitCqTag(call_sptr_);
  if (tag->Start(request))
    return;

  delete tag;
  status_.SetInternalError("Failed to start async client reader.");
  *status_ok_sptr_ = false;
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
      cq_sptr_, call_sptr_, status_ok_sptr_, read_handler_sptr_,
      [sptr]() { sptr->OnEndOfReading(); }));
  reader_sptr_->Start();
}

void ClientAsyncReaderImpl::OnEndOfReading() {
  // XXX recv status...
  reader_sptr_.reset();  // Stop circular sharing.
}

}  // namespace grpc_cb
