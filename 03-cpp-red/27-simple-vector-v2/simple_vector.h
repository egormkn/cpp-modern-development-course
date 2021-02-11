#pragma once

#include <algorithm>
#include <cstdlib>

template <typename T>
class SimpleVector {
 public:
  SimpleVector() : size(0u), capacity(0u), data(nullptr) {}

  explicit SimpleVector(size_t size)
      : size(size), capacity(size), data(new T[size]) {}

  SimpleVector(const SimpleVector<T>& other)
      : size(other.size), capacity(other.capacity), data(new T[other.capacity]) {
    std::copy(other.begin(), other.end(), begin());
  }

  void operator=(const SimpleVector<T>& other) {
    if (other.size <= capacity) {
      std::copy(other.begin(), other.end(), begin());
      size = other.size;
    } else {
      SimpleVector<T> temp(other);
      std::swap(temp.data, data);
      std::swap(temp.size, size);
      std::swap(temp.capacity, capacity);
    }
  }

  ~SimpleVector() { delete[] data; }

  T& operator[](size_t index) { return data[index]; }

  T* begin() { return data; }

  T* end() { return data + size; }

  const T* begin() const { return data; }

  const T* end() const { return data + size; }

  size_t Size() const { return size; }

  size_t Capacity() const { return capacity; }

  void PushBack(const T& value) {
    if (size >= capacity) {
      auto new_capacity = capacity == 0 ? 1 : 2 * capacity;
      auto new_data = new T[new_capacity];
      std::copy(begin(), end(), new_data);
      delete[] data;
      data = new_data;
      capacity = new_capacity;
    }
    data[size++] = value;
  }

 private:
  size_t size, capacity;
  T* data;
};
