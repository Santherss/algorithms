#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stddef.h>

typedef void (*BenchmarkFunction)(void *context);

/* Devuelve el tiempo promedio por ejecución, en segundos. */
double benchmark_seconds(BenchmarkFunction function, void *context,
                         size_t repetitions);
void benchmark_print_header(const char *title);
void benchmark_print_row(size_t size, double seconds);

#endif
