# Overview

C++ callback implementation of gRPC.

# Design
CompletionQueueTag is the element of CompletionQueue.
On completion, ```CompletionQueueTag``` is poped, and ```DoComplete()``` is called,
and then got deleted.
```CompletionQueueTag``` keeps the input and output of one rpc call.
```CallOperations``` keeps references to members of ```CompletionQueueTag```,
and is used in ```Call::StartBatch()```.
Async rpc callback is got called in ```DoComplete()```.

## Writer's internal queue.
```ClientWriter```, ```ClientReaderWriter```, ```ServerWriter``` will queue the messages for writing,
because grpc can only send message one by one.
```AsyncWrite()``` will queue the message and return immediately.
```BlockingWrite()``` will block until all messages sent.
```Write()``` normaly will call ```AsnycWrite()```, but will call ```BlockingWrite()```
 if the queue size reaches to a "high queue size".
```SetHighQueueSize()``` to change it from the default one. 

Todo: set default high queue size

## Sync and async
Because completion queue can not mix ```pluck()``` and ```next()```, 
the stream rpc call must be sync or async.
Sync call uses own completion queue and pluck on each read or write.
Async call uses a shared completion queue and do next for all operations.

