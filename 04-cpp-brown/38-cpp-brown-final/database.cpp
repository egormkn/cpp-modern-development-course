#include "database.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>

#include "response.h"

using namespace std;

TransportDatabase::TransportDatabase(Settings settings)
    : settings(move(settings)) {}

void TransportDatabase::AddStop(string stop_name, double latitude,
                                double longitude,
                                unordered_map<string, int> distances) {
#if VERSION >= 5
  AddVertex({stop_name, "", false});
#endif
  stop_data[move(stop_name)] = {latitude, longitude, move(distances)};
}

void TransportDatabase::AddBus(string bus_name, vector<string> stop_names,
                               bool is_roundtrip) {
  size_t stops_on_route = stop_names.size();
  if (!is_roundtrip) {
    stops_on_route += stop_names.size() - 1;
  }
  double geo_length = 0.0, road_length = 0.0;
  for (size_t i = 1; i < stop_names.size(); ++i) {
    geo_length += GetShortestDistance(stop_names[i - 1], stop_names[i]);;
#if VERSION >= 3
    road_length += GetRoadDistance(stop_names[i - 1], stop_names[i]);
    if (!is_roundtrip) {
      road_length += GetRoadDistance(stop_names[i], stop_names[i - 1]);
    }
#endif
  }
  if (!is_roundtrip) {
    geo_length *= 2;
  }
#if VERSION < 3
  double route_length = geo_length;
#else
  double route_length = road_length;
#endif

#if VERSION >= 5
  size_t from_id = AddVertex({stop_names[0], bus_name, false}), to_id, start_id = from_id;
  for (size_t i = 1; i < stop_names.size(); ++i) {
    bool is_roundtrip_end = is_roundtrip && i + 1 == stop_names.size();
    to_id = AddVertex({stop_names[i], bus_name, is_roundtrip_end});
    double distance = GetRoadDistance(stop_names[i - 1], stop_names[i]);
    graph.AddEdge({
        from_id,
        to_id,
        distance / (settings.bus_velocity * 1000 / 60)
    });
    if (!is_roundtrip) {
      distance = GetRoadDistance(stop_names[i], stop_names[i - 1]);
      graph.AddEdge({
        to_id,
        from_id,
        distance / (settings.bus_velocity * 1000 / 60)
      });
    } else if (is_roundtrip_end) {
      graph.AddEdge({
        to_id,
        start_id,
        settings.bus_wait_time * 1.0
      });
    }
    from_id = to_id;
  }
#endif

  if (is_roundtrip) {
    stop_data[stop_names.back()].end_buses.insert(bus_name);
  }

  sort(stop_names.begin(), stop_names.end());
  stop_names.erase(unique(stop_names.begin(), stop_names.end()),
                   stop_names.end());
  size_t unique_stops = stop_names.size();
  double curvature = road_length / geo_length;
  for (const string& stop_name : stop_names) {
    stop_data[stop_name].buses.insert(bus_name);
  }
  
  bus_data[move(bus_name)] = {stops_on_route, unique_stops, route_length,
                              curvature};
}

void TransportDatabase::Preprocess() {
#if VERSION >= 5
  for (const auto& [stop_name, data] : stop_data) {
    size_t original_id = vertex_ids[{stop_name, "", false}];
    for (const auto& bus_name : data.buses) {
      size_t bus_stop_id = vertex_ids[{stop_name, bus_name, false}];
      graph.AddEdge({original_id, bus_stop_id, settings.bus_wait_time * 1.0});
      graph.AddEdge({bus_stop_id, original_id, settings.bus_wait_time * 1.0});
      for (const auto& other_bus_name : data.buses) {
        if (bus_name == other_bus_name) continue;
        size_t other_bus_stop_id = vertex_ids[{stop_name, other_bus_name, false}];
        graph.AddEdge({other_bus_stop_id, bus_stop_id, settings.bus_wait_time * 1.0});
        graph.AddEdge({bus_stop_id, other_bus_stop_id, settings.bus_wait_time * 1.0});
      }
      for (const auto& other_bus_name : data.end_buses) {
        if (bus_name == other_bus_name) continue;
        size_t other_bus_stop_id = vertex_ids[{stop_name, other_bus_name, true}];
        graph.AddEdge({other_bus_stop_id, bus_stop_id, settings.bus_wait_time * 1.0});
        graph.AddEdge({bus_stop_id, other_bus_stop_id, settings.bus_wait_time * 1.0});
      }
    }
  }
  router = make_unique<Graph::Router<double>>(graph);
#endif
}

