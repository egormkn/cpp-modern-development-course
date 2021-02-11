#include <algorithm>
#include <future>
#include <numeric>
#include <vector>

#include "test_runner.h"

using namespace std;

template <typename Iterator>
class Page {
 public:
  Page(Iterator begin, Iterator end, size_t size)
      : page_begin(begin), page_end(end), page_size(size) {}

  Iterator begin() const { return page_begin; }

  Iterator end() const { return page_end; }

  size_t size() const { return page_size; }

 private:
  Iterator page_begin, page_end;
  size_t page_size;
};

template <typename Iterator>
class Paginator {
 public:
  Paginator(Iterator begin, Iterator end, size_t page_size) {
    size_t counter = 0;
    Iterator page_begin = begin, page_end = begin;
    for (Iterator it = begin; it != end; ++it) {
      if (counter + 1 == page_size || next(it) == end) {
        page_end = next(it);
        pages.push_back({page_begin, page_end, counter + 1});
        page_begin = page_end;
      }
      counter = (counter + 1) % page_size;
    }
  }

  auto begin() const { return pages.begin(); }

  auto end() const { return pages.end(); }

  size_t size() const { return pages.size(); }

 private:
  vector<Page<Iterator>> pages;
};

template <typename C>
auto Paginate(C& c, size_t page_size) {
  return Paginator(c.begin(), c.end(), page_size);
}

int64_t CalculateMatrixSum(const vector<vector<int>>& matrix) {
  vector<future<int64_t>> futures;
  futures.reserve(4);
  for (auto page : Paginate(matrix, matrix.size() / 4)) {
    futures.push_back(async([page] {
      int64_t sum = 0;
      for (const auto& row : page) {
        sum = accumulate(row.begin(), row.end(), sum);
      }
      return sum;
    }));
  }
  int64_t result = 0;
  for (auto& f : futures) {
    result += f.get();
  }
  return result;
}

void TestCalculateMatrixSum() {
  const vector<vector<int>> matrix = {
      {1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};
  ASSERT_EQUAL(CalculateMatrixSum(matrix), 136);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestCalculateMatrixSum);
}
