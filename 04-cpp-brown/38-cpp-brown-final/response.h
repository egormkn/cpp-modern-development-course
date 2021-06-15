#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "config.h"
#include "database.h"
#include "range.h"

struct Response {
  int request_id = 0;

  virtual void Print(std::ostream& stream = std::cout) const = 0;

  virtual void PrintJson(std::ostream& stream = std::cout) const = 0;

  virtual ~Response() = default;
};

std::ostream& operator<<(std::ostream& stream, const Response& response);

using ResponseHolder = std::unique_ptr<Response>;

struct GetBusResponse : Response {
  std::string name;
  bool found;
  size_t stops_on_route;
  size_t unique_stops;
  double route_length;
  double curvature;

  void Print(std::ostream& stream = std::cout) const override;

  void PrintJson(std::ostream& stream = std::cout) const override;
};

struct GetStopResponse : Response {
  std::string name;
  bool found;
  std::vector<std::string> buses;

  void Print(std::ostream& stream = std::cout) const override;
  
  void PrintJson(std::ostream& stream = std::cout) const override;
};

struct RouteElement;
using RouteElementHolder = std::unique_ptr<RouteElement>;

struct GetRouteResponse : Response {
  double total_time;
  bool found;
  std::vector<RouteElementHolder> items;

  void Print(std::ostream& stream = std::cout) const override;
  
  void PrintJson(std::ostream& stream = std::cout) const override;
};

struct RouteElement {
  enum class Type { BUS, WAIT };

  RouteElement(Type type);

  virtual void PrintJson(std::ostream& stream) const = 0;

  virtual ~RouteElement();

  const Type type;
};

struct BusRouteElement : RouteElement {
  BusRouteElement(std::string bus_name, int span_count, double time);

  void PrintJson(std::ostream& stream) const override;

  std::string bus_name;
  int span_count;
  double time;
};

struct WaitRouteElement : RouteElement {
  WaitRouteElement(std::string stop_name, double time);

  void PrintJson(std::ostream& stream) const override;

  std::string stop_name;
  double time;
};
