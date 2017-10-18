/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <atomic>
#include <chrono>
#include <future>  // for async()
#include <iostream>
#include <memory>
#include <random>
#include <string>

#include "helper.h"
#include "route_guide.grpc_cb.pb.h"

using grpc_cb::Channel;
using grpc_cb::ChannelSptr;
using grpc_cb::Status;
using routeguide::Point;
using routeguide::Feature;
using routeguide::Rectangle;
using routeguide::RouteSummary;
using routeguide::RouteNote;
using routeguide::RouteGuide::Stub;

const float kCoordFactor = 10000000.0;
static unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
static std::default_random_engine generator(seed);

Point MakePoint(long latitude, long longitude) {
  Point p;
  p.set_latitude(latitude);
  p.set_longitude(longitude);
  return p;
}

routeguide::Rectangle MakeRect(const Point& lo, const Point& hi) {
  routeguide::Rectangle rect;
  *rect.mutable_lo() = lo;
  *rect.mutable_hi() = hi;
  return rect;
}

routeguide::Rectangle MakeRect(long lo_latitude, long lo_longitude,
                   long hi_latitude, long hi_longitude) {
  return MakeRect(MakePoint(lo_latitude, lo_longitude),
                  MakePoint(hi_latitude, hi_longitude));
}

Feature MakeFeature(const std::string& name,
                    long latitude, long longitude) {
  Feature f;
  f.set_name(name);
  f.mutable_location()->CopyFrom(MakePoint(latitude, longitude));
  return f;
}

RouteNote MakeRouteNote(const std::string& message,
                        long latitude, long longitude) {
  RouteNote n;
  n.set_message(message);
  n.mutable_location()->CopyFrom(MakePoint(latitude, longitude));
  return n;
}

void PrintFeature(const Feature& feature) {
  if (!feature.has_location()) {
    std::cout << "Server returns incomplete feature." << std::endl;
    return;
  }
  if (feature.name().empty()) {
    std::cout << "Found no feature at "
        << feature.location().latitude()/kCoordFactor << ", "
        << feature.location().longitude()/kCoordFactor << std::endl;
  }
  else {
    std::cout << "Found feature called " << feature.name()  << " at "
        << feature.location().latitude()/kCoordFactor << ", "
        << feature.location().longitude()/kCoordFactor << std::endl;
  }
}  // PrintFeature()

void PrintServerNote(const RouteNote& server_note) {
  std::cout << "Got message " << server_note.message()
            << " at " << server_note.location().latitude() << ", "
            << server_note.location().longitude() << std::endl;
}

void RandomSleep() {
  std::uniform_int_distribution<int> delay_distribution(500, 1500);
  std::this_thread::sleep_for(std::chrono::milliseconds(
      delay_distribution(generator)));
}

void RunWriteRouteNote(Stub::RouteChat_SyncReaderWriter sync_reader_writer) {
  std::vector<RouteNote> notes{
    MakeRouteNote("First message", 0, 0),
    MakeRouteNote("Second message", 0, 1),
    MakeRouteNote("Third message", 1, 0),
    MakeRouteNote("Fourth message", 0, 0)};
  for (const RouteNote& note : notes) {
    std::cout << "Sending message " << note.message()
              << " at " << note.location().latitude() << ", "
              << note.location().longitude() << std::endl;
    sync_reader_writer.Write(note);
    // RandomSleep();
  }
  sync_reader_writer.CloseWriting();
}

class RouteGuideClient {
 public:
  RouteGuideClient(std::shared_ptr<Channel> channel, const std::string& db)
      : stub_(new Stub(channel)) {
    routeguide::ParseDb(db, &feature_list_);
    assert(!feature_list_.empty());
  }

  void SyncGetFeature() {
    Point point;
    Feature feature;
    point = MakePoint(409146138, -746188906);
    SyncGetOneFeature(point, &feature);
    point = MakePoint(0, 0);
    SyncGetOneFeature(point, &feature);
  }

  void SyncListFeatures() {
    routeguide::Rectangle rect = MakeRect(
        400000000, -750000000, 420000000, -730000000);
    Feature feature;

    std::cout << "Looking for features between 40, -75 and 42, -73"
        << std::endl;

    auto sync_reader(stub_->Sync_ListFeatures(rect));
    while (sync_reader.ReadOne(&feature)) {
      std::cout << "Found feature called "
                << feature.name() << " at "
                << feature.location().latitude()/kCoordFactor << ", "
                << feature.location().longitude()/kCoordFactor << std::endl;
    }
    Status status = sync_reader.RecvStatus();
    if (status.ok()) {
      std::cout << "ListFeatures rpc succeeded." << std::endl;
    } else {
      std::cout << "ListFeatures rpc failed." << std::endl;
    }
  }

