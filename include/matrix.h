#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    size_t rows;
    size_t cols;
    int64_t *data;
} Matrix;

int matrix_init(Matrix *matrix, size_t rows, size_t cols);
void matrix_free(Matrix *matrix);
int64_t *matrix_at(Matrix *matrix, size_t row, size_t col);
const int64_t *matrix_at_const(const Matrix *matrix, size_t row, size_t col);
void matrix_fill(Matrix *matrix, int64_t value);
void matrix_fill_deterministic(Matrix *matrix, uint64_t seed);
int matrix_copy(const Matrix *source, Matrix *destination);

/* Devuelven 1 si tienen éxito y 0 si las dimensiones o la memoria son inválidas. */
int matrix_multiply_classic(const Matrix *a, const Matrix *b, Matrix *result);
int matrix_multiply_strassen(const Matrix *a, const Matrix *b, Matrix *result);

#endif
