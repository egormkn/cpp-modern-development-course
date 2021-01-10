#include <vector>

using namespace std;

void Swap(int& a, int& b) { 
  int tmp = a;
  a = b;
  b = tmp;
}

void Reverse(vector<int>& v) { 
  for (int i = 0; i < v.size() / 2; ++i) {
    Swap(v[i], v[v.size() - i - 1]);
  }
}
