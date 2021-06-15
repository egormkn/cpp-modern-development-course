#include <functional>
#include <string>

#include "test_runner.h"

using namespace std;

template <typename T>
class LazyValue {
 public:
  explicit LazyValue(function<T()> init) : init(move(init)) {}

  bool HasValue() const { return value.has_value(); }

  const T& Get() const {
    if (!value.has_value()) {
      value = init();
    }
    return value.value();
  }

 private:
  mutable optional<T> value;
  const function<T()> init;
};

void UseExample() {
  const string big_string = "Giant amounts of memory";

  LazyValue<string> lazy_string([&big_string] { return big_string; });

  ASSERT(!lazy_string.HasValue());
  ASSERT_EQUAL(lazy_string.Get(), big_string);
  ASSERT_EQUAL(lazy_string.Get(), big_string);
}

void TestInitializerIsntCalled() {
  bool called = false;

  {
    LazyValue<int> lazy_int([&called] {
      called = true;
      return 0;
    });
  }
  ASSERT(!called);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, UseExample);
  RUN_TEST(tr, TestInitializerIsntCalled);
  return 0;
}
