#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "algorithms.h"
#include "benchmark.h"
#include "sorting.h"

enum { REPETITIONS = 3 };

typedef struct {
    uint64_t n;
    volatile uint64_t sink;
} ScalarContext;

typedef struct {
    int64_t *values;
    size_t length;
    volatile int64_t sink;
} SortContext;

typedef struct {
    Matrix *a;
    Matrix *b;
    Matrix *result;
    int use_strassen;
    volatile int64_t sink;
} MatrixContext;

static void run_power_linear(void *raw) {
    ScalarContext *context = raw;
    context->sink = power_linear(UINT64_C(3), context->n);
}

static void run_power_logarithmic(void *raw) {
    ScalarContext *context = raw;
    context->sink = power_logarithmic(UINT64_C(3), context->n);
}

static void run_fibonacci_linear(void *raw) {
    ScalarContext *context = raw;
    context->sink = fibonacci_linear(context->n);
}

static void run_fibonacci_logarithmic(void *raw) {
    ScalarContext *context = raw;
    context->sink = fibonacci_logarithmic(context->n);
}

static void run_merge_sort(void *raw) {
    SortContext *context = raw;
    for (size_t i = 0; i < context->length; ++i) {
        context->values[i] = (int64_t)((context->length - i) % 100003U);
    }
    (void)merge_sort(context->values, context->length);
    context->sink = context->values[context->length / 2];
}

static void run_matrix(void *raw) {
    MatrixContext *context = raw;
    if (context->use_strassen) {
        (void)matrix_multiply_strassen(context->a, context->b, context->result);
    } else {
        (void)matrix_multiply_classic(context->a, context->b, context->result);
    }
    context->sink = context->result->data[0];
}

static void print_scalar_series(const char *name, const uint64_t *sizes,
                                size_t count, BenchmarkFunction function) {
    for (size_t i = 0; i < count; ++i) {
        ScalarContext context = {sizes[i], 0};
        const double seconds = benchmark_seconds(function, &context, REPETITIONS);
        printf("%s,%llu,%.9f\n", name,
               (unsigned long long)sizes[i], seconds);
    }
}

static int print_sort_series(void) {
    static const size_t sizes[] = {1000, 10000, 100000, 1000000};
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i) {
        int64_t *values = malloc(sizes[i] * sizeof(*values));
        if (values == NULL) return 0;
        SortContext context = {values, sizes[i], 0};
        const double seconds = benchmark_seconds(run_merge_sort, &context, REPETITIONS);
        printf("merge_sort,%zu,%.9f\n", sizes[i], seconds);
        free(values);
    }
    return 1;
}

static int print_matrix_series(const char *name, int use_strassen) {
    static const size_t sizes[] = {16, 32, 64, 128};
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i) {
        Matrix a = {0, 0, NULL};
        Matrix b = {0, 0, NULL};
        Matrix result = {0, 0, NULL};
        if (!matrix_init(&a, sizes[i], sizes[i]) ||
            !matrix_init(&b, sizes[i], sizes[i])) {
            matrix_free(&a);
            matrix_free(&b);
            return 0;
        }
        matrix_fill_deterministic(&a, 1);
        matrix_fill_deterministic(&b, 2);
        MatrixContext context = {&a, &b, &result, use_strassen, 0};
        const double seconds = benchmark_seconds(run_matrix, &context, REPETITIONS);
        printf("%s,%zu,%.9f\n", name, sizes[i], seconds);
        matrix_free(&a);
        matrix_free(&b);
        matrix_free(&result);
    }
    return 1;
}

int main(void) {
    static const uint64_t scalar_sizes[] = {10, 100, 1000, 10000, 100000, 1000000};

    puts("algorithm,n,time_seconds");
    print_scalar_series("power_linear", scalar_sizes,
                        sizeof(scalar_sizes) / sizeof(scalar_sizes[0]), run_power_linear);
    print_scalar_series("power_logarithmic", scalar_sizes,
                        sizeof(scalar_sizes) / sizeof(scalar_sizes[0]), run_power_logarithmic);
    print_scalar_series("fibonacci_linear", scalar_sizes,
                        sizeof(scalar_sizes) / sizeof(scalar_sizes[0]), run_fibonacci_linear);
    print_scalar_series("fibonacci_logarithmic", scalar_sizes,
                        sizeof(scalar_sizes) / sizeof(scalar_sizes[0]), run_fibonacci_logarithmic);
    if (!print_sort_series() || !print_matrix_series("matrix_classic", 0) ||
        !print_matrix_series("matrix_strassen", 1)) {
        fputs("No se pudo reservar memoria para el benchmark.\n", stderr);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
