#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "test_runner.h"

using namespace std;

struct HttpRequest {
  string method, path, body;
  map<string, string> get_params;
};

enum class HttpCode {
  Ok = 200,
  NotFound = 404,
  Found = 302,
};

ostream& operator<<(ostream& output, const HttpCode& code) {
  output << static_cast<underlying_type_t<HttpCode>>(code) << " ";
  switch (code) {
    case HttpCode::Ok:
      return output << "Ok";
    case HttpCode::NotFound:
      return output << "Not found";
    case HttpCode::Found:
      return output << "Found";
  }
}

class HttpResponse {
 public:
  explicit HttpResponse(HttpCode code) : code(code) {}

  HttpResponse& AddHeader(string name, string value) {
    headers.emplace(move(name), move(value));
    return *this;
  }

  HttpResponse& SetContent(string a_content) {
    content = move(a_content);
    if (content.empty()) {
      headers.erase("Content-Length");
    } else {
      headers.emplace("Content-Length", to_string(content.size()));
    }
    return *this;
  }

  HttpResponse& SetCode(HttpCode a_code) {
    code = a_code;
    return *this;
  }

  friend ostream& operator<<(ostream& output, const HttpResponse& resp) {
    output << "HTTP/1.1 " << resp.code << '\n';
    for (const auto& [header, value] : resp.headers) {
      output << header << ": " << value << '\n';
    }
    return output << '\n' << resp.content;
  }

 private:
  HttpCode code;
  map<string, string> headers;
  string content;
};

pair<string, string> SplitBy(const string& what, const string& by) {
  size_t pos = what.find(by);
  if (by.size() < what.size() && pos < what.size() - by.size()) {
    return {what.substr(0, pos), what.substr(pos + by.size())};
  } else {
    return {what, {}};
  }
}

template <typename T>
T FromString(const string& s) {
  T x;
  istringstream is(s);
  is >> x;
  return x;
}

pair<size_t, string> ParseIdAndContent(const string& body) {
  auto [id_string, content] = SplitBy(body, " ");
  return {FromString<size_t>(id_string), content};
}

struct LastCommentInfo {
  size_t user_id, consecutive_count;
};

class CommentServer {
 private:
  vector<vector<string>> comments_;
  optional<LastCommentInfo> last_comment;
  unordered_set<size_t> banned_users;

  HttpResponse AddUser() {
    comments_.emplace_back();
    size_t user_id = comments_.size() - 1;
    return HttpResponse(HttpCode::Ok).SetContent(to_string(user_id));
  }

  HttpResponse AddComment(size_t user_id, string comment) {
    if (!last_comment || last_comment->user_id != user_id) {
      last_comment = LastCommentInfo{user_id, 1};
    } else if (++last_comment->consecutive_count > 3) {
      banned_users.insert(user_id);
    }
    if (banned_users.count(user_id)) {
      return HttpResponse(HttpCode::Found).AddHeader("Location", "/captcha");
    }
    comments_[user_id].push_back(move(comment));
    return HttpResponse(HttpCode::Ok);
  }

  HttpResponse GetUserComments(size_t user_id) {
    stringstream response;
    for (const string& c : comments_[user_id]) {
      response << c << '\n';
    }
    return HttpResponse(HttpCode::Ok).SetContent(response.str());
  }

  HttpResponse GetCaptcha() {
    return HttpResponse(HttpCode::Ok).SetContent(
        "What's the answer for The Ultimate Question of Life, the "
        "Universe, and Everything?");
  }

  HttpResponse CheckCaptcha(size_t user_id, const string& response) {
    if (response == "42") {
      banned_users.erase(user_id);
      if (last_comment && last_comment->user_id == user_id) {
        last_comment.reset();
      }
      return HttpResponse(HttpCode::Ok);
    } else {
      return HttpResponse(HttpCode::Found).AddHeader("Location", "/captcha");
    }
  }

