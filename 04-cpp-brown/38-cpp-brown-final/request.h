#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "config.h"
#include "database.h"
#include "json.h"

struct Request {
  enum class Type { ADD_BUS, ADD_STOP, GET_BUS, GET_STOP, GET_ROUTE };

  Request(Type type);

  static std::unique_ptr<Request> Create(Type type);

  virtual void ParseFrom(std::string_view input) = 0;

  virtual void ParseFromJson(const std::map<std::string, Json::Node>& nodes) = 0;

  virtual void Print(std::ostream& output = std::cout) const = 0;

  virtual ~Request() = default;

  const Type type;
  int id;
};

std::ostream& operator<<(std::ostream& output, const Request& request);

using RequestHolder = std::unique_ptr<Request>;

template <typename ResultType, typename ObjectType>
struct ReadRequest : Request {
  using Request::Request;
  virtual ResultType Process(const ObjectType& object) const = 0;
};

template <typename ResultType, typename ObjectType>
struct ReadWriteRequest : Request {
  using Request::Request;
  virtual ResultType Process(ObjectType& object) = 0;
};

struct AddBusRequest : ReadWriteRequest<void, TransportDatabase> {
  AddBusRequest();

  void ParseFrom(std::string_view input) override;

  void ParseFromJson(const std::map<std::string, Json::Node>& nodes) override;

  void Print(std::ostream& output) const override;

  void Process(TransportDatabase& manager) override;

  std::string name;
  std::vector<std::string> stops;
  bool is_roundtrip;
};

struct AddStopRequest : ReadWriteRequest<void, TransportDatabase> {
  AddStopRequest();

  void ParseFrom(std::string_view input) override;

  void ParseFromJson(const std::map<std::string, Json::Node>& nodes) override;

  void Print(std::ostream& output) const override;

  void Process(TransportDatabase& manager) override;

  std::string name;
  double latitude, longitude;
  std::unordered_map<std::string, int> road_distances;
};

struct Response;

using ResponseHolder = std::unique_ptr<Response>;

struct GetBusRequest : ReadRequest<ResponseHolder, TransportDatabase> {
  GetBusRequest();

  void ParseFrom(std::string_view input) override;

  void ParseFromJson(const std::map<std::string, Json::Node>& nodes) override;

  void Print(std::ostream& output) const override;

  ResponseHolder Process(const TransportDatabase& manager) const override;

  std::string name;
};

struct GetStopRequest : ReadRequest<ResponseHolder, TransportDatabase> {
  GetStopRequest();

  void ParseFrom(std::string_view input) override;

  void ParseFromJson(const std::map<std::string, Json::Node>& nodes) override;

  void Print(std::ostream& output) const override;

  ResponseHolder Process(const TransportDatabase& manager) const override;

  std::string name;
};

struct GetRouteRequest : ReadRequest<ResponseHolder, TransportDatabase> {
  GetRouteRequest();

  void ParseFrom(std::string_view input) override;

  void ParseFromJson(const std::map<std::string, Json::Node>& nodes) override;

  void Print(std::ostream& output) const override;

  ResponseHolder Process(const TransportDatabase& manager) const override;

  std::string name_from;
  std::string name_to;
};
