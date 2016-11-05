// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL_H

#include <grpc_cb/channel.h>  // for MakeSharedCall()
#include <grpc_cb/impl/client/client_async_reader_helper.h>  // for ClientAsyncReaderHelper
#include <grpc_cb/impl/client/client_reader_data.h>  // for ClientReaderDataSptr
#include <grpc_cb/impl/client/client_send_close_cqtag.h>  // for ClientSendCloseCqTag
#include <grpc_cb/impl/client/client_writer_helper.h>  // for ClientWriterHelper
#include <grpc_cb/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/status.h>                      // for Status

#include <functional>  // for function
#include <string>

namespace grpc_cb {

template <class Request, class Response>
class ClientAsyncReaderWriterImpl GRPC_FINAL {
 public:
  inline ClientAsyncReaderWriterImpl(const ChannelSptr& channel,
                            const std::string& method,
                            const CompletionQueueSptr& cq_sptr);
  inline ~ClientAsyncReaderWriterImpl();

 public:
  inline bool Write(const Request& request) const;
  // AsyncCloseWriting() is optional. Auto closed on dtr().
  inline void CloseWriting();

  using ReadCallback = std::function<void(const Response&)>;
  inline void ReadEach(
      const ReadCallback& on_read,
      const StatusCallback& on_status = StatusCallback()) const;

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  using Data = ClientReaderData<Response>;
  using DataSptr = ClientReaderDataSptr<Response>;
  DataSptr data_sptr_;  // Same as reader. Easy to copy.
  bool writing_closed_ = false;  // Is AsyncCloseWriting() called?
};  // class ClientAsyncReaderWriterImpl<>

// Todo: BlockingGetInitMd();

template <class Request, class Response>
ClientAsyncReaderWriterImpl<Request, Response>::ClientAsyncReaderWriterImpl(
    const ChannelSptr& channel, const std::string& method,
    const CompletionQueueSptr& cq_sptr)
    // Todo: same as ClientReader?
    : data_sptr_(new Data{cq_sptr, channel->MakeSharedCall(method, *cq_sptr)}) {
  assert(cq_sptr);
  assert(channel);
  assert(data_sptr_->call_sptr);
  ClientInitMdCqTag* tag = new ClientInitMdCqTag(data_sptr_->call_sptr);
  if (tag->Start()) return;
  delete tag;
  data_sptr_->status.SetInternalError("Failed to init stream.");
}

template <class Request, class Response>
ClientAsyncReaderWriterImpl<Request, Response>::~ClientAsyncReaderWriterImpl() {
  CloseWriting();  // XXX
}

template <class Request, class Response>
bool ClientAsyncReaderWriterImpl<Request, Response>::Write(const Request& request) const {
  assert(data_sptr_);
  assert(data_sptr_->call_sptr);
  return ClientWriterHelper::AsyncWrite(data_sptr_->call_sptr,
      request, data_sptr_->status);
}

template <class Request, class Response>
void ClientAsyncReaderWriterImpl<Request, Response>::CloseWriting() {
  if (writing_closed_) return;
  writing_closed_ = true;
  Status& status = data_sptr_->status;
  if (!status.ok()) return;
  ClientSendCloseCqTag* tag = new ClientSendCloseCqTag(data_sptr_->call_sptr);
  if (tag->Start()) return;

  delete tag;
  status.SetInternalError("Failed to set stream writes done.");
}

// Todo: same as ClientReader?

template <class Request, class Response>
void ClientAsyncReaderWriterImpl<Request, Response>::ReadEach(
    const ReadCallback& on_read,
    const StatusCallback& on_status) const {
    data_sptr_->on_msg = on_read;
    data_sptr_->on_status = on_status;
    ClientAsyncReaderHelper::AsyncReadNext(data_sptr_);
}

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL_H
