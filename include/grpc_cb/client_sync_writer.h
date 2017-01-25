// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_SYNC_WRITER_H
#define GRPC_CB_CLIENT_SYNC_WRITER_H

#include <cassert>  // for assert()
#include <cstdint>  // for int64_t

#include <grpc_cb/channel.h>                           // for MakeSharedCall()
#include <grpc_cb/impl/call_sptr.h>                    // for CallSptr
#include <grpc_cb/impl/client/client_send_init_md_cqtag.h>  // for ClientSendInitMdCqTag
#include <grpc_cb/impl/client/client_sync_writer_helper.h>  // for ClientSyncWriterHelper
#include <grpc_cb/impl/client/client_writer_close_cqtag.h>  // for ClientWriterCloseCqTag
#include <grpc_cb/status.h>                                 // for Status

namespace grpc_cb {

// Copyable.
// Use template class instead of template member function
//    to ensure client input the correct request type.
// Todo: Use non_template class as the implement.
template <class Request>
class ClientSyncWriter GRPC_FINAL {
 public:
  inline ClientSyncWriter(const ChannelSptr& channel, const std::string& method,
                          int64_t timeout_ms);

  // Todo: BlockingGetInitMd();
  bool Write(const Request& request) const {
    Data& d = *data_sptr_;
    return ClientSyncWriterHelper::BlockingWrite(d.call_sptr, d.cq_sptr,
                                                 request, d.status);
  }

  Status Close(::google::protobuf::Message* response) const;

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  struct Data {
    CompletionQueueSptr cq_sptr;
    CallSptr call_sptr;
    Status status;
  };
  using DataSptr = std::shared_ptr<Data>;

  DataSptr data_sptr_;  // Easy to copy.
};  // class ClientSyncWriter<>

template <class Request>
ClientSyncWriter<Request>::ClientSyncWriter(const ChannelSptr& channel,
                                            const std::string& method,
                                            int64_t timeout_ms)
    // Todo: same as ClientReader?
    : data_sptr_(new Data) {
  assert(channel);
  CompletionQueueSptr cq_sptr(new CompletionQueue);
  CallSptr call_sptr = channel->MakeSharedCall(method, *cq_sptr, timeout_ms);
  data_sptr_->cq_sptr = cq_sptr;
  data_sptr_->call_sptr = call_sptr;
  ClientSendInitMdCqTag tag(call_sptr);
  if (tag.Start()) {
    cq_sptr->Pluck(&tag);
    return;
  }

  data_sptr_->status.SetInternalError("Failed to start client sync writer.");
}

template <class Request>
Status ClientSyncWriter<Request>::Close(
    ::google::protobuf::Message* response) const {
  assert(response);
  assert(data_sptr_);
  Data& data = *data_sptr_;
  assert(data.call_sptr);
  assert(data.cq_sptr);

  Status& status = data.status;
  if (!status.ok()) return status;
  ClientWriterCloseCqTag tag(data.call_sptr);
  if (!tag.Start()) {
    status.SetInternalError("Failed to finish client stream.");
    return status;
  }

  data.cq_sptr->Pluck(&tag);

  // Todo: Get trailing metadata.
  if (tag.IsStatusOk())
    status = tag.GetResponse(*response);
  else
    status = tag.GetStatus();

  return status;
}  // BlockingFinish()

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_SYNC_WRITER_H
