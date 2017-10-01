# Advanced Usage
For basic usage, see tutorial in [../README.md](../README.md).

## Run multi stubs in one thread
Work in progress...
```
auto cq = std::make_shared<CQueueForNext>();
Svc1::Stub stub1(channel, cq);
Svc2::Stub stub2(channel, cq);
Run(cq);
```

## Run a stub in multi threads
```cpp
auto f1 = std::async(async, [stub]() { stub.Run(); }
auto f2 = std::async(async, [stub]() { stub.Run(); }
```

## Mix sync and async stub calls

```Stub``` uses an internal completion queue for async calls,
 and instantiate a completion queue for each sync operations,
 so ```Stub``` can mix sync and async calls.

```cpp
  ...
  auto f = std::async(std::launch::async, [&stub]() { 
    stub.Run();
  });

  Point point = MakePoint(0,0);
  stub.AsyncGetFeature(point);
  
  Feature feature;
  Status status = stub.SyncGetFeature(point, &feature);
  ...
```

## Set default error callback
Use ```ServiceStub::SetErrorCallback()``` to set a default error callback for this stub.
```cpp
RouteGuide::Stub stub(channel);
stub.SetErrorCallback([](const Status& status) {
	cout << "RouteGuide: " << status.GetDetails() << endl;
});
```

Use ```ServiceStub::SetDefaultErrorCallback()``` to set a default error callback for all the new stubs.
```cpp
ServiceStub::SetDefaultErrorCallback([](const Status& status) {
	cout << status.GetDetails() << endl;
});
Stub1 stub1(channel);
Stub2 stub2(channel);
```

Note: ```SetErrorCallback()``` and ```SetDefaultErrorCallback()``` are not thread-safe.

## Timeout RPC calls

* Set timeout on channel
	```cpp
	ChannelSptr channel(new Channel("localhost:50051"));
	Stub stub(channel);
	channel->SetCallTimeoutMs(1000);  // 1 second
	stub->SyncSayHello();
	```
	
* Set timeout on stub
	```cpp
	Stub stub(channel);
	stub.SetCallTimeoutMs(1000);
	```

* Set timeout on call
	+ To be implemented
	+ Use ClientContext
	