  void SyncRecordRoute() {
    Point point;
    const int kPoints = 10;
    std::uniform_int_distribution<int> feature_distribution(
        0, feature_list_.size() - 1);

    auto sync_writer(stub_->Sync_RecordRoute());
    for (int i = 0; i < kPoints; i++) {
      const Feature& f = feature_list_[feature_distribution(generator)];
      std::cout << "Visiting point "
                << f.location().latitude()/kCoordFactor << ", "
                << f.location().longitude()/kCoordFactor << std::endl;
      if (!sync_writer.Write(f.location())) {
        // Broken stream.
        break;
      }
      RandomSleep();
    }
    RouteSummary stats;
    // Recv reponse and status.
    Status status = sync_writer.Close(&stats);  // Todo: timeout
    if (status.ok()) {
      std::cout << "Finished trip with " << stats.point_count() << " points\n"
                << "Passed " << stats.feature_count() << " features\n"
                << "Traveled " << stats.distance() << " meters\n"
                << "It took " << stats.elapsed_time() << " seconds"
                << std::endl;
    } else {
      std::cout << "RecordRoute rpc failed." << std::endl;
    }
  }

  // Todo: Callback on client stream response and status.

  void SyncRouteChat() {
    auto sync_reader_writer(stub_->Sync_RouteChat());
    auto f = std::async(std::launch::async, [sync_reader_writer]() {
        RunWriteRouteNote(sync_reader_writer);
    });

    RouteNote server_note;
    while (sync_reader_writer.ReadOne(&server_note))
        PrintServerNote(server_note);

    f.wait();
    // Todo: Close() should auto close writing.
    Status status = sync_reader_writer.RecvStatus();
    if (!status.ok()) {
      std::cout << "RouteChat rpc failed." << std::endl;
    }
  }

 private:
  bool SyncGetOneFeature(const Point& point, Feature* feature) {
    Status status = stub_->Sync_GetFeature(point, feature);
    if (!status.ok()) {
      std::cout << "GetFeature rpc failed." << std::endl;
      return false;
    }
    PrintFeature(*feature);
    return feature->has_location();
  }

  std::unique_ptr<Stub> stub_;
  std::vector<Feature> feature_list_;
};

void GetFeatureAsync(const ChannelSptr& channel) {
  Stub stub(channel);

  // Ignore error status.
  stub.Async_GetFeature(MakePoint(0, 0),
                       [](const Feature& feature) { PrintFeature(feature); });

  // Ignore response.
  stub.Async_GetFeature(MakePoint(0, 0));

  Point point1 = MakePoint(409146138, -746188906);
  stub.Async_GetFeature(point1,
      [&stub](const Feature& feature) {
        PrintFeature(feature);
        stub.Shutdown();
      },
      [&stub](const Status& err) {
        std::cout << "AsyncGetFeature rpc failed. "
            << err.GetDetails() << std::endl;
        stub.Shutdown();
      });  // AsyncGetFeature()
  stub.Run();  // until stub.Shutdown()
}

void ListFeaturesAsync(const ChannelSptr& channel) {
  Stub stub(channel);
  routeguide::Rectangle rect = MakeRect(
      400000000, -750000000, 420000000, -730000000);
  std::cout << "Looking for features between 40, -75 and 42, -73" << std::endl;

  stub.Async_ListFeatures(rect,
    [](const Feature& feature) {
      std::cout << "Found feature called " << feature.name() << " at "
          << feature.location().latitude()/kCoordFactor << ", "
          << feature.location().longitude()/kCoordFactor << std::endl;
    },
    [&stub](const Status& status) {
      if (status.ok()) {
        std::cout << "ListFeatures rpc succeeded." << std::endl;
      } else {
        std::cout << "ListFeatures rpc failed." << std::endl;
      }
      stub.Shutdown();  // To break Run().
    });
  stub.Run();  // until stub.Shutdown()
}

