#include <cmath>
#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

using namespace std;

class Figure {
 public:
  virtual string Name() const = 0;
  virtual double Perimeter() const = 0;
  virtual double Area() const = 0;
};

class Triangle : public Figure {
 public:
  Triangle(int a, int b, int c) : a(a), b(b), c(c) {}

  string Name() const override { return "TRIANGLE"; }
  double Perimeter() const override { return a + b + c; }
  double Area() const override {
    double p = Perimeter() / 2.0;
    return sqrt(p * (p - a) * (p - b) * (p - c));
  }

 private:
  const int a, b, c;
};

class Rect : public Figure {
 public:
  Rect(int width, int height) : width(width), height(height) {}

  string Name() const override { return "RECT"; }
  double Perimeter() const override { return 2 * (width + height); }
  double Area() const override { return width * height; }

 private:
  const int width, height;
};

class Circle : public Figure {
 public:
  Circle(int radius) : r(radius) {}

  string Name() const override { return "CIRCLE"; }
  double Perimeter() const override { return 2 * 3.14 * r; }
  double Area() const override { return 3.14 * r * r; }

 private:
  const int r;
};

shared_ptr<Figure> CreateFigure(istream& is) {
  string type;
  is >> type;
  if (type == "RECT") {
    int width, height;
    is >> width >> height;
    return make_shared<Rect>(width, height);
  } else if (type == "TRIANGLE") {
    int a, b, c;
    is >> a >> b >> c;
    return make_shared<Triangle>(a, b, c);
  } else if (type == "CIRCLE") {
    int r;
    is >> r;
    return make_shared<Circle>(r);
  } else {
    throw invalid_argument("Figure expected");
  }
}

int main() {
  vector<shared_ptr<Figure>> figures;
  for (string line; getline(cin, line);) {
    istringstream is(line);

    string command;
    is >> command;
    if (command == "ADD") {
      // Пропускаем "лишние" ведущие пробелы.
      // Подробнее об std::ws можно узнать здесь:
      // https://en.cppreference.com/w/cpp/io/manip/ws
      is >> ws;
      figures.push_back(CreateFigure(is));
    } else if (command == "PRINT") {
      for (const auto& current_figure : figures) {
        cout << fixed << setprecision(3) << current_figure->Name() << " "
             << current_figure->Perimeter() << " " << current_figure->Area()
             << endl;
      }
    }
  }
  return 0;
}