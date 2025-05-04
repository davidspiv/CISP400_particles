#include "Matrices.h"

#include <iomanip>
#include <vector>

Matrix::Matrix(int rows, int cols)
    : m_values(
          std::vector<std::vector<double>>(rows, std::vector<double>(cols, 0))),
      m_rows(rows), m_cols(cols) {};

std::ostream &operator<<(std::ostream &os, const Matrix &a) {
  for (size_t row = 0; row < a.getRows(); row++) {
    for (size_t col = 0; col < a.getRows(); col++) {
      os << std::setw(10) << a(row, col) << ' ';
    }
    os << '\n';
  }
  return os;
}

Matrix operator*(const Matrix &a, const Matrix &b) {
  if (a.getCols() != b.getRows()) {
    throw std::runtime_error("Error: mismatched inner dimensions");
  }

  Matrix result(a.getRows(), b.getCols());

  for (size_t i = 0; i < a.getRows(); i++) {
    for (size_t k = 0; k < b.getCols(); k++) {
      float sum = 0;
      for (size_t j = 0; j < a.getCols(); j++) {
        sum += a(i, j) * b(j, k);
      }
      result(i, k) = sum;
    }
  }

  return result;
}
