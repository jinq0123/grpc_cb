# Advanced Usage
For basic usage, see tutorial in [../README.md](../README.md).

## Run multi stubs in one thread.
Todo: Use TryNext()
```
Svc1::Stub stub1(channel);
Svc2::Stub stub2(channel);
StubRunner runner;
runner.AddStub(stub1);
runner.AddStub(stub2);
runner.BlockingRun();
```

## Mix sync and async stub calls.

```Stub``` uses an internal completion queue for async calls,
 and instantiate a completion queue for each sync calls,
 so ```Stub``` can mix sync and async calls.

```cpp
  ...
  auto f = std::async(std::launch::async, [&stub]() { 
    stub.BlockingRun();
  });

  Point point = MakePoint(0,0);
  stub.AsyncGetFeature(point);
  
  Feature feature;
  Status status = stub.BlockingGetFeature(point, &feature);
  ...
``` 
