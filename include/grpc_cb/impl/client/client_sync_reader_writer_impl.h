// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_SYNC_READER_WRITER_IMPL_H
#define GRPC_CB_CLIENT_CLIENT_SYNC_READER_WRITER_IMPL_H

#include <cstdint>  // for int64_t
#include <string>

#include <grpc_cb/channel.h>                         // for MakeSharedCall()
#include <grpc_cb/impl/client/client_sync_reader_data.h>  // for ClientSyncReaderDataSptr
#include <grpc_cb/impl/client/client_recv_init_md_cqtag.h>  // for ClientRecvInitMdCqTag
#include <grpc_cb/impl/client/client_send_close_cqtag.h>    // for ClientSendCloseCqTag
#include <grpc_cb/impl/client/client_sync_reader_helper.h>  // for ClientSyncReaderHelper
#include <grpc_cb/impl/client/client_sync_writer_helper.h>  // for ClientSyncWriterHelper
#include <grpc_cb/impl/cqueue_for_pluck.h>  // for CQueueForPluck
#include <grpc_cb/status.h>                      // for Status

namespace grpc_cb {

template <class Request, class Response>
class ClientSyncReaderWriterImpl GRPC_FINAL {
 public:
  inline ClientSyncReaderWriterImpl(const ChannelSptr& channel,
                                    const std::string& method,
                                    int64_t timeout_ms);
  inline ~ClientSyncReaderWriterImpl();

 public:
  inline bool Write(const Request& request) const;
  // Writing() is optional which is called in dtr().
  inline void CloseWriting();

  inline bool ReadOne(Response* response);
  inline Status RecvStatus() const {
    const Data& d = *data_sptr_;
    if (!d.status.ok()) return d.status;
    return ClientSyncReaderHelper::BlockingRecvStatus(d.call_sptr, d.cq4p_sptr);
  }

 private:
  void RecvInitMdIfNot();

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  using Data = ClientSyncReaderData<Response>;
  using DataSptr = ClientSyncReaderDataSptr<Response>;
  DataSptr data_sptr_;  // Same as reader. Easy to copy.
  bool writing_closed_ = false;  // Is BlockingCloseWriting() called?
  bool init_md_received_ = false;  // to receive init metadata once
};  // class ClientSyncReaderWriterImpl<>

// Todo: BlockingGetInitMd();

template <class Request, class Response>
ClientSyncReaderWriterImpl<Request, Response>::ClientSyncReaderWriterImpl(
    const ChannelSptr& channel, const std::string& method, int64_t timeout_ms)
    // Todo: same as ClientReader?
    : data_sptr_(new Data) {
  assert(channel);
  CQueueForPluckSptr cq4p_sptr(new CQueueForPluck);
  CallSptr call_sptr = channel->MakeSharedCall(method, *cq4p_sptr, timeout_ms);
  data_sptr_->cq4p_sptr = cq4p_sptr;
  data_sptr_->call_sptr = call_sptr;
  ClientSendInitMdCqTag tag(call_sptr);
  if (tag.Start()) {
    cq4p_sptr->Pluck(&tag);
    return;
  }
  data_sptr_->status.SetInternalError("Failed to start client sync reader writer.");
}

template <class Request, class Response>
ClientSyncReaderWriterImpl<Request, Response>::~ClientSyncReaderWriterImpl() {
  CloseWriting();
}

template <class Request, class Response>
bool ClientSyncReaderWriterImpl<Request, Response>::Write(
    const Request& request) const {
  assert(data_sptr_);
  Data& d = *data_sptr_;
  assert(d.call_sptr);
  return ClientSyncWriterHelper::BlockingWrite(d.call_sptr, d.cq4p_sptr, request,
                                               d.status);
}

template <class Request, class Response>
void ClientSyncReaderWriterImpl<Request, Response>::CloseWriting() {
  if (writing_closed_) return;
  writing_closed_ = true;
  Status& status = data_sptr_->status;
  if (!status.ok()) return;

  ClientSendCloseCqTag tag(data_sptr_->call_sptr);
  if (tag.Start()) {
    data_sptr_->cq4p_sptr->Pluck(&tag);
    return;
  }

  status.SetInternalError("Failed to close writing.");
}

// Todo: same as ClientReader?
template <class Request, class Response>
bool ClientSyncReaderWriterImpl<Request, Response>::ReadOne(Response* response) {
  assert(response);
  Data& d = *data_sptr_;
  Status& status = d.status;
  if (!status.ok()) return false;
  RecvInitMdIfNot();
  if (!status.ok()) return false;
  return ClientSyncReaderHelper::BlockingReadOne(
      d.call_sptr, d.cq4p_sptr, *response, status);
}

template <class Request, class Response>
void ClientSyncReaderWriterImpl<Request, Response>::RecvInitMdIfNot() {
  if (init_md_received_) return;
  init_md_received_ = true;
  Status& status = data_sptr_->status;
  assert(status.ok());

  ClientRecvInitMdCqTag tag(data_sptr_->call_sptr);
  if (tag.Start()) {
    data_sptr_->cq4p_sptr->Pluck(&tag);
    return;
  }
  status.SetInternalError("Failed to receive init metadata.");
}

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_SYNC_READER_WRITER_IMPL_H
