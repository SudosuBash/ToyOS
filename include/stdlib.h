#ifndef _TOYOS_STDLIB
#define _TOYOS_STDLIB

#include <stdint.h>

typedef uint32_t size_t;

void memset(void* addr,uint8_t val, size_t size);
#endif