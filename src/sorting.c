#include "sorting.h"

#include <stdint.h>
#include <stdlib.h>

static void merge(int64_t *values, int64_t *buffer,
                         size_t begin, size_t middle, size_t end) {
    size_t left = begin;
    size_t right = middle;
    size_t output = begin;

    while (left < middle && right < end) {
        if (values[left] <= values[right]) {
            buffer[output++] = values[left++];
        } else {
            buffer[output++] = values[right++];
        }
    }
    while (left < middle) buffer[output++] = values[left++];
    while (right < end) buffer[output++] = values[right++];
    for (size_t i = begin; i < end; ++i) values[i] = buffer[i];
}

static void merge_sort_range(int64_t *values, int64_t *buffer,
                                 size_t begin, size_t end) {
    if (end - begin <= 1) return;

    const size_t middle = begin + (end - begin) / 2;
    merge_sort_range(values, buffer, begin, middle);
    merge_sort_range(values, buffer, middle, end);
    merge(values, buffer, begin, middle, end);
}

int merge_sort(int64_t *values, size_t length) {
    if (values == NULL) return length == 0;
    if (length <= 1) return 1;
    if (length > SIZE_MAX / sizeof(*values)) return 0;

    int64_t *buffer = malloc(length * sizeof(*buffer));
    if (buffer == NULL) return 0;
    merge_sort_range(values, buffer, 0, length);
    free(buffer);
    return 1;
}
