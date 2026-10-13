#include <assert.h>
#include <stdio.h>

#include "algorithms.h"
#include "sorting.h"

static void test_merge_sort(void) {
    int64_t values[] = {7, -2, 7, 0, 4, -9, 3, 1};
    const int64_t expected[] = {-9, -2, 0, 1, 3, 4, 7, 7};

    assert(merge_sort(values, sizeof(values) / sizeof(values[0])));
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
        assert(values[i] == expected[i]);
    assert(merge_sort(NULL, 0));
    assert(merge_sort(NULL, 1) == 0);
}

static void test_matrix_multiplication(void) {
    Matrix a = {0, 0, NULL};
    Matrix b = {0, 0, NULL};
    Matrix classic = {0, 0, NULL};
    Matrix strassen = {0, 0, NULL};
    assert(matrix_init(&a, 3, 2));
    assert(matrix_init(&b, 2, 4));
    for (size_t i = 0; i < a.rows * a.cols; ++i) a.data[i] = (int64_t)i + 1;
    for (size_t i = 0; i < b.rows * b.cols; ++i) b.data[i] = (int64_t)i - 2;
    assert(matrix_multiply_classic(&a, &b, &classic));
    assert(matrix_multiply_strassen(&a, &b, &strassen));
    assert(classic.rows == strassen.rows && classic.cols == strassen.cols);
    for (size_t i = 0; i < classic.rows * classic.cols; ++i)
        assert(classic.data[i] == strassen.data[i]);
    matrix_free(&a);
    matrix_free(&b);
    matrix_free(&classic);
    matrix_free(&strassen);

    Matrix large_a = {0, 0, NULL};
    Matrix large_b = {0, 0, NULL};
    Matrix large_classic = {0, 0, NULL};
    Matrix large_strassen = {0, 0, NULL};
    assert(matrix_init(&large_a, 33, 33));
    assert(matrix_init(&large_b, 33, 33));
    matrix_fill_deterministic(&large_a, 1);
    matrix_fill_deterministic(&large_b, 2);
    assert(matrix_multiply_classic(&large_a, &large_b, &large_classic));
    assert(matrix_multiply_strassen(&large_a, &large_b, &large_strassen));
    for (size_t i = 0; i < large_a.rows * large_a.cols; ++i)
        assert(large_classic.data[i] == large_strassen.data[i]);
    matrix_free(&large_a);
    matrix_free(&large_b);
    matrix_free(&large_classic);
    matrix_free(&large_strassen);
}

int main(void) {
    const Matrix2x2 fibonacci_base = {{{1, 1}, {1, 0}}};
    const Matrix2x2 fibonacci_power = matrix2x2_power(&fibonacci_base, 10);

    assert(power_linear(3, 5) == 243);
    assert(power_logarithmic(3, 5) == 243);
    assert(fibonacci_linear(20) == 6765);
    assert(fibonacci_logarithmic(20) == 6765);
    assert(fibonacci_power.value[0][1] == 55);
    test_merge_sort();
    test_matrix_multiplication();

    puts("Todas las pruebas pasaron.");
    return 0;
}
