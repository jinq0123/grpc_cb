# Advanced Usage
For basic usage, see tutorial in [../README.md](../README.md).

## Run multi stubs in one thread.

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
