#ifndef _TOYOS_STDINT
#define _TOYOS_STDINT

typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;
typedef int int32_t;

typedef long intptr_t;
typedef unsigned long uintptr_t;

typedef unsigned long long uint64_t;
typedef long long int64_t;

typedef uintptr_t size_t;

typedef int64_t stat_t;

typedef uint8_t bool;
typedef uint64_t uintmax_t;
typedef int64_t intmax_t;

typedef int32_t wchar_t;
#define true 1
#define false 0

#endif