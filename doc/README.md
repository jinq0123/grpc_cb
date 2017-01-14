# Overview

C++ callback implementation of gRPC.

# Status

Not ready

# Todo
* WriteOptions
* Add interfaces for protobuf bytes message.
* DoComplete(bool success) use success parameter.
* Handle ServerReplier, ServerReader ServerWriter likely.
* Examples of async server.
* Examples of async client stream.

# Design
CompletionQueueTag is the element of CompletionQueue.
On completion, CompletionQueueTag is poped, and DoComplete() is called,
and then got deleted.
CompletionQueueTag keeps the input and output of one rpc call.
CallOperations keeps references to members of CompletionQueueTag, and is 
used in Call::StartBatch().
Async rpc callback is got called in DoComplete().

## Writer's internal queue.
ClientWriter, ClientReaderWriter, ServerWriter will queue the messages for writing,
because grpc can only send message one by one.
AsyncWrite() will queue the message and return immediately.
BlockingWrite() will block until all messages sent.
Write() normaly will call AsnycWrite(), but will call BlockingWrite()
 if the queue size reaches to a "high queue size".
SetHighQueueSize() to change it from the default one. 

Todo: set default high queue size

## Sync and async
Because completion queue can not mix pluck() and next(), the stream rpc call must be sync or async.
Sync call uses own completion queue and pluck on each read or write.
Async call uses a shared completion queue and do next for all calls.

## Async reader writer interfaces
* SetOnStart() to get init metadata from reader/writer
* SetOnRead(on_read)
* SetOnEnd(on_end) to check status and get trail metadata

		on_end(status) or 
		on_end(status, response) for client stream

* Write()
* CloseWrite()


# Other
 
To generate grpc_cb files:
```
D:\Jinq\tools\protoc.exe ^
  -I D:\Jinq\Git\grpc_jinq0123\examples\protos ^
  --grpc_out=D:\Jinq\Git\grpc_jinq0123\examples\cpp\helloworld ^
  --plugin=protoc-gen-grpc=D:\Jinq\Git\grpc_jinq0123\vsprojects\Debug\grpc_cpp_cb_plugin.exe ^
  D:\Jinq\Git\grpc_jinq0123\examples\protos\helloworld.proto
```

Assert failed if too many ClientWriter::Write()

 	route_guide_cb_client.exe!issue_debug_notification(const wchar_t * const message) 行 125	C++
 	route_guide_cb_client.exe!__acrt_report_runtime_error(const wchar_t * message) 行 142	C++
 	route_guide_cb_client.exe!abort() 行 51	C++
	route_guide_cb_client.exe!perform_stream_op_locked(grpc_exec_ctx * exec_ctx, grpc_chttp2_transport_global * transport_global, grpc_chttp2_stream_global * stream_global, grpc_transport_stream_op * op) 行 833	C
 	route_guide_cb_client.exe!perform_stream_op(grpc_exec_ctx * exec_ctx, grpc_transport * gt, grpc_stream * gs, grpc_transport_stream_op * op) 行 906	C
 	route_guide_cb_client.exe!grpc_transport_perform_stream_op(grpc_exec_ctx * exec_ctx, grpc_transport * transport, grpc_stream * stream, grpc_transport_stream_op * op) 行 100	C
 	route_guide_cb_client.exe!con_start_transport_stream_op(grpc_exec_ctx * exec_ctx, grpc_call_element * elem, grpc_transport_stream_op * op) 行 73	C
 	route_guide_cb_client.exe!grpc_call_next_op(grpc_exec_ctx * exec_ctx, grpc_call_element * elem, grpc_transport_stream_op * op) 行 230	C
 	route_guide_cb_client.exe!hc_start_transport_op(grpc_exec_ctx * exec_ctx, grpc_call_element * elem, grpc_transport_stream_op * op) 行 142	C
 	route_guide_cb_client.exe!grpc_subchannel_call_process_op(grpc_exec_ctx * exec_ctx, grpc_subchannel_call * call, grpc_transport_stream_op * op) 行 671	C
 	route_guide_cb_client.exe!grpc_subchannel_call_holder_perform_op(grpc_exec_ctx * exec_ctx, grpc_subchannel_call_holder * holder, grpc_transport_stream_op * op) 行 98	C
 	route_guide_cb_client.exe!cc_start_transport_stream_op(grpc_exec_ctx * exec_ctx, grpc_call_element * elem, grpc_transport_stream_op * op) 行 110	C
 	route_guide_cb_client.exe!grpc_call_next_op(grpc_exec_ctx * exec_ctx, grpc_call_element * elem, grpc_transport_stream_op * op) 行 230	C
 	route_guide_cb_client.exe!compress_start_transport_stream_op(grpc_exec_ctx * exec_ctx, grpc_call_element * elem, grpc_transport_stream_op * op) 行 229	C
 	route_guide_cb_client.exe!execute_op(grpc_exec_ctx * exec_ctx, grpc_call * call, grpc_transport_stream_op * op) 行 722	C
 	route_guide_cb_client.exe!call_start_batch(grpc_exec_ctx * exec_ctx, grpc_call * call, const grpc_op * ops, unsigned int nops, void * notify_tag, int is_notify_tag_closure) 行 1338	C
 	route_guide_cb_client.exe!grpc_call_start_batch(grpc_call * call, const grpc_op * ops, unsigned int nops, void * tag, void * reserved) 行 1384	C
 	route_guide_cb_client.exe!grpc_cb::Call::StartBatch(const grpc_cb::CallOperations & ops, void * tag) 行 57	C++
 	route_guide_cb_client.exe!grpc_cb::ClientSendMsgCqTag::Start(const google::protobuf::Message & message) 行 32	C++
 	route_guide_cb_client.exe!grpc_cb::ClientWriterHelper::Write(const std::shared_ptr<grpc_cb::Call> & call_sptr, const google::protobuf::Message & request, grpc_cb::Status & status) 行 28	C++
 	route_guide_cb_client.exe!grpc_cb::ClientWriter<routeguide::Point>::Write(const routeguide::Point & request) 行 31	C++
 	route_guide_cb_client.exe!TimeRecordRouteAsync(const std::shared_ptr<grpc_cb::Channel> & channel, const std::basic_string<char,std::char_traits<char>,std::allocator<char> > & db) 行 315	C++
 	route_guide_cb_client.exe!main(int argc, char * * argv) 行 381	C++
 	route_guide_cb_client.exe!invoke_main() 行 64	C++
 	route_guide_cb_client.exe!__scrt_common_main_seh() 行 253	C++
 	route_guide_cb_client.exe!__scrt_common_main() 行 296	C++
 	route_guide_cb_client.exe!mainCRTStartup() 行 17	C++
 	kernel32.dll!75b5336a()	未知
 	[下面的框架可能不正确和/或缺失，没有为 kernel32.dll 加载符号]	
 	ntdll.dll!77d09902()	未知
 	ntdll.dll!77d098d5()	未知
