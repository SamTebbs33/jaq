//
// Created by Samuel Tebbs on 18/04/2018.
//

#include "util.h"
#include "maths.h"

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
