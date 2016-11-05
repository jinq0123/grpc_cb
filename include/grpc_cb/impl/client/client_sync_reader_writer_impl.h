// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_SYNC_READER_WRITER_IMPL_H
#define GRPC_CB_CLIENT_CLIENT_SYNC_READER_WRITER_IMPL_H

#include <string>

#include <grpc_cb/channel.h>                         // for MakeSharedCall()
#include <grpc_cb/impl/client/client_reader_data.h>  // for ClientReaderDataSptr
#include <grpc_cb/impl/client/client_send_close_cqtag.h>  // for ClientSendCloseCqTag
#include <grpc_cb/impl/client/client_sync_reader_helper.h>  // for ClientSyncReaderHelper
#include <grpc_cb/impl/client/client_sync_writer_helper.h>  // for ClientSyncWriterHelper
#include <grpc_cb/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/status.h>                      // for Status

namespace grpc_cb {

template <class Request, class Response>
class ClientSyncReaderWriterImpl GRPC_FINAL {
 public:
  inline ClientSyncReaderWriterImpl(const ChannelSptr& channel,
                                    const std::string& method);
  inline ~ClientSyncReaderWriterImpl();

 public:
  inline bool Write(const Request& request) const;
  // Writing() is optional which is called in dtr().
  inline void CloseWriting();

  inline bool ReadOne(Response* response) const;
  inline Status RecvStatus() const {
    const Data& d = *data_sptr_;
    if (!d.status.ok()) return d.status;
    return ClientSyncReaderHelper::BlockingRecvStatus(d.call_sptr, d.cq_sptr);
  }

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  using Data = ClientReaderData<Response>;
  using DataSptr = ClientReaderDataSptr<Response>;
  DataSptr data_sptr_;  // Same as reader. Easy to copy.
  bool writing_closed_ = false;  // Is BlockingCloseWriting() called?
};  // class ClientSyncReaderWriterImpl<>

// Todo: BlockingGetInitMd();

template <class Request, class Response>
ClientSyncReaderWriterImpl<Request, Response>::ClientSyncReaderWriterImpl(
    const ChannelSptr& channel, const std::string& method)
    // Todo: same as ClientReader?
    : data_sptr_(new Data) {
  assert(channel);
  CompletionQueueSptr cq_sptr(new CompletionQueue);
  CallSptr call_sptr = channel->MakeSharedCall(method, *cq_sptr);
  data_sptr_->cq_sptr = cq_sptr;
  data_sptr_->call_sptr = call_sptr;
  ClientInitMdCqTag tag(call_sptr);
  if (tag.Start()) {
    cq_sptr->Pluck(&tag);
    return;
  }
  data_sptr_->status.SetInternalError("Failed to start client sync reader writer.");
}

template <class Request, class Response>
ClientSyncReaderWriterImpl<Request, Response>::~ClientSyncReaderWriterImpl() {
  CloseWriting();
}

template <class Request, class Response>
bool ClientSyncReaderWriterImpl<Request, Response>::Write(const Request& request) const {
  assert(data_sptr_);
  assert(data_sptr_->call_sptr);
  return ClientSyncWriterHelper::BlockingWrite(data_sptr_->call_sptr,
      request, data_sptr_->status);
}

template <class Request, class Response>
void ClientSyncReaderWriterImpl<Request, Response>::CloseWriting() {
  if (writing_closed_) return;
  writing_closed_ = true;
  Status& status = data_sptr_->status;
  if (!status.ok()) return;

  ClientSendCloseCqTag tag(data_sptr_->call_sptr);
  if (tag.Start()) {
    data_sptr_->cq_sptr->Pluck(&tag);
    return;
  }

  status.SetInternalError("Failed to set stream writes done.");
}

// Todo: same as ClientReader?
template <class Request, class Response>
bool ClientSyncReaderWriterImpl<Request, Response>::ReadOne(Response* response) const {
  assert(response);
  Data& d = *data_sptr_;
  return ClientSyncReaderHelper::BlockingReadOne(
      d.call_sptr, d.cq_sptr, *response, d.status);
}

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_SYNC_READER_WRITER_IMPL_H
