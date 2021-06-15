#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
#include <stdexcept>
#include <string>

#include "test_runner.h"
using namespace std;

template <class T>
class ObjectPool {
 public:
  T* Allocate() {
    if (free.empty()) {
      free.push(new T);
    }
    T* object = free.front();
    free.pop();
    used.insert(object);
    return object;
  }

  T* TryAllocate() {
    if (free.empty()) {
      return nullptr;
    }
    return Allocate();
  }

  void Deallocate(T* object) {
    if (!used.count(object)) {
      throw invalid_argument("Object was not allocated");
    }
    used.erase(object);
    free.push(object);
  }

  ~ObjectPool() {
    for (T* object : used) {
      delete object;
    }
    while (!free.empty()) {
      T* object = free.front();
      free.pop();
      delete object;
    }
  }

 private:
  set<T*> used;
  queue<T*> free;
};

void TestObjectPool() {
  ObjectPool<string> pool;

  auto p1 = pool.Allocate();
  auto p2 = pool.Allocate();
  auto p3 = pool.Allocate();

  *p1 = "first";
  *p2 = "second";
  *p3 = "third";

  pool.Deallocate(p2);
  ASSERT_EQUAL(*pool.Allocate(), "second");

  pool.Deallocate(p3);
  pool.Deallocate(p1);
  ASSERT_EQUAL(*pool.Allocate(), "third");
  ASSERT_EQUAL(*pool.Allocate(), "first");

  pool.Deallocate(p1);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestObjectPool);
  return 0;
}
