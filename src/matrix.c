#include "matrix.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int element_count(size_t rows, size_t cols, size_t *count) {
    if (cols != 0 && rows > SIZE_MAX / cols) return 0;
    *count = rows * cols;
    return 1;
}

static int allocate_elements(size_t count, int64_t **data) {
    *data = count == 0 ? NULL : malloc(count * sizeof(**data));
    return count == 0 || *data != NULL;
}

int matrix_init(Matrix *matrix, size_t rows, size_t cols) {
    size_t count;
    int64_t *data;
    if (matrix == NULL || !element_count(rows, cols, &count) ||
        count > SIZE_MAX / sizeof(*data) || !allocate_elements(count, &data)) {
        return 0;
    }
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->data = data;
    return 1;
}

void matrix_free(Matrix *matrix) {
    if (matrix == NULL) return;
    free(matrix->data);
    matrix->data = NULL;
    matrix->rows = 0;
    matrix->cols = 0;
}

int64_t *matrix_at(Matrix *matrix, size_t row, size_t col) {
    return &matrix->data[row * matrix->cols + col];
}

const int64_t *matrix_at_const(const Matrix *matrix, size_t row, size_t col) {
    return &matrix->data[row * matrix->cols + col];
}

void matrix_fill(Matrix *matrix, int64_t value) {
    size_t count;
    if (matrix == NULL || !element_count(matrix->rows, matrix->cols, &count)) return;
    for (size_t i = 0; i < count; ++i) matrix->data[i] = value;
}

void matrix_fill_deterministic(Matrix *matrix, uint64_t seed) {
    size_t count;
    if (matrix == NULL || !element_count(matrix->rows, matrix->cols, &count)) return;
    for (size_t i = 0; i < count; ++i) {
        seed = seed * UINT64_C(6364136223846793005) + UINT64_C(1442695040888963407);
        matrix->data[i] = (int64_t)((seed >> 32) % 200) - 100;
    }
}

int matrix_copy(const Matrix *source, Matrix *destination) {
    size_t count;
    int64_t *data;
    if (source == NULL || destination == NULL ||
        !element_count(source->rows, source->cols, &count) ||
        count > SIZE_MAX / sizeof(*data) || !allocate_elements(count, &data)) return 0;
    if (count != 0) memcpy(data, source->data, count * sizeof(*data));
    if (source == destination) {
        free(data);
        return 1;
    }
    free(destination->data);
    *destination = (Matrix){source->rows, source->cols, data};
    return 1;
}

static int valid_multiplication(const Matrix *a, const Matrix *b) {
    return a != NULL && b != NULL && a->data != NULL && b->data != NULL &&
           a->rows != 0 && a->cols != 0 && b->cols != 0 && a->cols == b->rows;
}

int matrix_multiply_classic(const Matrix *a, const Matrix *b, Matrix *result) {
    Matrix product = {0, 0, NULL};
    if (result == NULL || !valid_multiplication(a, b) ||
        !matrix_init(&product, a->rows, b->cols)) return 0;
    matrix_fill(&product, 0);
    for (size_t i = 0; i < a->rows; ++i) {
        for (size_t k = 0; k < a->cols; ++k) {
            const int64_t left = *matrix_at_const(a, i, k);
            for (size_t j = 0; j < b->cols; ++j) {
                *matrix_at(&product, i, j) +=
                    left * *matrix_at_const(b, k, j);
            }
        }
    }
    matrix_free(result);
    *result = product;
    return 1;
}

static void add_blocks(const int64_t *a, size_t a_stride, const int64_t *b,
                       size_t b_stride, int64_t *out, size_t size, int subtract) {
    for (size_t row = 0; row < size; ++row) {
        for (size_t col = 0; col < size; ++col) {
            const int64_t left = a[row * a_stride + col];
            const int64_t right = b[row * b_stride + col];
            out[row * size + col] = subtract ? left - right : left + right;
        }
    }
}

