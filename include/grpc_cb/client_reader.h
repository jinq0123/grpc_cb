// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_H
#define GRPC_CB_CLIENT_CLIENT_READER_H

#include <cassert>     // for assert()
#include <functional>  // for std::function

#include <grpc_cb/channel.h>                           // for MakeSharedCall()
#include <grpc_cb/impl/client/client_reader_helper.h>  // for ClientReaderHelper
#include <grpc_cb/impl/client/client_reader_init_cqtag.h>  // for ClientReaderInitCqTag
#include <grpc_cb/status.h>                                // for Status
#include <grpc_cb/status_callback.h>                       // for StatusCallback

namespace grpc_cb {

// Copyable.
template <class Response>
class ClientReader GRPC_FINAL {
 public:
  // Todo: Also need to template request?
  inline ClientReader(const ChannelSptr& channel, const std::string& method,
               const ::google::protobuf::Message& request,
               const CompletionQueueSptr& cq_sptr);

 public:
  inline bool BlockingReadOne(Response* response) const {
    assert(response);
    Data& d = *data_sptr_;
    return ClientReaderHelper::BlockingReadOne(
        d.call_sptr, d.cq_sptr, *response, d.status);
  }

  inline Status BlockingRecvStatus() const {
    const Data& d = *data_sptr_;
    return ClientReaderHelper::BlockingRecvStatus(d.call_sptr, d.cq_sptr);
  }

  using MsgCallback = std::function<void(const Response&)>;
  inline void AsyncReadEach(
      const MsgCallback& on_msg,
      const StatusCallback& on_status = StatusCallback()) const {
    data_sptr_->on_msg = on_msg;
    data_sptr_->on_status = on_status;
    ClientReaderHelper::AsyncReadNext(data_sptr_);
  }

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  using Data = ClientReaderData<Response>;
  using DataSptr = ClientReaderDataSptr<Response>;
  DataSptr data_sptr_;
};  // class ClientReader<>

template <class Response>
ClientReader<Response>::ClientReader(
    const ChannelSptr& channel, const std::string& method,
    const ::google::protobuf::Message& request,
    const CompletionQueueSptr& cq_sptr)
    : data_sptr_(new Data{cq_sptr, channel->MakeSharedCall(method, *cq_sptr)}) {
  assert(cq_sptr);
  assert(channel);
  assert(data_sptr_->call_sptr);
  ClientReaderInitCqTag* tag = new ClientReaderInitCqTag(data_sptr_->call_sptr);
  if (tag->Start(request)) return;
  delete tag;
  data_sptr_->status.SetInternalError(
      "Failed to start client reader stream.");
}  // ClientReader()

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_H
