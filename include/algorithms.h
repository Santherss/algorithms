#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <stdint.h>

#include "matrix.h"

uint64_t power_linear(uint64_t base, uint64_t exponent);
uint64_t power_logarithmic(uint64_t base, uint64_t exponent);

typedef struct {
    int64_t value[2][2];
} Matrix2x2;

Matrix2x2 matrix2x2_identity(void);
Matrix2x2 matrix2x2_multiply(const Matrix2x2 *a, const Matrix2x2 *b);
Matrix2x2 matrix2x2_power(const Matrix2x2 *base, uint64_t exponent);

uint64_t fibonacci_linear(uint64_t n);
uint64_t fibonacci_logarithmic(uint64_t n);

#endif
