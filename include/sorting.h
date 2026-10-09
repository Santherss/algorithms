#ifndef SORTING_H
#define SORTING_H

#include <stddef.h>
#include <stdint.h>

/* Ordena values de forma ascendente y estable. Devuelve 1 si tuvo éxito. */
int merge_sort(int64_t *values, size_t length);

#endif
