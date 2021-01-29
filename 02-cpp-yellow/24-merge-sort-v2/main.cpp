#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
  if (range_end - range_begin < 2) return;
  vector<typename RandomIt::value_type> elements(range_begin, range_end);
  size_t one_third = elements.size() / 3;
  MergeSort(elements.begin(), elements.begin() + one_third);
  MergeSort(elements.begin() + one_third, elements.end() - one_third);
  MergeSort(elements.end() - one_third, elements.end());
  vector<typename RandomIt::value_type> v;
  merge(elements.begin(), elements.begin() + one_third,
        elements.begin() + one_third, elements.end() - one_third,
        back_inserter(v));
  merge(v.begin(), v.end(), elements.end() - one_third, elements.end(),
        range_begin);
}

int main() {
  vector<int> v = {6, 4, 7, 6, 4, 4, 0, 1, 5};
  MergeSort(begin(v), end(v));
  for (int x : v) {
    cout << x << " ";
  }
  cout << endl;
  return 0;
}