 public:
  HttpResponse ServeRequest(const HttpRequest& req) {
    if (req.method == "POST") {
      if (req.path == "/add_user") {
        return AddUser();
      } else if (req.path == "/add_comment") {
        auto [user_id, comment] = ParseIdAndContent(req.body);
        return AddComment(user_id, comment);
      } else if (req.path == "/checkcaptcha") {
        auto [user_id, response] = ParseIdAndContent(req.body);
        return CheckCaptcha(user_id, response);
      }
    } else if (req.method == "GET") {
      if (req.path == "/user_comments") {
        auto user_id = FromString<size_t>(req.get_params.at("user_id"));
        return GetUserComments(user_id);
      } else if (req.path == "/captcha") {
        return GetCaptcha();
      }
    }
    return HttpResponse(HttpCode::NotFound);
  }
};

struct HttpHeader {
  string name, value;
};

ostream& operator<<(ostream& output, const HttpHeader& h) {
  return output << h.name << ": " << h.value;
}

bool operator==(const HttpHeader& lhs, const HttpHeader& rhs) {
  return lhs.name == rhs.name && lhs.value == rhs.value;
}

struct ParsedResponse {
  int code;
  vector<HttpHeader> headers;
  string content;
};

istream& operator>>(istream& input, ParsedResponse& r) {
  string line;
  getline(input, line);

  {
    istringstream code_input(line);
    string dummy;
    code_input >> dummy >> r.code;
  }

  size_t content_length = 0;

  r.headers.clear();
  while (getline(input, line) && !line.empty()) {
    if (auto [name, value] = SplitBy(line, ": "); name == "Content-Length") {
      istringstream length_input(value);
      length_input >> content_length;
    } else {
      r.headers.push_back({std::move(name), std::move(value)});
    }
  }

  r.content.resize(content_length);
  input.read(r.content.data(), r.content.size());
  return input;
}

void Test(CommentServer& srv, const HttpRequest& request,
          const ParsedResponse& expected) {
  stringstream ss;
  ss << srv.ServeRequest(request);
  ParsedResponse resp;
  ss >> resp;
  ASSERT_EQUAL(resp.code, expected.code);
  ASSERT_EQUAL(resp.headers, expected.headers);
  ASSERT_EQUAL(resp.content, expected.content);
}

template <typename CommentServer>
void TestServer() {
  CommentServer cs;

  const ParsedResponse ok{200};
  const ParsedResponse redirect_to_captcha{302, {{"Location", "/captcha"}}, {}};
  const ParsedResponse not_found{404};

  Test(cs, {"POST", "/add_user"}, {200, {}, "0"});
  Test(cs, {"POST", "/add_user"}, {200, {}, "1"});
  Test(cs, {"POST", "/add_comment", "0 Hello"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Hi"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Buy my goods"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Enlarge"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Buy my goods"}, redirect_to_captcha);
  Test(cs, {"POST", "/add_comment", "0 What are you selling?"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Buy my goods"}, redirect_to_captcha);
  Test(cs, {"GET", "/user_comments", "", {{"user_id", "0"}}},
       {200, {}, "Hello\nWhat are you selling?\n"});
  Test(cs, {"GET", "/user_comments", "", {{"user_id", "1"}}},
       {200, {}, "Hi\nBuy my goods\nEnlarge\n"});
  Test(cs, {"GET", "/captcha"},
       {200,
        {},
        {"What's the answer for The Ultimate Question of Life, the Universe, "
         "and Everything?"}});
  Test(cs, {"POST", "/checkcaptcha", "1 24"}, redirect_to_captcha);
  Test(cs, {"POST", "/checkcaptcha", "1 42"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Sorry! No spam any more"}, ok);
  Test(cs, {"GET", "/user_comments", "", {{"user_id", "1"}}},
       {200, {}, "Hi\nBuy my goods\nEnlarge\nSorry! No spam any more\n"});

  Test(cs, {"GET", "/user_commntes"}, not_found);
  Test(cs, {"POST", "/add_uesr"}, not_found);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestServer<CommentServer>);
}
