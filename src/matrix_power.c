#include "algorithms.h"

Matrix2x2 matrix2x2_identity(void) {
    Matrix2x2 result = {{{1, 0}, {0, 1}}};
    return result;
}

Matrix2x2 matrix2x2_multiply(const Matrix2x2 *a, const Matrix2x2 *b) {
    Matrix2x2 result = {{{0, 0}, {0, 0}}};

    for (size_t row = 0; row < 2; ++row) {
        for (size_t col = 0; col < 2; ++col) {
            for (size_t k = 0; k < 2; ++k) {
                result.value[row][col] += a->value[row][k] * b->value[k][col];
            }
        }
    }
    return result;
}

Matrix2x2 matrix2x2_power(const Matrix2x2 *base, uint64_t exponent) {
    Matrix2x2 result = matrix2x2_identity();
    Matrix2x2 factor = *base;

    while (exponent != 0) {
        if ((exponent & 1U) != 0) {
            result = matrix2x2_multiply(&result, &factor);
        }
        exponent >>= 1;
        if (exponent != 0) {
            factor = matrix2x2_multiply(&factor, &factor);
        }
    }
    return result;
}
