#include <cstddef>  // нужно для nullptr_t

#include "test_runner.h"

using namespace std;

template <typename T>
class UniquePtr {
 private:
  T* ptr = nullptr;

 public:
  UniquePtr() {}
  
  UniquePtr(T* ptr) : ptr(ptr) {}

  UniquePtr(const UniquePtr&) = delete;

  UniquePtr(UniquePtr&& other) = default;

  UniquePtr& operator=(const UniquePtr&) = delete;

  UniquePtr& operator=(nullptr_t) {
    if (ptr) {
      delete ptr;
    }
    ptr = nullptr;
  }

  UniquePtr& operator=(UniquePtr&& other) = default;

  ~UniquePtr() {
    if (ptr) {
      delete ptr;
    }
  }

  T& operator*() const { return *ptr; }

  T* operator->() const { return ptr; }

  T* Release() { 
    T* result = ptr;
    ptr = nullptr;
    return result;
  }

  void Reset(T* new_ptr) {
    T* old_ptr = ptr;
    ptr = new_ptr;
    if (old_ptr) {
      delete old_ptr;
    }
  }

  void Swap(UniquePtr& other) {
    swap(ptr, other.ptr);
  }

  T* Get() const { return ptr; }
};

struct Item {
  static int counter;
  int value;
  Item(int v = 0) : value(v) { ++counter; }
  Item(const Item& other) : value(other.value) { ++counter; }
  ~Item() { --counter; }
};

int Item::counter = 0;

void TestLifetime() {
  Item::counter = 0;
  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    ptr.Reset(new Item);
    ASSERT_EQUAL(Item::counter, 1);
  }
  ASSERT_EQUAL(Item::counter, 0);

  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    auto rawPtr = ptr.Release();
    ASSERT_EQUAL(Item::counter, 1);

    delete rawPtr;
    ASSERT_EQUAL(Item::counter, 0);
  }
  ASSERT_EQUAL(Item::counter, 0);
}

void TestGetters() {
  UniquePtr<Item> ptr(new Item(42));
  ASSERT_EQUAL(ptr.Get()->value, 42);
  ASSERT_EQUAL((*ptr).value, 42);
  ASSERT_EQUAL(ptr->value, 42);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestLifetime);
  RUN_TEST(tr, TestGetters);
}
