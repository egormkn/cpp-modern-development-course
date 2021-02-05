#include <string>
#include <string_view>
#include <vector>

#include "test_runner.h"

using namespace std;

class Translator {
 public:
  void Add(string_view source, string_view target) {
    auto source_it = dict.insert(string(source)).first;
    auto target_it = dict.insert(string(target)).first;
    source_to_target[*source_it] = *target_it;
    target_to_source[*target_it] = *source_it;
  }

  string_view TranslateForward(string_view source) const {
    return source_to_target.count(source) ? source_to_target.at(source) : "";
  }

  string_view TranslateBackward(string_view target) const {
    return target_to_source.count(target) ? target_to_source.at(target) : "";
  }

 private:
  set<string> dict;
  map<string_view, string_view> source_to_target;
  map<string_view, string_view> target_to_source;
};

void TestSimple() {
  Translator translator;
  translator.Add(string("okno"), string("window"));
  translator.Add(string("stol"), string("table"));

  ASSERT_EQUAL(translator.TranslateForward("okno"), "window");
  ASSERT_EQUAL(translator.TranslateBackward("table"), "stol");
  ASSERT_EQUAL(translator.TranslateBackward("stol"), "");
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSimple);
  return 0;
}
