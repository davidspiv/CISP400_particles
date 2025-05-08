#include "Matrices.h"

#include <iomanip>
#include <stdexcept>
#include <vector>

namespace Matrices {

Matrix::Matrix(int rows, int cols)
    : m_values(std::vector<std::vector<float>>(rows, std::vector<float>(cols, 0)))
    , m_rows(rows)
    , m_cols(cols)
{
}

Matrix::Matrix(std::vector<std::vector<float>> const& arr_2d)
{
    m_rows = arr_2d.size();
    m_cols = arr_2d[0].size();
    m_values = arr_2d;
}

Matrix Matrix::column_wise_scaling(Matrix const& other) const
{
    Matrix result(m_rows, m_cols);

    for (size_t j = 0; j < m_cols; j++) {
        for (size_t i = 0; i < m_rows; i++) {
            result(i, j) = m_values[i][j] * other(j, 0);
        }
    }

    return result;
}

// LLM code
Matrix Matrix::invert() const
{
    if (m_rows != m_cols) {
        throw std::runtime_error("Matrix inversion only defined for square matrices.");
    }

    int n = m_rows;
    Matrix result(n, n);
    Matrix augmented(n, 2 * n);

    // Step 1: Set up augmented matrix [A | I]
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            augmented(i, j) = (*this)(i, j);              // original A
            augmented(i, j + n) = (i == j) ? 1.0f : 0.0f; // identity matrix I
        }
    }

    // Step 2: Forward elimination
    for (int i = 0; i < n; ++i) {
        // Find pivot
        float pivot = augmented(i, i);
        if (pivot == 0.0f) {
            throw std::runtime_error("Matrix is singular and cannot be inverted.");
        }

        // Normalize the pivot row
        for (int j = 0; j < 2 * n; ++j) {
            augmented(i, j) /= pivot;
        }

        // Eliminate column i in other rows
        for (int k = 0; k < n; ++k) {
            if (k == i)
                continue;
            float factor = augmented(k, i);
            for (int j = 0; j < 2 * n; ++j) {
                augmented(k, j) -= factor * augmented(i, j);
            }
        }
    }

    // Step 3: Copy right half to result
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            result(i, j) = augmented(i, j + n);
        }
    }

    return result;
}

std::vector<float> Matrix::flatten() const
{
    std::vector<float> values(m_cols * m_rows, 0.0f);

    for (size_t i = 0; i < m_cols; i++) {
        for (size_t j = 0; j < m_rows; j++) {
            values[i + j] = m_values[i][j];
        }
    }
    return values;
}

Matrix arr_to_column(std::array<float, 3> arr)
{
    return Matrix({ { arr[0] }, { arr[1] }, { arr[2] } });
}

Matrix create_to_xyz_transformation_matrix(std::array<float, 3> ref_white)
{
    auto const [r_x, r_y, r_z] = std::array<float, 3> { 0.6400f, 0.3300f, 0.212656f };
    auto const [g_x, g_y, g_z] = std::array<float, 3> { 0.3000f, 0.6000f, 0.715158f };
    auto const [b_x, b_y, b_z] = std::array<float, 3> { 0.1500f, 0.0600f, 0.072186f };

    float const r_X = r_x / r_y;
    float const r_Y = 1;
    float const r_Z = (1 - r_x - r_y) / r_y;

    float const g_X = g_x / g_y;
    float const g_Y = 1;
    float const g_Z = (1 - g_x - g_y) / g_y;

    float const b_X = b_x / b_y;
    float const b_Y = 1;
    float const b_Z = (1 - b_x - b_y) / b_y;

    std::vector<std::vector<float>> const XYZ
        = { { r_X, g_X, b_X }, { r_Y, g_Y, b_Y }, { r_Z, g_Z, b_Z } };

    Matrix const XYZ_matrix(XYZ);
    Matrix const XYZ_matrix_inverted = XYZ_matrix.invert();
    Matrix const S_matrix = XYZ_matrix_inverted * arr_to_column(ref_white);

    return XYZ_matrix.column_wise_scaling(S_matrix);
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

RotationMatrix::RotationMatrix(float theta)
    : Matrix(2, 2)
{
    float const cosTheta = cos(theta);
    float const sinTheta = sin(theta);

    m_values[0][0] = cosTheta;
    m_values[0][1] = -sinTheta;
    m_values[1][0] = sinTheta;
    m_values[1][1] = cosTheta;
}

ScalingMatrix::ScalingMatrix(float scale)
    : Matrix(2, 2)
{

    m_values[0][0] = scale;
    m_values[1][1] = scale;
}

TranslationMatrix::TranslationMatrix(float xShift, float yShift, int nCols)
    : Matrix(2, nCols)
{

    for (int i = 0; i < nCols; i++) {
        m_values[0][i] = xShift;
        m_values[1][i] = yShift;
    }
}

} // namespace Matrices
