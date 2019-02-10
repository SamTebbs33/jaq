//
// Created by Samuel Tebbs on 18/04/2018.
//

#include <lib/util.h>
#include <lib/maths.h>

int8_t sign_i32(int32_t i) {
    return (int8_t) (i < 0 ? -1 : (i == 0 ? 0 : 1));
}

uint32_t max_u32(uint32_t a, uint32_t b) {
    return a > b ? a : b;
}

uint32_t min_u32(uint32_t a, uint32_t b) {
    return a > b ? b : a;
}

size_t max_size(size_t a, size_t b) {
    return max_u32(a, b);
}

size_t min_size(size_t a, size_t b) {
    return min_u32(a, b);
}

// Source: https://stackoverflow.com/questions/11376288/fast-computing-of-log2-for-64-bit-integers
uint32_t log2_floor(uint32_t n) {
    #define S(k) if (n >= (UINT32_C(1) << k)) { i += k; n >>= k; }
    int i = -(n == 0); S(16); S(8); S(4); S(2); S(1); return i;
    #undef S
}
