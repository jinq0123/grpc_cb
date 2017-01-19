# gRPC_cb
C++ [gRPC](http://www.grpc.io/) library with callback interface. Depend on grpc but not on grpc++. It is a replacement of grpc++. Easier to use.

(Not ready)

## Build
1. Copy [grpc](https://github.com/grpc/grpc) as third-party/grpc, and build grpc.
	* To ease the building on Windows: [plasticbox/grpc-windows](https://github.com/plasticbox/grpc-windows)
2. Use build/grpc_cb.sln or build/Makefile to build grpc_cb.

## Done
* grpc_cpp_cb_plugin
* grpc_cb library
* helloworld example
* route_guide example

## Todo
1. Delete NewStub()
1. Check thread-safety
1. Rename ...CallBack to On...
1. Make impl sptr const:

		const std::shared_ptr<Impl> impl_sptr_;

1. Lua binding. Convert grpc_byte_buffer to string, which is needed by lua.
1. Timeout
1. Security
1. Support message types other than protobuffer.

## Tutorial
### Generating client and server code
examples/protos/generate.bat is an example to generate client and server interfaces from .proto file, which runs:

		protoc -I . --cpp_out=../cpp_cb/route_guide route_guide.proto
		protoc -I . --grpc_out=../cpp_cb/route_guide --plugin=protoc-gen-grpc=grpc_cpp_cb_plugin.exe route_guide.proto

This generates the following files in directory examples/cpp_cb/route_guide
* `route_guide.pb.h`, generated message classes header
* `route_guide.pb.cc`, the implementation of message classes
* `route_guide.grpc_cb.pb.h`, generated service classes header
* `route_guide.grpc_cb.pb.cc`, the implementation of service classes

The generated namespace ```RouteGuide``` contains
* a ```Stub``` class for clients to call.
* a ```Service``` class for servers to implement.

### Creating the client
See examples/cpp_cb/route_guide/route_guide_cb_client.cc.

#### Creating a stub
1. Create a shared ```Channel```, specifying the server address.
	```cpp
	ChannelSptr channel(new Channel("localhost:50051"));
	```

1. Instantiate a ```Stub```
	```cpp
	Stub stub(channel);
	```

#### Calling service methods
+ Blocking call
	* Simple RPC
		```cpp
		Point point = MakePoint(0, 0);
		Feature feature;
		Status status = stub.BlockingGetFeature(point, &feature);
		```

	* Server-side streaming RPC
		```cpp
		auto reader(stub_->SyncListFeatures(rect));
		while (reader.ReadOne(&feature)) {
			cout << feature.name() << endl;
		}
		Status status = reader.RecvStatus();
		```

	* Client-side streaming RPC
		```cpp
		auto writer(stub_->SyncRecordRoute());
		for (int i = 0; i < kPoints; i++) {
			const Feature& f = GetRandomFeature();
			if (!writer.Write(f.location())) {
				// Broken stream.
				break;
			}
		}
		
		// Recv reponse and status.
		RouteSummary stats;
		Status status = writer.Close(&stats);
		```

	* Bidirectional streaming RPC
		```cpp
		auto sync_reader_writer(stub_->SyncRouteChat());
		auto f = std::async(std::launch::async, [sync_reader_writer]() {
			RunWriteRouteNote(sync_reader_writer);
		});
	
		RouteNote server_note;
		while (sync_reader_writer.ReadOne(&server_note))
			PrintServerNote(server_note);
	
		f.wait();
		Status status = sync_reader_writer.RecvStatus();
		```

		```cpp
		void RunWriteRouteNote(RouteChat_SyncReaderWriter sync_reader_writer) {
			std::vector<RouteNote> notes{ ... };
			for (const RouteNote& note : notes) {
				sync_reader_writer.Write(note);
				RandomSleep();
			}
			sync_reader_writer.CloseWriting();
		}
		```
+ Asycn call
	* Simple RPC: ```AsyncGetFeature()```
		+ With response callback
			```cpp
			Point point = MakePoint(0, 0);
			stub.AsyncGetFeature(point,
				[](const Feature& feature) {
					PrintFeature(feature);
				});
			```

		+ Ignoring response
			```cpp
			stub.AsyncGetFeature(point);
			```

		+ With error callback
			```cpp
			stub.AsyncGetFeature(point,
				[](const Feature& feature) { PrintFeature(feature); },
				[](const Status& err) {
					cout << err.GetDetails() << endl;
				});  // AsyncGetFeature()
			```

	* Run the stub
		+ Async calls need 
			```cpp
			stub.BlockingRun();  // until stub.Shutdown()
			```

		+ It can run in other thread.
		+ It can be before or after async calls.
		+ ```stub.Shutdown()``` or ```~Stub()``` to end ```stub.BlockingRun()```.

	* Server-side streaming RPC
	* Client-side streaming RPC
	* Bidirectional streaming RPC

### Creating the server
See examples/cpp_cb/route_guide/route_guide_server.cc.
