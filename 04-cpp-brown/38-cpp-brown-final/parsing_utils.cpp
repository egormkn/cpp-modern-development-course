#include "parsing_utils.h"

#include <sstream>
#include <stdexcept>

using namespace std;

pair<string_view, optional<string_view>> SplitTwoStrict(string_view input,
                                                        string_view delimiter) {
  const size_t pos = input.find(delimiter);
  if (pos == input.npos) {
    return {input, nullopt};
  } else {
    return {input.substr(0, pos), input.substr(pos + delimiter.length())};
  }
}

pair<string_view, string_view> SplitTwo(string_view input,
                                        string_view delimiter) {
  const auto [lhs, rhs_opt] = SplitTwoStrict(input, delimiter);
  return {lhs, rhs_opt.value_or("")};
}

string_view ReadToken(string_view& input, string_view delimiter) {
  const auto [lhs, rhs] = SplitTwo(input, delimiter);
  input = rhs;
  return lhs;
}

int ConvertToInt(string_view str) {
  // use std::from_chars when available to git rid of string copy
  size_t pos;
  const int result = stoi(string(str), &pos);
  if (pos != str.length()) {
    stringstream error;
    error << "string " << str << " contains " << (str.length() - pos)
          << " trailing chars";
    throw invalid_argument(error.str());
  }
  return result;
}

double ConvertToDouble(string_view str) {
  // use std::from_chars when available to git rid of string copy
  size_t pos;
  const double result = stod(string(str), &pos);
  if (pos != str.length()) {
    stringstream error;
    error << "string " << str << " contains " << (str.length() - pos)
          << " trailing chars";
    throw invalid_argument(error.str());
  }
  return result;
}
