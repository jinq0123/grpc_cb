// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_GRPC_CB_H
#define GRPC_CB_GRPC_CB_H

#include <grpc_cb/blocking_run.h>  // for Run()
#include <grpc_cb/channel.h>       // for Channel
#include <grpc_cb/client_async_reader.h>
#include <grpc_cb/client_async_reader_writer.h>
#include <grpc_cb/client_async_writer.h>
#include <grpc_cb/client_sync_reader.h>
#include <grpc_cb/client_sync_reader_writer.h>
#include <grpc_cb/client_sync_writer.h>
#include <grpc_cb/completion_queue_for_next.h>  // for CompletionQueueForNext
#include <grpc_cb/completion_queue_for_next_sptr.h>  // for CompletionQueueForNextSptr
#include <grpc_cb/server.h>           // for Server
#include <grpc_cb/server_reader.h>    // for ServerReader<>
#include <grpc_cb/server_reader_for_bidi_streaming.h>
#include <grpc_cb/server_reader_for_client_only_streaming.h>
#include <grpc_cb/server_replier.h>   // for ServerReplier<>
#include <grpc_cb/server_writer.h>    // for ServerWriter<>
#include <grpc_cb/service.h>          // for Service
#include <grpc_cb/service_sptr.h>     // for ServiceSptr
#include <grpc_cb/service_stub.h>     // for ServiceStub
#include <grpc_cb/status.h>           // for Status
#include <grpc_cb/status_callback.h>  // for StatusCallback

#endif  // GRPC_CB_GRPC_CB_H
