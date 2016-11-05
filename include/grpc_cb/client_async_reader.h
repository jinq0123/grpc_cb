// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_READER_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_READER_H

#include <cassert>     // for assert()
#include <functional>  // for std::function

#include <grpc_cb/channel.h>  // for MakeSharedCall()
#include <grpc_cb/impl/client/client_async_reader_helper.h>  // for ClientAsyncReaderHelper
#include <grpc_cb/impl/client/client_reader_data.h>  // for ClientReaderDataSptr
#include <grpc_cb/impl/client/client_reader_init_cqtag.h>  // for ClientReaderInitCqTag
#include <grpc_cb/status.h>                                // for Status
#include <grpc_cb/status_callback.h>                       // for StatusCallback

namespace grpc_cb {

// Copyable.
template <class Response>
class ClientAsyncReader GRPC_FINAL {
 public:
  // Todo: Also need to template request?
  inline ClientAsyncReader(const ChannelSptr& channel, const std::string& method,
                      const ::google::protobuf::Message& request,
                      const CompletionQueueSptr& cq_sptr);

 public:
  using MsgCallback = std::function<void(const Response&)>;
  inline void ReadEach(
      const MsgCallback& on_msg,
      const StatusCallback& on_status = StatusCallback()) const {
    data_sptr_->on_msg = on_msg;
    data_sptr_->on_status = on_status;
    ClientAsyncReaderHelper::AsyncReadNext(data_sptr_);
  }

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  using Data = ClientReaderData<Response>;
  using DataSptr = ClientReaderDataSptr<Response>;
  DataSptr data_sptr_;
};  // class ClientAsyncReader<>

// XXX Delete ClientAsyncReader. Only need DataSptr.

template <class Response>
ClientAsyncReader<Response>::ClientAsyncReader(const ChannelSptr& channel,
                                     const std::string& method,
                                     const ::google::protobuf::Message& request,
                                     const CompletionQueueSptr& cq_sptr)
    : data_sptr_(new Data{cq_sptr, channel->MakeSharedCall(method, *cq_sptr)}) {
  assert(cq_sptr);
  assert(channel);
  assert(data_sptr_->call_sptr);
  ClientReaderInitCqTag* tag = new ClientReaderInitCqTag(data_sptr_->call_sptr);
  if (tag->Start(request)) return;
  delete tag;
  data_sptr_->status.SetInternalError("Failed to start client reader stream.");
}  // ClientAsyncReader()

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_READER_H
