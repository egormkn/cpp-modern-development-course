#pragma once

#include <iostream>

namespace RAII {

template <typename Provider>
class Booking {
 public:
  Booking(Provider* p, int counter) :p(p), counter(counter) {}

  Booking(const Booking&) = delete;
  Booking& operator=(const Booking&) = delete;

  Booking(Booking&& other) { 
    p = other.p;
    counter = other.counter;
    other.p = nullptr;
  }

  Booking& operator=(Booking&& other) {
    if (this != &other) {
      p = other.p;
      counter = other.counter;
      other.p = nullptr;
    }
  }

  ~Booking() {
    if (p) {
      p->CancelOrComplete(*this);
    }
  }

 private:
  Provider* p;
  int counter;
};

}  // namespace RAII