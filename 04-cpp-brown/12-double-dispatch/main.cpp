#include <memory>
#include <vector>

#include "game_object.h"
#include "geo2d.h"
#include "test_runner.h"

using namespace std;

// Определите классы Unit, Building, Tower и Fence так, чтобы они наследовались
// от GameObject и реализовывали его интерфейс.

template <typename T>
struct Collider : public GameObject {
  bool Collide(const GameObject& that) const final override {
    // Статически приводим тип *this к типу const T&,
    // потому что мы знаем, что T — наш наследник
    return that.CollideWith(static_cast<const T&>(*this));
  }
};

class Unit : public Collider<Unit> {
 public:
  explicit Unit(geo2d::Point geometry) : geometry(geometry) {}

  bool CollideWith(const Unit& that) const override;

  bool CollideWith(const Building& that) const override;

  bool CollideWith(const Tower& that) const override;

  bool CollideWith(const Fence& that) const override;

  geo2d::Point geometry;
};

class Building : public Collider<Building> {
 public:
  explicit Building(geo2d::Rectangle geometry) : geometry(geometry) {}

  bool CollideWith(const Unit& that) const override;

  bool CollideWith(const Building& that) const override;

  bool CollideWith(const Tower& that) const override;

  bool CollideWith(const Fence& that) const override;

  geo2d::Rectangle geometry;
};

class Tower : public Collider<Tower> {
 public:
  explicit Tower(geo2d::Circle geometry) : geometry(geometry) {}

  bool CollideWith(const Unit& that) const override;

  bool CollideWith(const Building& that) const override;

  bool CollideWith(const Tower& that) const override;

  bool CollideWith(const Fence& that) const override;

  geo2d::Circle geometry;
};

class Fence : public Collider<Fence> {
 public:
  explicit Fence(geo2d::Segment geometry) : geometry(geometry) {}

  bool CollideWith(const Unit& that) const override;

  bool CollideWith(const Building& that) const override;

  bool CollideWith(const Tower& that) const override;

  bool CollideWith(const Fence& that) const override;

  geo2d::Segment geometry;
};

#define COLLIDE_IMPL(type)                             \
  bool type::CollideWith(const Unit& that) const {     \
    return geo2d::Collide(geometry, that.geometry);    \
  }                                                    \
                                                       \
  bool type::CollideWith(const Building& that) const { \
    return geo2d::Collide(geometry, that.geometry);    \
  }                                                    \
                                                       \
  bool type::CollideWith(const Tower& that) const {    \
    return geo2d::Collide(geometry, that.geometry);    \
  }                                                    \
                                                       \
  bool type::CollideWith(const Fence& that) const {    \
    return geo2d::Collide(geometry, that.geometry);    \
  }

COLLIDE_IMPL(Unit)
COLLIDE_IMPL(Building)
COLLIDE_IMPL(Tower)
COLLIDE_IMPL(Fence)

// Реализуйте функцию Collide из файла GameObject.h

bool Collide(const GameObject& first, const GameObject& second) {
  return first.Collide(second);
}

void TestAddingNewObjectOnMap() {
  // Юнит-тест моделирует ситуацию, когда на игровой карте уже есть какие-то
  // объекты, и мы хотим добавить на неё новый, например, построить новое здание
  // или башню. Мы можем его добавить, только если он не пересекается ни с одним
  // из существующих.
  using namespace geo2d;

  const vector<shared_ptr<GameObject>> game_map = {
      make_shared<Unit>(Point{3, 3}),
      make_shared<Unit>(Point{5, 5}),
      make_shared<Unit>(Point{3, 7}),
      make_shared<Fence>(Segment{{7, 3}, {9, 8}}),
      make_shared<Tower>(Circle{Point{9, 4}, 1}),
      make_shared<Tower>(Circle{Point{10, 7}, 1}),
      make_shared<Building>(Rectangle{{11, 4}, {14, 6}})};

  for (size_t i = 0; i < game_map.size(); ++i) {
    Assert(Collide(*game_map[i], *game_map[i]),
           "An object doesn't collide with itself: " + to_string(i));

    for (size_t j = 0; j < i; ++j) {
      Assert(!Collide(*game_map[i], *game_map[j]),
             "Unexpected collision found " + to_string(i) + ' ' + to_string(j));
    }
  }

  auto new_warehouse = make_shared<Building>(Rectangle{{4, 3}, {9, 6}});
  ASSERT(!Collide(*new_warehouse, *game_map[0]));
  ASSERT(Collide(*new_warehouse, *game_map[1]));
  ASSERT(!Collide(*new_warehouse, *game_map[2]));
  ASSERT(Collide(*new_warehouse, *game_map[3]));
  ASSERT(Collide(*new_warehouse, *game_map[4]));
  ASSERT(!Collide(*new_warehouse, *game_map[5]));
  ASSERT(!Collide(*new_warehouse, *game_map[6]));

  auto new_defense_tower = make_shared<Tower>(Circle{{8, 2}, 2});
  ASSERT(!Collide(*new_defense_tower, *game_map[0]));
  ASSERT(!Collide(*new_defense_tower, *game_map[1]));
  ASSERT(!Collide(*new_defense_tower, *game_map[2]));
  ASSERT(Collide(*new_defense_tower, *game_map[3]));
  ASSERT(Collide(*new_defense_tower, *game_map[4]));
  ASSERT(!Collide(*new_defense_tower, *game_map[5]));
  ASSERT(!Collide(*new_defense_tower, *game_map[6]));
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestAddingNewObjectOnMap);
  return 0;
}
