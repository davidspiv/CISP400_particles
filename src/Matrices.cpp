#include "Matrices.h"

#include <iomanip>
#include <stdexcept>
#include <vector>

namespace Matrices {

Matrix::Matrix(int rows, int cols)
    : m_values(std::vector<std::vector<double>>(rows, std::vector<double>(cols, 0)))
    , m_rows(rows)
    , m_cols(cols)
{
}

Matrix operator+(Matrix const& a, Matrix const& b)
{
    if (a.rows() != b.rows() || a.cols() != b.cols()) {
        throw std::domain_error("Error: mismatched dimensions");
    }

    Matrix result(a.rows(), a.cols());

    for (int i = 0; i < a.rows(); i++) {
        for (int j = 0; j < a.cols(); j++) {
            result(i, j) = a(i, j) + b(i, j);
        }
    }

    return result;
}

Matrix operator*(Matrix const& a, Matrix const& b)
{
    if (a.cols() != b.rows()) {
        throw std::domain_error("Error: mismatched inner dimensions");
    }

    Matrix result(a.rows(), b.cols());

    for (int i = 0; i < a.rows(); i++) {
        for (int k = 0; k < b.cols(); k++) {
            float sum = 0;
            for (int j = 0; j < a.cols(); j++) {
                sum += a(i, j) * b(j, k);
            }
            result(i, k) = sum;
        }
    }

    return result;
}

bool operator==(Matrix const& a, Matrix const& b)
{
    if (a.rows() != b.rows() || a.cols() != b.cols()) {
        return false;
    }

    Matrix result(a.rows(), a.cols());

    for (int i = 0; i < a.rows(); i++) {
        for (int j = 0; j < a.cols(); j++) {
            if (a(i, j) != b(i, j)) {
                return false;
            }
        }
    }

    return true;
}

bool operator!=(Matrix const& a, Matrix const& b) { return !(a == b); }

std::ostream& operator<<(std::ostream& os, Matrix const& a)
{
    for (int row = 0; row < a.rows(); row++) {
        for (int col = 0; col < a.rows(); col++) {
            os << std::setw(10) << a(row, col) << ' ';
        }
        os << '\n';
    }
    return os;
}

RotationMatrix::RotationMatrix(double theta)
    : Matrix(2, 2)
{
    double const cosTheta = cos(theta);
    double const sinTheta = sin(theta);

    m_values[0][0] = cosTheta;
    m_values[0][1] = -sinTheta;
    m_values[1][0] = sinTheta;
    m_values[1][1] = cosTheta;
}

ScalingMatrix::ScalingMatrix(double scale)
    : Matrix(2, 2)
{

    m_values[0][0] = scale;
    m_values[1][1] = scale;
}

TranslationMatrix::TranslationMatrix(double xShift, double yShift, int nCols)
    : Matrix(2, nCols)
{

    for (int i = 0; i < nCols; i++) {
        m_values[0][i] = xShift;
        m_values[1][i] = yShift;
    }
}

} // namespace Matrices
