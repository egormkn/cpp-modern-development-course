#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "test_runner.h"

using namespace std;

struct Email {
  string from;
  string to;
  string body;

  Email() {}

  Email(string from, string to, string body)
      : from(move(from)), to(move(to)), body(move(body)) {}

  friend istream& operator>>(istream& is, Email& email) {
    getline(is, email.from);
    getline(is, email.to);
    getline(is, email.body);
    return is;
  }

  friend ostream& operator<<(ostream& os, const Email& email) {
    return os << email.from << '\n' << email.to << '\n' << email.body;
  }
};

class Worker {
 public:
  virtual ~Worker() = default;

  virtual void Process(unique_ptr<Email> email) = 0;

  virtual void Run() {
    // только первому worker-у в пайплайне нужно это имплементировать
    throw logic_error("Unimplemented");
  }

 protected:
  // реализации должны вызывать PassOn, чтобы передать объект дальше
  // по цепочке обработчиков
  void PassOn(unique_ptr<Email> email) const { 
    if (next) {
      next->Process(move(email));
    } 
  }

 public:
  void SetNext(unique_ptr<Worker> new_next) { next = move(new_next); }

 private:
  unique_ptr<Worker> next;
};

class Reader : public Worker {
 public:
  Reader(istream& is) : is(is) {}

  void Process(unique_ptr<Email> email) override {
    PassOn(move(email));
  }

  void Run() override {
    Email email;
    while (is >> email) {
      Process(make_unique<Email>(move(email)));
    }
  }

 private:
  istream& is;
};

class Filter : public Worker {
 public:
  using Function = function<bool(const Email&)>;

 public:
  Filter(Function predicate) : predicate(move(predicate)) {}

  void Process(unique_ptr<Email> email) override {
    if (predicate(*email)) {
      PassOn(move(email));
    }
  }

 private:
  Function predicate;
};

class Copier : public Worker {
 public:
  Copier(string address) : address(move(address)) {}

  void Process(unique_ptr<Email> email) override {
    unique_ptr<Email> copy;
    if (address != email->to) {
      copy = make_unique<Email>(*email);
      copy->to = address;
    }
    PassOn(move(email));
    if (copy) {
      PassOn(move(copy));
    }
  }

 private:
  string address;
};

class Sender : public Worker {
 public:
  Sender(ostream& os) : os(os) {}

  void Process(unique_ptr<Email> email) override {
    os << *email << '\n';
    PassOn(move(email));
  }

 private:
  ostream& os;
};

// реализуйте класс
class PipelineBuilder {
 public:
  // добавляет в качестве первого обработчика Reader
  explicit PipelineBuilder(istream& in) : head(make_unique<Reader>(in)), tail(head.get()) {}

  // добавляет новый обработчик Filter
  PipelineBuilder& FilterBy(Filter::Function filter) {
    return AddWorker(make_unique<Filter>(filter));
  }

  // добавляет новый обработчик Copier
  PipelineBuilder& CopyTo(string recipient) {
    return AddWorker(make_unique<Copier>(recipient));
  }

  // добавляет новый обработчик Sender
  PipelineBuilder& Send(ostream& out) {
    return AddWorker(make_unique<Sender>(out));
  }

  // возвращает готовую цепочку обработчиков
  unique_ptr<Worker> Build() {
    tail = nullptr;
    return move(head);
  }

 private:
  unique_ptr<Worker> head;
  Worker* tail;

  PipelineBuilder& AddWorker(unique_ptr<Worker> worker) {
    Worker* new_tail = worker.get();
    tail->SetNext(move(worker));
    tail = new_tail;
    return *this;
  }
};

void TestSanity() {
  string input =
      ("erich@example.com\n"
       "richard@example.com\n"
       "Hello there\n"

       "erich@example.com\n"
       "ralph@example.com\n"
       "Are you sure you pressed the right button?\n"

       "ralph@example.com\n"
       "erich@example.com\n"
       "I do not make mistakes of that kind\n");
  istringstream inStream(input);
  ostringstream outStream;

  PipelineBuilder builder(inStream);
  builder.FilterBy(
      [](const Email& email) { return email.from == "erich@example.com"; });
  builder.CopyTo("richard@example.com");
  builder.Send(outStream);
  auto pipeline = builder.Build();

  pipeline->Run();

  string expectedOutput =
      ("erich@example.com\n"
       "richard@example.com\n"
       "Hello there\n"

       "erich@example.com\n"
       "ralph@example.com\n"
       "Are you sure you pressed the right button?\n"

       "erich@example.com\n"
       "richard@example.com\n"
       "Are you sure you pressed the right button?\n");

  ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSanity);
  return 0;
}
