#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>

#include "test_runner.h"

using namespace std;

template <typename T>
class PriorityCollection {
 public:
  using Id = pair<int, typename set<pair<int, int>>::iterator>;

  // Добавить объект с нулевым приоритетом
  // с помощью перемещения и вернуть его идентификатор
  Id Add(T object) {
    objects[next_id] = move(object);
    auto it = priorities.emplace(0, next_id).first;
    return {next_id++, it};
  }

  // Добавить все элементы диапазона [range_begin, range_end)
  // с помощью перемещения, записав выданные им идентификаторы
  // в диапазон [ids_begin, ...)
  template <typename ObjInputIt, typename IdOutputIt>
  void Add(ObjInputIt range_begin, ObjInputIt range_end, IdOutputIt ids_begin) {
    while (range_begin != range_end) {
      *ids_begin++ = Add(move(*range_begin++));
    }
  }

  // Определить, принадлежит ли идентификатор какому-либо
  // хранящемуся в контейнере объекту
  bool IsValid(Id id) const { return objects.count(id.first); }

  // Получить объект по идентификатору
  const T& Get(Id id) const { return objects.at(id.first); }

  // Увеличить приоритет объекта на 1
  void Promote(Id id) {
    auto node = priorities.extract(id.second);
    ++node.value().first;
    priorities.insert(move(node));
  }

  // Получить объект с максимальным приоритетом и его приоритет
  pair<const T&, int> GetMax() const {
    auto it = prev(priorities.end());
    auto [priority, id] = *it;
    return {objects.at(id), priority};
  }

  // Аналогично GetMax, но удаляет элемент из контейнера
  pair<T, int> PopMax() {
    auto it = prev(priorities.end());
    auto [priority, id] = *it;
    T object = move(objects.at(id));
    objects.erase(id);
    priorities.erase(it);
    return {move(object), priority};
  }

 private:
  set<pair<int, int>> priorities;
  map<int, T> objects;
  int next_id = 0;
};

class StringNonCopyable : public string {
 public:
  using string::string;  // Позволяет использовать конструкторы строки
  StringNonCopyable(const StringNonCopyable&) = delete;
  StringNonCopyable(StringNonCopyable&&) = default;
  StringNonCopyable& operator=(const StringNonCopyable&) = delete;
  StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
  PriorityCollection<StringNonCopyable> strings;
  const auto white_id = strings.Add("white");
  const auto yellow_id = strings.Add("yellow");
  const auto red_id = strings.Add("red");

  (void)white_id;

  strings.Promote(yellow_id);
  for (int i = 0; i < 2; ++i) {
    strings.Promote(red_id);
  }
  strings.Promote(yellow_id);
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "red");
    ASSERT_EQUAL(item.second, 2);
  }
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "yellow");
    ASSERT_EQUAL(item.second, 2);
  }
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "white");
    ASSERT_EQUAL(item.second, 0);
  }
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestNoCopy);
  return 0;
}
