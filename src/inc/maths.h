//
// Created by Samuel Tebbs on 18/04/2018.
//

#ifndef JAQ_MATHS_H
#define JAQ_MATHS_H

#include <stdinc.h>

/**
 * Gets the sign of the argument
 * @param i The integer to get the sign of
 * @return -1 if i < 0, 0 if i == 0 else 1
 */
int8_t sign_i32(int32_t i);

/**
 * Get the maximum of the two uint32_t arguments
 * @param a
 * @param b
 * @return The maximum of a and b
 */
uint32_t max_u32(uint32_t a, uint32_t b);

/**
 * Get the minimum of the two uint32_t arguments
 * @param a
 * @param b
 * @return The minimim of a and b
 */
uint32_t min_u32(uint32_t a, uint32_t b);

/**
 * Get the maximum of the two size_t arguments
 * @param a
 * @param b
 * @return The maximum of a and b
 */
size_t max_size(size_t a, size_t b);

/**
 * Get the minimum of the two size_t arguments
 * @param a
 * @param b
 * @return The minimim of a and b
 */
size_t min_size(size_t a, size_t b);

/**
 * Calculates the floor of the base 2 logarithm of the integer
 * @param i The integer to log
 * @return The floor of the base 2 logarith of i
 */
uint32_t log2_floor(uint32_t i);

#endif //JAQ_MATHS_H
