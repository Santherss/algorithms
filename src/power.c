#include "algorithms.h"

uint64_t power_linear(uint64_t base, uint64_t exponent) {
    uint64_t result = 1;

    for (uint64_t i = 0; i < exponent; ++i) {
        result *= base;
    }
    return result;
}

uint64_t power_logarithmic(uint64_t base, uint64_t exponent) {
    uint64_t result = 1;

    while (exponent != 0) {
        if ((exponent & 1U) != 0) {
            result *= base;
        }
        base *= base;
        exponent >>= 1;
    }
    return result;
}
