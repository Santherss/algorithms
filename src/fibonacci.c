#include "algorithms.h"

uint64_t fibonacci_linear(uint64_t n){
    uint64_t previous = 0;
    uint64_t current = 1;

    for (uint64_t i = 0; i < n; ++i) {
        const uint64_t next = previous + current;
        previous = current;
        current = next;
    }
    return previous;
}

static void fibonacci_pair(uint64_t n, uint64_t *fn, uint64_t *fn_plus_one){
    if (n == 0) {
        *fn = 0;
        *fn_plus_one = 1;
        return;
    }

    uint64_t a;
    uint64_t b;
    fibonacci_pair(n / 2, &a, &b);

    const uint64_t c = a * (2 * b - a);
    const uint64_t d = a * a + b * b;
    if ((n & 1U) == 0) {
        *fn = c;
        *fn_plus_one = d;
    } else {
        *fn = d;
        *fn_plus_one = c + d;
    }
}

uint64_t fibonacci_logarithmic(uint64_t n){
    uint64_t result;
    uint64_t next;
    fibonacci_pair(n, &result, &next);
    return result;
}
