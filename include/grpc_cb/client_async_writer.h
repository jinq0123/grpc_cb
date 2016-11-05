// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_WRITER_H
#define GRPC_CB_CLIENT_ASYNC_WRITER_H

#include <cassert>     // for assert()

#include <grpc_cb/channel.h>         // for MakeSharedCall()
#include <grpc_cb/impl/call_sptr.h>  // for CallSptr
#include <grpc_cb/impl/client/client_async_writer_helper.h>  // for ClientAsyncWriterHelper
#include <grpc_cb/impl/client/client_init_md_cqtag.h>  // for ClientInitMdCqTag
#include <grpc_cb/impl/client/client_writer_finish_cqtag.h>  // for ClientWriterFinishCqTag
#include <grpc_cb/status.h>                                  // for Status

namespace grpc_cb {

// Copyable.
// Use template class instead of template member function
//    to ensure client input the correct request type.
// Todo: Use non_template class as the implement.
template <class Request>
class ClientAsyncWriter GRPC_FINAL {
 public:
  inline ClientAsyncWriter(const ChannelSptr& channel, const std::string& method,
                      const CompletionQueueSptr& cq_sptr);

  // Todo: BlockingGetInitMd();
  bool Write(const Request& request) const {
    Data& d = *data_sptr_;
    return ClientAsyncWriterHelper::AsyncWrite(d.call_sptr, request, d.status);
  }

  Status Finish(
      ::google::protobuf::Message* response) const;

  // Todo: AsyncFinish

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  struct Data {
    CompletionQueueSptr cq_sptr;
    CallSptr call_sptr;
    Status status;
  };
  std::shared_ptr<Data> data_sptr_;  // Easy to copy.
};  // class ClientAsyncWriter<>

template <class Request>
ClientAsyncWriter<Request>::ClientAsyncWriter(const ChannelSptr& channel,
                                    const std::string& method,
                                    const CompletionQueueSptr& cq_sptr)
    // Todo: same as ClientReader?
    : data_sptr_(new Data{cq_sptr, channel->MakeSharedCall(method, *cq_sptr)}) {
  assert(cq_sptr);
  assert(channel);
  assert(data_sptr_->call_sptr);
  ClientInitMdCqTag* tag = new ClientInitMdCqTag(data_sptr_->call_sptr);
  if (tag->Start()) return;
  delete tag;
  data_sptr_->status.SetInternalError("Failed to init client stream.");
}

template <class Request>
Status ClientAsyncWriter<Request>::Finish(
    ::google::protobuf::Message* response) const {
  // XXX
  return Status::UNIMPLEMENTED;
}

  //assert(response);
  //assert(data_sptr_);
  //Data& data = *data_sptr_;
  //assert(data.call_sptr);
  //assert(data.cq_sptr);

  //Status& status = data.status;
  //if (!status.ok()) return status;
  //ClientWriterFinishCqTag tag(data.call_sptr);
  //if (!tag.Start()) {
  //  status.SetInternalError("Failed to finish client stream.");
  //  return status;
  //}

  //data.cq_sptr->Pluck(&tag);

  //// Todo: Get trailing metadata.
  //if (tag.IsStatusOk())
  //  status = tag.GetResponse(*response);
  //else
  //  status = tag.GetStatus();

  //return status;

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_ASYNC_WRITER_H
