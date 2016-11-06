#ifndef GRPC_CB_IMPL_CLINET_CLIENT_READER_DATA_H
#define GRPC_CB_IMPL_CLINET_CLIENT_READER_DATA_H

#include <memory>  // for shared_ptr<>

#include <grpc_cb/impl/call_sptr.h>              // for CallSptr
#include <grpc_cb/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/status.h>                      // for Status
#include <grpc_cb/status_callback.h>             // for StatusCallback

namespace grpc_cb {

// Todo: Delete it.

// Wrap all data in shared struct pointer to make copy quick.
template <class Response>
struct ClientReaderData {
    CompletionQueueSptr cq_sptr;
    CallSptr call_sptr;
    Status status;

    using MsgCallback = std::function<void(const Response&)>;
    MsgCallback on_msg;
    StatusCallback on_status;
};

template <class Response>
using ClientReaderDataSptr = std::shared_ptr<ClientReaderData<Response>>;

}  // namespace grpc_cb
#endif  // GRPC_CB_IMPL_CLINET_CLIENT_READER_DATA_H
