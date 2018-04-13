//
// Created by Samuel Tebbs on 11/04/2018.
//

#ifndef SAMIX_STDINT_H
#define SAMIX_STDINT_H

#define NULL 0x00

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

#define UINT8_MAX (uint8_t) -1
#define UINT16_MAX (uint16_t) -1
#define UINT32_MAX (uint32_t) -1
#define UINT64_MAX (uint64_t) -1

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

#define INT_MAX(bits) (1 << (bits - 1)) - 1
#define INT8_MAX INT_MAX(8)
#define INT16_MAX INT_MAX(16)
#define INT32_MAX INT_MAX(32)
#define INT64_MAX INT_MAX(64)

typedef uint32_t size_t;

#define SIZE_MAX UINT32_MAX

#endif //SAMIX_STDINT_H