ResponseHolder TransportDatabase::GetBus(const string& bus_name) const {
  auto response = make_unique<GetBusResponse>();
  response->name = bus_name;
  if (auto bus_it = bus_data.find(response->name); bus_it != bus_data.end()) {
    const BusData& data = bus_it->second;
    response->found = true;
    response->stops_on_route = data.stops_on_route;
    response->unique_stops = data.unique_stops;
    response->route_length = data.route_length;
    response->curvature = data.curvature;
  } else {
    response->found = false;
  }
  return response;
}

ResponseHolder TransportDatabase::GetStop(const string& stop_name) const {
  auto response = make_unique<GetStopResponse>();
  response->name = stop_name;
  if (const auto& stop_it = stop_data.find(response->name);
      stop_it != stop_data.end()) {
    const StopData& data = stop_it->second;
    response->found = true;
    response->buses = {data.buses.begin(), data.buses.end()};
  } else {
    response->found = false;
  }
  return response;
}

ResponseHolder TransportDatabase::GetRoute(const string& stop_name_from,
                                           const string& stop_name_to) const {
  Vertex from = {stop_name_from, "", false};
  Vertex to = {stop_name_to, "", false};
  size_t from_id = vertex_ids.at(from);
  size_t to_id = vertex_ids.at(to);
  auto route_info = router->BuildRoute(from_id, to_id);
  auto response = make_unique<GetRouteResponse>();
  if (route_info) {
    response->found = true;
    response->total_time = route_info->weight - settings.bus_wait_time;
    vector<RouteElementHolder>& items = response->items;
    uint64_t id = route_info->id;
    for (size_t i = 0; i + 1 < route_info->edge_count; ++i) {
      const auto& edge = graph.GetEdge(router->GetRouteEdge(id, i));
      const Vertex& from = vertices[edge.from];
      const Vertex& to = vertices[edge.to];
      if (from.bus_name != to.bus_name) {
        RouteElementHolder element = make_unique<WaitRouteElement>(from.stop_name, edge.weight);
        items.push_back(move(element));
      } else if (items.empty() || items.back()->type != RouteElement::Type::BUS) {
        RouteElementHolder element = make_unique<BusRouteElement>(from.bus_name, 1, edge.weight);
        items.push_back(move(element));
      } else {
        auto& element = static_cast<BusRouteElement&>(*items.back());
        if (element.bus_name == from.bus_name) {
          element.span_count += 1;
          element.time += edge.weight;
        } else {
          RouteElementHolder element = make_unique<BusRouteElement>(from.bus_name, 1, edge.weight);
          items.push_back(move(element));
        }
      }
    }
  } else {
    response->found = false;
  }
  return response;
}

bool TransportDatabase::Vertex::operator==(const Vertex& other) const {
  return stop_name == other.stop_name && bus_name == other.bus_name &&
         is_roundtrip_end == other.is_roundtrip_end;
}

size_t TransportDatabase::VertexHasher::operator()(const Vertex& vertex) const {
  return string_hasher(vertex.stop_name) * prime * prime +
         string_hasher(vertex.bus_name) * prime +
         bool_hasher(vertex.is_roundtrip_end);
}

size_t TransportDatabase::AddVertex(Vertex vertex) {
  vertex_ids[vertex] = vertices.size();
  vertices.push_back(move(vertex));
  return graph.AddVertex();
}

double TransportDatabase::GetShortestDistance(const string& stop_name_from,
                                              const string& stop_name_to) {
  const auto& from = stop_data[stop_name_from];
  const auto& to = stop_data[stop_name_to];
  auto to_rad = [](double degrees) { return degrees * 3.1415926535 / 180; };
  return acos(sin(to_rad(from.latitude)) * sin(to_rad(to.latitude)) +
              cos(to_rad(from.latitude)) * cos(to_rad(to.latitude)) *
                  cos(abs(to_rad(from.longitude) - to_rad(to.longitude)))) *
         6371000;
}

double TransportDatabase::GetRoadDistance(const string& stop_name_from,
                                          const string& stop_name_to) {
  const auto& distances_from = stop_data[stop_name_from].distances;
  if (const auto it = distances_from.find(stop_name_to);
      it != distances_from.end()) {
    return it->second;
  } else {
    const auto& distances_to = stop_data[stop_name_to].distances;
    if (const auto it = distances_to.find(stop_name_from);
        it != distances_to.end()) {
      return it->second;
    } else {
      stringstream error;
      error << "Distance between \"" << stop_name_from << "\" and \""
            << stop_name_to << "\" is unknown";
      throw invalid_argument(error.str());
    }
  }
}