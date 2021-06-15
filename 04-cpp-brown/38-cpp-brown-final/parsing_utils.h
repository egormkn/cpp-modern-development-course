#pragma once

#include <istream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

std::pair<std::string_view, std::optional<std::string_view>> SplitTwoStrict(
    std::string_view input, std::string_view delimiter = " ");

std::pair<std::string_view, std::string_view> SplitTwo(
    std::string_view input, std::string_view delimiter = " ");

std::string_view ReadToken(std::string_view& input, std::string_view delimiter = " ");

int ConvertToInt(std::string_view str);

double ConvertToDouble(std::string_view str);

template <typename Number>
Number ReadNumberOnLine(std::istream& stream) {
  Number number;
  stream >> number;
  std::string dummy;
  getline(stream, dummy);
  return number;
}