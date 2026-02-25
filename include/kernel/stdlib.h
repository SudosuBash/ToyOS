#ifndef _TOYOS_STDLIB
#define _TOYOS_STDLIB

#include <kernel/stdint.h>

typedef uint32_t size_t;

void memset(void* addr,uint8_t val, size_t size);

void hex_to_str(uint64_t n,char* buf);
void hex_to_dec(uint64_t num, char* buf);
#endif