static int strassen_recursive(const int64_t *a, size_t a_stride,
                              const int64_t *b, size_t b_stride,
                              int64_t *out, size_t out_stride, size_t size) {
    /* Para bloques pequeños, la multiplicación clásica evita la sobrecarga recursiva. */
    if (size <= 32) {
        for (size_t row = 0; row < size; ++row) {
            for (size_t col = 0; col < size; ++col) {
                int64_t sum = 0;
                for (size_t k = 0; k < size; ++k)
                    sum += a[row * a_stride + k] * b[k * b_stride + col];
                out[row * out_stride + col] = sum;
            }
        }
        return 1;
    }

    const size_t half = size / 2;
    size_t count;
    int64_t *products[7] = {NULL};
    int64_t *left_sum = NULL;
    int64_t *right_sum = NULL;
    if (!element_count(half, half, &count)) return 0;
    for (size_t i = 0; i < 7; ++i) {
        if (!allocate_elements(count, &products[i])) goto failure;
    }
    if (!allocate_elements(count, &left_sum) || !allocate_elements(count, &right_sum)) goto failure;

    /* Cada matriz se interpreta como cuatro bloques de tamaño half x half. */
    const int64_t *a11 = a;
    const int64_t *a12 = a + half;
    const int64_t *a21 = a + half * a_stride;
    const int64_t *a22 = a21 + half;
    const int64_t *b11 = b;
    const int64_t *b12 = b + half;
    const int64_t *b21 = b + half * b_stride;
    const int64_t *b22 = b21 + half;

    /* Los siete productos de Strassen; left_sum y right_sum se reutilizan. */
    /* P1 = (A11 + A22)(B11 + B22) */
    add_blocks(a11, a_stride, a22, a_stride, left_sum, half, 0);
    add_blocks(b11, b_stride, b22, b_stride, right_sum, half, 0);
    if (!strassen_recursive(left_sum, half, right_sum, half, products[0], half, half)) goto failure;
    /* P2 = (A21 + A22)B11 */
    add_blocks(a21, a_stride, a22, a_stride, left_sum, half, 0);
    if (!strassen_recursive(left_sum, half, b11, b_stride, products[1], half, half)) goto failure;
    /* P3 = A11(B12 - B22) */
    add_blocks(b12, b_stride, b22, b_stride, right_sum, half, 1);
    if (!strassen_recursive(a11, a_stride, right_sum, half, products[2], half, half)) goto failure;
    /* P4 = A22(B21 - B11) */
    add_blocks(b21, b_stride, b11, b_stride, right_sum, half, 1);
    if (!strassen_recursive(a22, a_stride, right_sum, half, products[3], half, half)) goto failure;
    /* P5 = (A11 + A12)B22 */
    add_blocks(a11, a_stride, a12, a_stride, left_sum, half, 0);
    if (!strassen_recursive(left_sum, half, b22, b_stride, products[4], half, half)) goto failure;
    /* P6 = (A21 - A11)(B11 + B12) */
    add_blocks(a21, a_stride, a11, a_stride, left_sum, half, 1);
    add_blocks(b11, b_stride, b12, b_stride, right_sum, half, 0);
    if (!strassen_recursive(left_sum, half, right_sum, half, products[5], half, half)) goto failure;
    /* P7 = (A12 - A22)(B21 + B22) */
    add_blocks(a12, a_stride, a22, a_stride, left_sum, half, 1);
    add_blocks(b21, b_stride, b22, b_stride, right_sum, half, 0);
    if (!strassen_recursive(left_sum, half, right_sum, half, products[6], half, half)) goto failure;

    /* C11 = P1 + P4 - P5 + P7; C12 = P3 + P5. */
    /* C21 = P2 + P4; C22 = P1 - P2 + P3 + P6. */
    for (size_t row = 0; row < half; ++row) {
        for (size_t col = 0; col < half; ++col) {
            const size_t index = row * half + col;
            out[row * out_stride + col] = products[0][index] + products[3][index] - products[4][index] + products[6][index];
            out[row * out_stride + col + half] = products[2][index] + products[4][index];
            out[(row + half) * out_stride + col] = products[1][index] + products[3][index];
            out[(row + half) * out_stride + col + half] = products[0][index] - products[1][index] + products[2][index] + products[5][index];
        }
    }
    for (size_t i = 0; i < 7; ++i) free(products[i]);
    free(left_sum); free(right_sum);
    return 1;

failure:
    for (size_t i = 0; i < 7; ++i) free(products[i]);
    free(left_sum); free(right_sum);
    return 0;
}

static int next_power_of_two(size_t value, size_t *power) {
    size_t result = 1;
    while (result < value) {
        if (result > SIZE_MAX / 2) return 0;
        result *= 2;
    }
    *power = result;
    return 1;
}

int matrix_multiply_strassen(const Matrix *a, const Matrix *b, Matrix *result) {
    Matrix product = {0, 0, NULL};
    int64_t *left = NULL, *right = NULL, *padded = NULL;
    size_t largest, size, count;
    if (result == NULL || !valid_multiplication(a, b)) return 0;
    largest = a->rows > a->cols ? a->rows : a->cols;
    if (b->rows > largest) largest = b->rows;
    if (b->cols > largest) largest = b->cols;
    if (!next_power_of_two(largest, &size) || !element_count(size, size, &count) ||
        count > SIZE_MAX / sizeof(*left) || !allocate_elements(count, &left) ||
        !allocate_elements(count, &right) || !allocate_elements(count, &padded) ||
        !matrix_init(&product, a->rows, b->cols)) {
        free(left); free(right); free(padded);
        return 0;
    }
    memset(left, 0, count * sizeof(*left));
    memset(right, 0, count * sizeof(*right));
    memset(padded, 0, count * sizeof(*padded));
    for (size_t row = 0; row < a->rows; ++row)
        memcpy(left + row * size, a->data + row * a->cols, a->cols * sizeof(*left));
    for (size_t row = 0; row < b->rows; ++row)
        memcpy(right + row * size, b->data + row * b->cols, b->cols * sizeof(*right));
    if (!strassen_recursive(left, size, right, size, padded, size, size)) {
        free(left); free(right); free(padded); matrix_free(&product); return 0;
    }
    for (size_t row = 0; row < product.rows; ++row)
        memcpy(product.data + row * product.cols, padded + row * size, product.cols * sizeof(*padded));
    free(left); free(right); free(padded);
    matrix_free(result);
    *result = product;
    return 1;
}
