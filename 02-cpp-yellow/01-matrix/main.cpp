#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

// Реализуйте здесь
// * класс Matrix
// * оператор ввода для класса Matrix из потока istream
// * оператор вывода класса Matrix в поток ostream
// * оператор проверки на равенство двух объектов класса Matrix
// * оператор сложения двух объектов класса Matrix

class Matrix {
 public:
  Matrix() : rows(0), columns(0) {}

  Matrix(int num_rows, int num_columns) { Reset(num_rows, num_columns); }

  void Reset(int num_rows, int num_columns) {
    if (num_rows < 0) {
      throw out_of_range("Number of rows can't be negative");
    }
    if (num_columns < 0) {
      throw out_of_range("Number of columns can't be negative");
    }
    if (num_rows == 0 || num_columns == 0) {
      num_rows = num_columns = 0;
    }
    rows = num_rows;
    columns = num_columns;
    elements.assign(rows, vector<int>(columns));
  }

  int At(int row, int column) const { return elements.at(row).at(column); }

  int &At(int row, int column) { return elements.at(row).at(column); }

  int GetNumRows() const { return rows; }

  int GetNumColumns() const { return columns; }

 private:
  int rows, columns;
  vector<vector<int>> elements;
};

istream &operator>>(istream &stream, Matrix &matrix) {
  int rows, columns;
  stream >> rows >> columns;
  matrix.Reset(rows, columns);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      stream >> matrix.At(i, j);
    }
  }
  return stream;
}

ostream &operator<<(ostream &stream, const Matrix &matrix) {
  int rows = matrix.GetNumRows(), columns = matrix.GetNumColumns();
  stream << rows << ' ' << columns << endl;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (j > 0) {
        stream << ' ';
      }
      stream << matrix.At(i, j);
    }
    stream << endl;
  }
  return stream;
}

bool operator==(const Matrix &lhs, const Matrix &rhs) {
  if (lhs.GetNumRows() != rhs.GetNumRows() ||
      lhs.GetNumColumns() != rhs.GetNumColumns()) {
    return false;
  }
  for (int i = 0; i < lhs.GetNumRows(); ++i) {
    for (int j = 0; j < lhs.GetNumColumns(); ++j) {
      if (lhs.At(i, j) != rhs.At(i, j)) return false;
    }
  }
  return true;
}

Matrix operator+(const Matrix &lhs, const Matrix &rhs) {
  if (lhs.GetNumRows() != rhs.GetNumRows() ||
      lhs.GetNumColumns() != rhs.GetNumColumns()) {
    throw invalid_argument("Matrices must be the same size");
  }
  int rows = lhs.GetNumRows(), columns = lhs.GetNumColumns();
  Matrix result(rows, columns);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      result.At(i, j) = lhs.At(i, j) + rhs.At(i, j);
    }
  }
  return result;
}

int main() {
  Matrix one;
  Matrix two;

  cin >> one >> two;
  cout << one + two << endl;
  return 0;
}