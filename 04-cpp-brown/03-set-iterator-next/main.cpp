#include <cassert>
#include <deque>
#include <iostream>

#include "test_runner.h"

using namespace std;

struct Node {
  Node(int v, Node* p) : value(v), parent(p) {}

  int value;
  Node* left = nullptr;
  Node* right = nullptr;
  Node* parent;
};

class NodeBuilder {
 public:
  Node* CreateRoot(int value) {
    nodes.emplace_back(value, nullptr);
    return &nodes.back();
  }

  Node* CreateLeftSon(Node* me, int value) {
    assert(me->left == nullptr);
    nodes.emplace_back(value, me);
    me->left = &nodes.back();
    return me->left;
  }

  Node* CreateRightSon(Node* me, int value) {
    assert(me->right == nullptr);
    nodes.emplace_back(value, me);
    me->right = &nodes.back();
    return me->right;
  }

 private:
  deque<Node> nodes;
};

Node* Next(Node* me) {
  if (me->right) {
    me = me->right;
    while (me->left) {
      me = me->left;
    }
  } else {
    while (me->parent && me->parent->right == me) {
      me = me->parent;
    }
    me = me->parent;
  }
  return me;
}

void Test1() {
  NodeBuilder nb;

  Node* root = nb.CreateRoot(50);
  ASSERT_EQUAL(root->value, 50);

  Node* l = nb.CreateLeftSon(root, 2);
  Node* min = nb.CreateLeftSon(l, 1);
  Node* r = nb.CreateRightSon(l, 4);
  ASSERT_EQUAL(min->value, 1);
  ASSERT_EQUAL(r->parent->value, 2);

  nb.CreateLeftSon(r, 3);
  nb.CreateRightSon(r, 5);

  r = nb.CreateRightSon(root, 100);
  l = nb.CreateLeftSon(r, 90);
  nb.CreateRightSon(r, 101);

  nb.CreateLeftSon(l, 89);
  r = nb.CreateRightSon(l, 91);

  ASSERT_EQUAL(Next(l)->value, 91);
  ASSERT_EQUAL(Next(root)->value, 89);
  ASSERT_EQUAL(Next(min)->value, 2);
  ASSERT_EQUAL(Next(r)->value, 100);

  while (min) {
    cout << min->value << '\n';
    min = Next(min);
  }
}

void TestRootOnly() {
  NodeBuilder nb;
  Node* root = nb.CreateRoot(42);
  ASSERT(Next(root) == nullptr);
};

void TestComplexTree() {
  NodeBuilder nb;

  Node* root = nb.CreateRoot(0);

  Node* n8 = nb.CreateRightSon(root, 8);
  Node* n4 = nb.CreateLeftSon(n8, 4);
  Node* n12 = nb.CreateRightSon(n8, 12);

  Node* n2 = nb.CreateLeftSon(n4, 2);
  Node* n1 = nb.CreateLeftSon(n2, 1);
  Node* n3 = nb.CreateRightSon(n2, 3);

  Node* n6 = nb.CreateRightSon(n4, 6);
  Node* n5 = nb.CreateLeftSon(n6, 5);
  Node* n7 = nb.CreateRightSon(n6, 7);

  Node* n10 = nb.CreateLeftSon(n12, 10);
  Node* n9 = nb.CreateLeftSon(n10, 9);
  Node* n11 = nb.CreateRightSon(n10, 11);

  Node* n14 = nb.CreateRightSon(n12, 14);
  Node* n13 = nb.CreateLeftSon(n14, 13);
  Node* n15 = nb.CreateRightSon(n14, 15);

  vector<int> values;
  Node* current = root;
  while ((current = Next(current))) {
    values.push_back(current->value);
  }

  AssertEqual(values, vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, Test1);
  RUN_TEST(tr, TestRootOnly);
  RUN_TEST(tr, TestComplexTree);
  return 0;
}
