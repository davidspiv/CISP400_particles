#include "Matrices.h"

#include <iomanip>
#include <stdexcept>
#include <vector>

Matrix::Matrix(int rows, int cols)
    : m_values(
          std::vector<std::vector<double>>(rows, std::vector<double>(cols, 0))),
      m_rows(rows), m_cols(cols) {};


Matrix operator+(const Matrix &a, const Matrix &b) {
  if (a.rows() != b.rows() || a.cols() != b.cols()) {
    throw std::domain_error("Error: mismatched dimensions");
  }

  Matrix result(a.rows(), a.cols());

  for (size_t i = 0; i < a.rows(); i++) {
    for (size_t j = 0; j < a.cols(); j++) {
      result(i, j) = a(i, j) + b(i, j);
    }
  }

  return result;
}


Matrix operator*(const Matrix &a, const Matrix &b) {
  if (a.cols() != b.rows()) {
    throw std::domain_error("Error: mismatched inner dimensions");
  }

  Matrix result(a.rows(), b.cols());

  for (size_t i = 0; i < a.rows(); i++) {
    for (size_t k = 0; k < b.cols(); k++) {
      float sum = 0;
      for (size_t j = 0; j < a.cols(); j++) {
        sum += a(i, j) * b(j, k);
      }
      result(i, k) = sum;
    }
  }

  return result;
}


bool operator==(const Matrix &a, const Matrix &b) {
  if (a.rows() != b.rows() || a.cols() != b.cols()) {
    return false;
  }

  Matrix result(a.rows(), a.cols());

  for (size_t i = 0; i < a.rows(); i++) {
    for (size_t j = 0; j < a.cols(); j++) {
      if (a(i, j) != b(i, j)) {
        return false;
      }
    }
  }

  return true;
}


bool operator!=(const Matrix &a, const Matrix &b) { return !(a == b); }


std::ostream &operator<<(std::ostream &os, const Matrix &a) {
  for (size_t row = 0; row < a.rows(); row++) {
    for (size_t col = 0; col < a.rows(); col++) {
      os << std::setw(10) << a(row, col) << ' ';
    }
    os << '\n';
  }
  return os;
}
