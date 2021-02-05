#include <algorithm>
#include <string>

#include "test_runner.h"

using namespace std;

class Editor {
 public:
  Editor() {}

  void Left() { Move(before, after); }
  void Right() { Move(after, before); }

  void Insert(char token) { before.push_back(token); }

  void Cut(size_t tokens = 1) {
    buffer.clear();
    buffer.reserve(tokens);
    Move(after, buffer, tokens);
  }

  void Copy(size_t tokens = 1) {
    buffer.clear();
    buffer.reserve(tokens);
    copy_n(after.rbegin(), min(tokens, after.size()), back_inserter(buffer));
  }

  void Paste() { before += buffer; }

  string GetText() const {
    string result;
    result.reserve(before.size() + after.size());
    result += before;
    copy(after.rbegin(), after.rend(), back_inserter(result));
    return result;
  }

 private:
  string before, after, buffer;

  void Move(string& src, string& dest, size_t num = 1) {
    for (size_t i = 0; i < num && !src.empty(); ++i) {
      dest.push_back(src.back());
      src.pop_back();
    }
  }
};

void TypeText(Editor& editor, const string& text) {
  for (char c : text) {
    editor.Insert(c);
  }
}

void TestEditing() {
  {
    Editor editor;

    const size_t text_len = 12;
    const size_t first_part_len = 7;
    TypeText(editor, "hello, world");
    for (size_t i = 0; i < text_len; ++i) {
      editor.Left();
    }
    editor.Cut(first_part_len);
    for (size_t i = 0; i < text_len - first_part_len; ++i) {
      editor.Right();
    }
    TypeText(editor, ", ");
    editor.Paste();
    editor.Left();
    editor.Left();
    editor.Cut(3);

    ASSERT_EQUAL(editor.GetText(), "world, hello");
  }
  {
    Editor editor;

    TypeText(editor, "misprnit");
    editor.Left();
    editor.Left();
    editor.Left();
    editor.Cut(1);
    editor.Right();
    editor.Paste();

    ASSERT_EQUAL(editor.GetText(), "misprint");
  }
}

void TestReverse() {
  Editor editor;

  const string text = "esreveR";
  for (char c : text) {
    editor.Insert(c);
    editor.Left();
  }

  ASSERT_EQUAL(editor.GetText(), "Reverse");
}

void TestNoText() {
  Editor editor;
  ASSERT_EQUAL(editor.GetText(), "");

  editor.Left();
  editor.Left();
  editor.Right();
  editor.Right();
  editor.Copy(0);
  editor.Cut(0);
  editor.Paste();

  ASSERT_EQUAL(editor.GetText(), "");
}

void TestEmptyBuffer() {
  Editor editor;

  editor.Paste();
  TypeText(editor, "example");
  editor.Left();
  editor.Left();
  editor.Paste();
  editor.Right();
  editor.Paste();
  editor.Copy(0);
  editor.Paste();
  editor.Left();
  editor.Cut(0);
  editor.Paste();

  ASSERT_EQUAL(editor.GetText(), "example");
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestEditing);
  RUN_TEST(tr, TestReverse);
  RUN_TEST(tr, TestNoText);
  RUN_TEST(tr, TestEmptyBuffer);
  return 0;
}