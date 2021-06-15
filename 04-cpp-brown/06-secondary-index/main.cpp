#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

#include "test_runner.h"

using namespace std;

/*
Требования:
  - Операция Put должна возвращать true, если вставка удалась, и false, если
вставка не удалась, потому что в базе данных уже есть запись с таким же id. В
последнем случае состояние базы данных не должно меняться.
  - Операция GetById должна возвращать nullptr, если в базе данных нет записи с
указанным id.
  - Операция Erase должна возвращать true, если удалось удалить элемент с
заданным id, и false, если элемент с заданным id не был найден. В последнем
случае состояние базы данных не должно меняться.
  - Операции RangeByTimestamp, RangeByKarma и AllByUser принимают функтор
callback. Подразумевается, что он имеет оператор (), принимающий в качестве
аргумента объект типа Record и возвращающий bool.
  - Операция RangeByTimestamp должна обходить все записи, у которых поле
timestamp находится в диапазоне [low, high] (обе границы включены), и для каждой
из них один раз вызывать callback. Если очередной вызов callback вернул false,
итерация прекращается.
  - Операция RangeByKarma должна обходить все записи, у которых поле karma
находится в диапазоне [low, high].
  - Операция AllByUser должна обходить все записи в хранилище данных, у которых
поле user равно значению параметра user.
  - Так же, как и RangeByTimestamp, методы RangeByKarma и AllByUser должны
вызывать callback для каждой записи, по которой они итерируются, и прекращать
итерацию, если callback вернул false.
  - Параметр callback позволяет управлять процессом обхода: он должен возвращать
true, если требуется продолжить обход найденных записей, и false в противном
случае. Например, это позволит вывести первые 10 записей или найти первую
запись, удовлетворяющую дополнительному критерию.
  - Время выполнения всех операций должно иметь в среднем сублинейную (например,
константную или логарифмическую) зависимость от общего числа записей в базе
данных.
  - Все границы интервалов - включительные, [low, high] — это наиболее
универсальный интервальный запрос, подумайте, как с помощью него выразить
условия вида x < high, x > low, x = x_0.
  - При решении этой задачи вы можете считать, что bad_alloc кидаться не будет.
*/

struct Record {
  string id;
  string title;
  string user;
  int timestamp;
  int karma;
};

// Реализуйте этот класс
class Database {
 public:
  bool Put(const Record& record) {
    IndexedRecord ir;
    ir.record = record;
    auto [it, success] = data.insert({record.id, move(ir)});
    if (success) {
      IndexedRecord& ir = it->second;
      ir.timestamp_it = timestamp_index.insert({ir.record.timestamp, &ir.record});
      ir.karma_it = karma_index.insert({ir.record.karma, &ir.record});
      ir.user_it = user_index.insert({ir.record.user, &ir.record});
    }
    return success;
  }

  const Record* GetById(const string& id) const {
    if (auto it = data.find(id); it != data.end()) {
      return &it->second.record;
    } else {
      return nullptr;
    }
  }

  bool Erase(const string& id) {
    if (auto it = data.find(id); it != data.end()) {
      IndexedRecord& ir = it->second;
      timestamp_index.erase(ir.timestamp_it);
      karma_index.erase(ir.karma_it);
      user_index.erase(ir.user_it);
      data.erase(it);
      return true;
    } else {
      return false;
    }
  }

  template <typename Callback>
  void RangeByTimestamp(int low, int high, Callback callback) const {
    auto first = timestamp_index.lower_bound(low);
    auto last = timestamp_index.upper_bound(high);
    while (first != last) {
      if (!callback(*first->second)) break;
      ++first;
    }
  }

  template <typename Callback>
  void RangeByKarma(int low, int high, Callback callback) const {
    auto first = karma_index.lower_bound(low);
    auto last = karma_index.upper_bound(high);
    while (first != last) {
      if (!callback(*first->second)) break;
      ++first;
    }
  }

  template <typename Callback>
  void AllByUser(const string& user, Callback callback) const {
    auto [first, last] = user_index.equal_range(user);
    while (first != last) {
      if (!callback(*first->second)) break;
      ++first;
    }
  }

 private:
  struct IndexedRecord {
    Record record;
    multimap<int, Record*>::iterator timestamp_it, karma_it;
    multimap<string, Record*>::iterator user_it;
  };

  unordered_map<string, IndexedRecord> data;
  multimap<int, Record*> timestamp_index, karma_index;
  multimap<string, Record*> user_index;
};

void TestRangeBoundaries() {
  const int good_karma = 1000;
  const int bad_karma = -10;

  Database db;
  db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
  db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

  int count = 0;
  db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(2, count);
}

void TestSameUser() {
  Database db;
  db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
  db.Put({"id2", "Rethink life", "master", 1536107260, 2000});

  int count = 0;
  db.AllByUser("master", [&count](const Record&) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(2, count);
}

void TestReplacement() {
  const string final_body = "Feeling sad";

  Database db;
  db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
  db.Erase("id");
  db.Put({"id", final_body, "not-master", 1536107260, -10});

  auto record = db.GetById("id");
  ASSERT(record != nullptr);
  ASSERT_EQUAL(final_body, record->title);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestRangeBoundaries);
  RUN_TEST(tr, TestSameUser);
  RUN_TEST(tr, TestReplacement);
  return 0;
}
