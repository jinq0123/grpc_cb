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

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpc/grpc.h>
#include <grpc_cb/server.h>
#include <grpc_cb/security/server_credentials.h>
#include "helper.h"
#include "route_guide.grpc_cb.pb.h"

using grpc_cb::Server;
using grpc_cb::ServerWriter;
using grpc_cb::Status;
using routeguide::Point;
using routeguide::Feature;
using routeguide::Rectangle;
using routeguide::RouteSummary;
using routeguide::RouteNote;
using std::chrono::system_clock;

using FeatureVector = std::vector<Feature>;

float ConvertToRadians(float num) {
  return float(num * 3.1415926 / 180);
}

float GetDistance(const Point& start, const Point& end) {
  const float kCoordFactor = 10000000.0;
  float lat_1 = start.latitude() / kCoordFactor;
  float lat_2 = end.latitude() / kCoordFactor;
  float lon_1 = start.longitude() / kCoordFactor;
  float lon_2 = end.longitude() / kCoordFactor;
  float lat_rad_1 = ConvertToRadians(lat_1);
  float lat_rad_2 = ConvertToRadians(lat_2);
  float delta_lat_rad = ConvertToRadians(lat_2-lat_1);
  float delta_lon_rad = ConvertToRadians(lon_2-lon_1);

  float a = pow(sin(delta_lat_rad/2), 2) + cos(lat_rad_1) * cos(lat_rad_2) *
            pow(sin(delta_lon_rad/2), 2);
  float c = 2 * atan2(sqrt(a), sqrt(1-a));
  int R = 6371000; // metres

  return R * c;
}

std::string GetFeatureName(const Point& point,
                           const FeatureVector& feature_vector) {
  for (const Feature& f : feature_vector) {
    if (f.location().latitude() == point.latitude() &&
        f.location().longitude() == point.longitude()) {
      return f.name();
    }
  }
  return "";
}

class RecordRoute_ReaderImpl
    : public routeguide::RouteGuide::Service::RecordRoute_Reader {
 public:
  explicit RecordRoute_ReaderImpl(const FeatureVector& feature_vec)
      : feature_vec_(feature_vec) {}

 protected:
  void OnMsg(const Point& point) override {
    RecordRouteResult& r = record_route_result_;
    r.point_count++;
    if (!GetFeatureName(point, feature_vec_).empty()) {
      r.feature_count++;
    }
    if (r.point_count != 1) {
      r.distance += GetDistance(r.previous, point);
    }
    r.previous = point;
  }

  void OnEnd() override {
    // Make a copy.
    const RecordRouteResult r = record_route_result_;
    Replier replier = GetReplier();
    std::thread t([r, replier]() {
      system_clock::time_point end_time = system_clock::now();
      RouteSummary summary;
      summary.set_point_count(r.point_count);
      summary.set_feature_count(r.feature_count);
      summary.set_distance(static_cast<long>(r.distance));
      auto secs = std::chrono::duration_cast<std::chrono::seconds>(
          end_time - r.start_time);
      summary.set_elapsed_time(int32_t(secs.count()));

      // Delayed reply.
      std::this_thread::sleep_for(std::chrono::seconds(1));
      replier.Reply(summary);
    });
    t.detach();
  }

 private:
  const FeatureVector& feature_vec_;

  struct RecordRouteResult {
      int point_count = 0;
      int feature_count = 0;
      float distance = 0.0;
      Point previous;
      system_clock::time_point start_time = system_clock::now();
  };

  RecordRouteResult record_route_result_;
};  // class RecordRoute_ReaderImpl

class RouteGuideImpl final : public routeguide::RouteGuide::Service {
 public:
  explicit RouteGuideImpl(const std::string& db) {
    routeguide::ParseDb(db, &feature_vector_);
  }

  void GetFeature(
      const Point& point,
      GetFeature_Replier replier) override {
    Feature feature;
    feature.set_name(GetFeatureName(point, feature_vector_));
    feature.mutable_location()->CopyFrom(point);
    replier.Reply(feature);
  }

  void ListFeatures(
      const routeguide::Rectangle& rectangle,
      ListFeatures_Writer writer) override {
    const FeatureVector& feature_vector = feature_vector_;
    std::thread t([rectangle, writer, &feature_vector]() {
      auto lo = rectangle.lo();
      auto hi = rectangle.hi();
      long left = (std::min)(lo.longitude(), hi.longitude());
      long right = (std::max)(lo.longitude(), hi.longitude());
      long top = (std::max)(lo.latitude(), hi.latitude());
      long bottom = (std::min)(lo.latitude(), hi.latitude());
      for (const Feature& f : feature_vector) {
        if (f.location().longitude() >= left &&
            f.location().longitude() <= right &&
            f.location().latitude() >= bottom &&
            f.location().latitude() <= top) {
          if (!writer.Write(f)) break;
          // std::this_thread::sleep_for(std::chrono::seconds(1));
        }
      }
      // Todo: auto writer.Close(Status::OK);
    });  // thread t
    t.detach();
  }

  // Todo: Need session id.
  RecordRoute_ReaderSptr RecordRoute(
      RecordRoute_Replier replier) override {
    return std::make_shared<RecordRoute_ReaderImpl>(feature_vector_);
  }  // RecordRoute()

  RouteChat_ReaderSptr RouteChat(RouteChat_Writer writer) override {
    class Reader : public RouteChat_Reader {
     protected:
      void OnMsg(const RouteNote& msg) override {
        for (const RouteNote& n : received_notes_) {
          if (n.location().latitude() == msg.location().latitude() &&
              n.location().longitude() == msg.location().longitude()) {
            GetWriter().Write(n);
          }  // if
        }  // for
        received_notes_.push_back(msg);
      }  // OnMsg()
    
      void OnEnd() override {
        RouteChat_Writer writer = GetWriter();
        std::thread t([writer]() {
          std::this_thread::sleep_for(std::chrono::seconds(1));
          writer.Write(RouteNote());
        });
        t.detach();
      }  // OnEnd()

     private:
       std::vector<RouteNote> received_notes_;
    };  // class Reader

    return std::make_shared<Reader>();
  }

 private:
  FeatureVector feature_vector_;
};

void RunServer(const std::string& db_path) {
  std::string server_address("0.0.0.0:50051");
  RouteGuideImpl service(db_path);

  Server svr;
  svr.AddListeningPort(server_address);
  svr.RegisterService(service);
  std::cout << "Server listening on " << server_address << std::endl;
  svr.BlockingRun();
}

int main(int argc, char** argv) {
  // Expect only arg: --db_path=path/to/route_guide_db.json.
  std::string db = routeguide::GetDbFileContent(argc, argv);
  RunServer(db);

  return 0;
}
