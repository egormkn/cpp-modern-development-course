#include <algorithm>
#include <memory>
#include <vector>

#include "test_runner.h"

using namespace std;

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
  const size_t size = distance(range_begin, range_end);
  if (size < 2) return;
  const size_t one_third = size / 3;
  vector<typename RandomIt::value_type> elements(
      make_move_iterator(range_begin), make_move_iterator(range_end));
  MergeSort(elements.begin(), elements.begin() + one_third);
  MergeSort(elements.begin() + one_third, elements.end() - one_third);
  MergeSort(elements.end() - one_third, elements.end());
  vector<typename RandomIt::value_type> v;
  v.reserve(size - one_third);
  merge(make_move_iterator(elements.begin()),
        make_move_iterator(elements.begin() + one_third),
        make_move_iterator(elements.begin() + one_third),
        make_move_iterator(elements.end() - one_third), back_inserter(v));
  merge(make_move_iterator(v.begin()), make_move_iterator(v.end()),
        make_move_iterator(elements.end() - one_third),
        make_move_iterator(elements.end()), range_begin);
}

struct NoncopyableInt {
  int value;

  NoncopyableInt(int value) : value(value) {}

  NoncopyableInt(const NoncopyableInt&) = delete;
  NoncopyableInt& operator=(const NoncopyableInt&) = delete;

  NoncopyableInt(NoncopyableInt&&) = default;
  NoncopyableInt& operator=(NoncopyableInt&&) = default;
};

bool operator==(const NoncopyableInt& lhs, const NoncopyableInt& rhs) {
  return lhs.value == rhs.value;
}

bool operator<(const NoncopyableInt& lhs, const NoncopyableInt& rhs) {
  return lhs.value < rhs.value;
}

ostream& operator<<(ostream& os, const NoncopyableInt& v) {
  return os << v.value;
}

void TestNonCopyableIntVector() {
  vector<NoncopyableInt> numbers;
  numbers.push_back({6});
  numbers.push_back({1});
  numbers.push_back({3});
  numbers.push_back({9});
  numbers.push_back({1});
  numbers.push_back({9});
  numbers.push_back({8});
  numbers.push_back({12});
  numbers.push_back({1});
  MergeSort(begin(numbers), end(numbers));
  ASSERT(is_sorted(begin(numbers), end(numbers)));
}

void TestIntVector() {
  vector<int> numbers = {6, 1, 3, 9, 1, 9, 8, 12, 1};
  MergeSort(begin(numbers), end(numbers));
  ASSERT(is_sorted(begin(numbers), end(numbers)));
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestIntVector);
  RUN_TEST(tr, TestNonCopyableIntVector);
  return 0;
}
