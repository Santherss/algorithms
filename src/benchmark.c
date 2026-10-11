#define _POSIX_C_SOURCE 200809L

#include "benchmark.h"

#include <stdio.h>
#include <time.h>

double benchmark_seconds(BenchmarkFunction function, void *context,
                         size_t repetitions) {
    struct timespec start;
    struct timespec end;

    if (function == NULL || repetitions == 0) return 0.0;

    (void)clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < repetitions; ++i) {
        function(context);
    }
    (void)clock_gettime(CLOCK_MONOTONIC, &end);

    const double elapsed = (double)(end.tv_sec - start.tv_sec);
    const double nanoseconds = (double)(end.tv_nsec - start.tv_nsec) / 1e9;
    return (elapsed + nanoseconds) / (double)repetitions;
}

void benchmark_print_header(const char *title) {
    printf("\n%s\n", title);
    printf("size,time_seconds\n");
}

void benchmark_print_row(size_t size, double seconds) {
    printf("%zu,%.9f\n", size, seconds);
}
