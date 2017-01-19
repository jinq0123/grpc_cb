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
using grpc_cb::ClientAsyncReader;
using grpc_cb::ClientAsyncReaderWriter;
using grpc_cb::ClientAsyncWriter;
using grpc_cb::ClientSyncReader;
using grpc_cb::ClientSyncReaderWriter;
using grpc_cb::ClientSyncWriter;
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

void RunWriteRouteNote(ClientSyncReaderWriter<RouteNote, RouteNote>
    sync_reader_writer) {
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
      : stub_(routeguide::RouteGuide::NewStub(channel)) {
    routeguide::ParseDb(db, &feature_list_);
    assert(!feature_list_.empty());
  }

  void BlockingGetFeature() {
    Point point;
    Feature feature;
    point = MakePoint(409146138, -746188906);
    BlockingGetOneFeature(point, &feature);
    point = MakePoint(0, 0);
    BlockingGetOneFeature(point, &feature);
  }

  void BlockingListFeatures() {
    routeguide::Rectangle rect = MakeRect(
        400000000, -750000000, 420000000, -730000000);
    Feature feature;

    std::cout << "Looking for features between 40, -75 and 42, -73"
        << std::endl;

    auto reader(stub_->SyncListFeatures(rect));
    while (reader.ReadOne(&feature)) {
      std::cout << "Found feature called "
                << feature.name() << " at "
                << feature.location().latitude()/kCoordFactor << ", "
                << feature.location().longitude()/kCoordFactor << std::endl;
    }
    Status status = reader.RecvStatus();
    if (status.ok()) {
      std::cout << "ListFeatures rpc succeeded." << std::endl;
    } else {
      std::cout << "ListFeatures rpc failed." << std::endl;
    }
  }

  void BlockingRecordRoute() {
    Point point;
    const int kPoints = 10;
    std::uniform_int_distribution<int> feature_distribution(
        0, feature_list_.size() - 1);

    auto writer(stub_->SyncRecordRoute());
    for (int i = 0; i < kPoints; i++) {
      const Feature& f = feature_list_[feature_distribution(generator)];
      std::cout << "Visiting point "
                << f.location().latitude()/kCoordFactor << ", "
                << f.location().longitude()/kCoordFactor << std::endl;
      if (!writer.Write(f.location())) {
        // Broken stream.
        break;
      }
      RandomSleep();
    }
    RouteSummary stats;
    // Recv reponse and status.
    Status status = writer.Close(&stats);  // Todo: timeout
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

  void BlockingRouteChat() {
    ClientSyncReaderWriter<RouteNote, RouteNote> sync_reader_writer(
        stub_->SyncRouteChat());

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
  bool BlockingGetOneFeature(const Point& point, Feature* feature) {
    Status status = stub_->BlockingGetFeature(point, feature);
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
  stub.AsyncGetFeature(MakePoint(0, 0),
                       [](const Feature& feature) { PrintFeature(feature); });

  // Ignore response.
  stub.AsyncGetFeature(MakePoint(0, 0));

  Point point1 = MakePoint(409146138, -746188906);
  stub.AsyncGetFeature(point1,
      [&stub](const Feature& feature) {
        PrintFeature(feature);
        stub.Shutdown();
      },
      [&stub](const Status& err) {
        std::cout << "AsyncGetFeature rpc failed. "
            << err.GetDetails() << std::endl;
        stub.Shutdown();
      });  // AsyncGetFeature()
  stub.BlockingRun();  // until stub.Shutdown()
}

void ListFeaturesAsync(const ChannelSptr& channel) {
  Stub stub(channel);
  routeguide::Rectangle rect = MakeRect(
      400000000, -750000000, 420000000, -730000000);
  std::cout << "Looking for features between 40, -75 and 42, -73" << std::endl;

  stub.AsyncListFeatures(rect,
    [](const Feature& feature){
      std::cout << "Got feature " << feature.name() << " at "
          << feature.location().latitude()/kCoordFactor << ", "
          << feature.location().longitude()/kCoordFactor << std::endl;
    },
    [&stub](const Status& status){
      std::cout << "End status: (" << status.GetCode() << ") "
          << status.GetDetails() << std::endl;
      stub.Shutdown();  // To break BlockingRun().
    });
  stub.BlockingRun();  // until stub.Shutdown()
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
  auto f = std::async(std::launch::async, [&stub]() { stub.BlockingRun(); });

  // ClientAsyncWriter<Point, RouteSummary> writer;
  auto writer = stub.AsyncRecordRoute();
  for (int i = 0; i < kPoints; i++) {
    const Feature& f = feature_list[feature_distribution(generator)];
    std::cout << "Visiting point "
              << f.location().latitude()/kCoordFactor << ", "
              << f.location().longitude()/kCoordFactor << std::endl;
    if (!writer.Write(f.location())) {
      // Broken stream.
      break;
    }
    RandomSleep();
  }
  // Recv reponse and status.
  writer.Close([](const Status& status, const RouteSummary& resp) {
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

void AsyncWriteRouteNotes(ClientAsyncReaderWriter<RouteNote, RouteNote>
    async_reader_writer) {
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
    // RandomSleep();
  }
  async_reader_writer.CloseWriting();  // Optional.
}

void RouteChatAsync(const ChannelSptr& channel) {
  Stub stub(channel);
  std::atomic_bool bReaderDone = false;
  ClientAsyncReaderWriter<RouteNote, RouteNote> async_reader_writer(
      stub.AsyncRouteChat([&bReaderDone](const Status& status) {
        if (!status.ok()) {
          std::cout << "RouteChat rpc failed. " << status.GetDetails()
                    << std::endl;
        }
        bReaderDone = true;
      }));

  async_reader_writer.ReadEach(
      [](const RouteNote& note) { PrintServerNote(note); });

  AsyncWriteRouteNotes(async_reader_writer);
  auto f = std::async(std::launch::async, [&stub]() { 
    stub.BlockingRun();
  });

  while (!bReaderDone)
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  stub.Shutdown();  // To break BlockingRun().
}  // RouteChatAsync()

int main(int argc, char** argv) {
  // Expect only arg: --db_path=path/to/route_guide_db.json.
  std::string db = routeguide::GetDbFileContent(argc, argv);
  assert(!db.empty());
  ChannelSptr channel(new Channel("localhost:50051"));
  RouteGuideClient guide(channel, db);

  std::cout << "---- BlockingGetFeature --------------" << std::endl;
  guide.BlockingGetFeature();
  std::cout << "---- BlockingListFeatures --------------" << std::endl;
  guide.BlockingListFeatures();
  std::cout << "---- BlockingRecordRoute --------------" << std::endl;
  guide.BlockingRecordRoute();
  std::cout << "---- BlockingRouteChat --------------" << std::endl;
  guide.BlockingRouteChat();

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
