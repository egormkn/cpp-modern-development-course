#include <vector>
#include <stdexcept>

using namespace std;

template <typename T>
class Deque {
 public:
  Deque() {}

  bool Empty() const {
    return front.empty() && back.empty();
  }

  size_t Size() const {
    return front.size() + back.size();
  }

  T& operator[](size_t index) {
    return index < front.size() ? front[front.size() - index - 1] : back[index - front.size()];
  }
  
  const T& operator[](size_t index) const {
    return index < front.size() ? front[front.size() - index - 1] : back[index - front.size()];
  }

  T& At(size_t index) {
    CheckIndex(index);
    return operator[](index);
  }

  const T& At(size_t index) const {
    CheckIndex(index);
    return operator[](index);
  }

  T& Front() { return operator[](0); }

  const T& Front() const { return operator[](0); }

  T& Back() { return operator[](Size() - 1); }

  const T& Back() const { return operator[](Size() - 1); }

  void PushFront(const T& element) { front.push_back(element); }

  void PushBack(const T& element) { back.push_back(element); }

 private:
  void CheckIndex(size_t index) {
    if (index >= Size()) {
      throw out_of_range("No such element in deque");
    }
  }

  vector<T> front, back;
};