void RecordRouteAsync(const ChannelSptr& channel,
                      const std::string& db) {
  assert(!db.empty());
  std::vector<Feature> feature_list;
  routeguide::ParseDb(db, &feature_list);
  assert(!feature_list.empty());

  Point point;
  const int kPoints = 10;
  std::uniform_int_distribution<int> feature_distribution(
      0, feature_list.size() - 1);

  Stub stub(channel);
  auto f = std::async(std::launch::async, [&stub]() { stub.Run(); });

  // ClientAsyncWriter<Point, RouteSummary> async_writer;
  auto async_writer = stub.Async_RecordRoute();
  for (int i = 0; i < kPoints; i++) {
    const Feature& f = feature_list[feature_distribution(generator)];
    std::cout << "Visiting point "
              << f.location().latitude()/kCoordFactor << ", "
              << f.location().longitude()/kCoordFactor << std::endl;
    if (!async_writer.Write(f.location())) {
      // Broken stream.
      break;
    }
    RandomSleep();
  }
  // Recv reponse and status.
  async_writer.Close([](const Status& status, const RouteSummary& resp) {
    if (!status.ok()) {
      std::cout << "RecordRoute rpc failed." << std::endl;
      return;
    }
    std::cout << "Finished trip with " << resp.point_count() << " points\n"
              << "Passed " << resp.feature_count() << " features\n"
              << "Traveled " << resp.distance() << " meters\n"
              << "It took " << resp.elapsed_time() << " seconds" << std::endl;
  });

  // Todo: timeout

  stub.Shutdown();
}  // RecordRouteAsync()

void AsyncWriteRouteNotes(Stub::RouteChat_AsyncReaderWriter async_reader_writer) {
  std::vector<RouteNote> notes{
    MakeRouteNote("First message", 0, 0),
    MakeRouteNote("Second message", 0, 1),
    MakeRouteNote("Third message", 1, 0),
    MakeRouteNote("Fourth message", 0, 0)};
  for (const RouteNote& note : notes) {
    std::cout << "Sending message " << note.message()
              << " at " << note.location().latitude() << ", "
              << note.location().longitude() << std::endl;
    async_reader_writer.Write(note);
    RandomSleep();
  }
  async_reader_writer.CloseWriting();  // Tell server.
}

void RouteChatAsync(const ChannelSptr& channel) {
  Stub stub(channel);
  auto f_run = std::async(std::launch::async, [&stub]() { 
    stub.Run();
  });

  std::atomic_bool bReaderDone = false;
  auto async_reader_writer(
      stub.Async_RouteChat([&bReaderDone](const Status& status) {
        if (!status.ok()) {
          std::cout << "RouteChat rpc failed. " << status.GetDetails()
                    << std::endl;
        }
        bReaderDone = true;
      }));

  async_reader_writer.ReadEach(
      [](const RouteNote& note) { PrintServerNote(note); });
  AsyncWriteRouteNotes(async_reader_writer);

  while (!bReaderDone)
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  stub.Shutdown();  // To break Run().
}  // RouteChatAsync()

void TestRpcTimeout(const ChannelSptr& channel) {
  Stub stub(channel);
  stub.SetCallTimeoutMs(INT64_MIN);
  Point point = MakePoint(0, 0);
  Feature feature;
  Status status = stub.Sync_GetFeature(point, &feature);
  assert(status.GetCode() == GRPC_STATUS_DEADLINE_EXCEEDED);
}

int main(int argc, char** argv) {
  // Expect only arg: --db_path=path/to/route_guide_db.json.
  std::string db = routeguide::GetDbFileContent(argc, argv);
  assert(!db.empty());
  ChannelSptr channel(new Channel("localhost:50051"));
  RouteGuideClient guide(channel, db);

  TestRpcTimeout(channel);

  std::cout << "---- SyncGetFeature --------------" << std::endl;
  guide.SyncGetFeature();
  std::cout << "---- SyncListFeatures --------------" << std::endl;
  guide.SyncListFeatures();
  std::cout << "---- SyncRecordRoute --------------" << std::endl;
  guide.SyncRecordRoute();
  std::cout << "---- SyncRouteChat --------------" << std::endl;
  guide.SyncRouteChat();

  std::cout << "---- GetFeatureAsync ----" << std::endl;
  GetFeatureAsync(channel);
  std::cout << "---- ListFeaturesAsync ----" << std::endl;
  ListFeaturesAsync(channel);
  std::cout << "---- RecordRouteAsnyc ----" << std::endl;
  RecordRouteAsync(channel, db);
  std::cout << "---- RouteChatAsync ---" << std::endl;
  RouteChatAsync(channel);

  return 0;
}
