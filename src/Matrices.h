#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

namespace Matrices {

class Matrix {
public:
    Matrix(int rows, int cols);
    explicit Matrix(std::vector<std::vector<float>> const& arr_2d);

    float const& operator()(int i, int j) const { return m_values.at(i).at(j); }
    float& operator()(int i, int j) { return m_values.at(i).at(j); }

    int rows() const { return m_rows; }
    int cols() const { return m_cols; }

    Matrix column_wise_scaling(Matrix const& other) const;
    Matrix invert() const;
    std::vector<float> flatten() const;

protected:
    std::vector<std::vector<float>> m_values;

private:
    size_t m_rows;
    size_t m_cols;
};

Matrix operator+(Matrix const& a, Matrix const& b);
Matrix operator*(Matrix const& a, Matrix const& b);
bool operator==(Matrix const& a, Matrix const& b);
bool operator!=(Matrix const& a, Matrix const& b);
std::ostream& operator<<(std::ostream& os, Matrix const& a);

/*******************************************************************************/

/// 2D rotation matrix
/// usage:  A = R * A rotates A theta radians counter-clockwise
class RotationMatrix : public Matrix {
public:
    /// theta represents the angle of rotation in radians, counter-clockwise
    RotationMatrix(float theta);
};

/// 2D scaling matrix
/// usage:  A = S * A expands or contracts A by the specified scaling factor
class ScalingMatrix : public Matrix {
public:
    /// scale represents the size multiplier
    ScalingMatrix(float scale);
};

/// 2D Translation matrix
/// usage:  A = T + A will shift all coordinates of A by (xShift, yShift)
class TranslationMatrix : public Matrix {
public:
    /// parameters are xShift, yShift, and nCols
    /// nCols represents the number of columns in the matrix
    /// where each column contains one (x,y) coordinate pair
    TranslationMatrix(float xShift, float yShift, int nCols);
};

Matrix create_to_xyz_transformation_matrix(std::array<float, 3> ref_white);

} // namespace Matrices

#endif
