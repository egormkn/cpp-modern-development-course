#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "config.h"
#include "graph.h"
#include "router.h"

struct Response;

class TransportDatabase {
 public:
  struct Settings {
    int bus_wait_time;    // minutes
    double bus_velocity;  // km/h
  };

  TransportDatabase(Settings settings);

  void AddStop(std::string stop_name, double latitude, double longitude,
               std::unordered_map<std::string, int> distances = {});

  void AddBus(std::string bus_name, std::vector<std::string> stop_names,
              bool is_roundtrip);

  void Preprocess();

  std::unique_ptr<Response> GetBus(const std::string& bus_name) const;

  std::unique_ptr<Response> GetStop(const std::string& stop_name) const;

  std::unique_ptr<Response> GetRoute(const std::string& stop_name_from,
                                     const std::string& stop_name_to) const;

 private:
  struct StopData {
    double latitude;
    double longitude;
    std::unordered_map<std::string, int> distances;
    std::unordered_set<std::string> buses;
    std::unordered_set<std::string> end_buses;
  };

  struct BusData {
    size_t stops_on_route;
    size_t unique_stops;
    double route_length;
    double curvature;
  };

  Settings settings;
  std::unordered_map<std::string, StopData> stop_data;
  std::unordered_map<std::string, BusData> bus_data;

  struct Vertex {
    std::string stop_name;
    std::string bus_name;
    bool is_roundtrip_end;

    bool operator==(const Vertex& other) const;
  };

  struct VertexHasher {
    size_t operator()(const Vertex& vertex) const;

   private:
    std::hash<std::string> string_hasher;
    std::hash<bool> bool_hasher;
    static const size_t prime = 19937;
  };

  std::vector<Vertex> vertices;
  std::unordered_map<Vertex, size_t, VertexHasher> vertex_ids;
  Graph::DirectedWeightedGraph<double> graph;
  std::unique_ptr<Graph::Router<double>> router;

  size_t AddVertex(Vertex vertex);

  double GetShortestDistance(const std::string& stop_name_from,
                             const std::string& stop_name_to);

  double GetRoadDistance(const std::string& stop_name_from,
                         const std::string& stop_name_to);
};