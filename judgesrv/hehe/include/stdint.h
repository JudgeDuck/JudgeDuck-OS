#pragma once

#ifndef NULL
#define NULL ((void*) 0)
#endif

typedef __signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

typedef int32_t intptr_t;
typedef uint32_t uintptr_t;

typedef uint32_t size_t;
typedef int32_t ssize_t;

#define offsetof(type, member)  ((size_t) (&((type*)0)->member))
