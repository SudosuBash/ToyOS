#ifndef _TOYOS_STDLIB
#define _TOYOS_STDLIB

#include <kernel/stdint.h>

void memset(void* addr,uint8_t val, size_t size);
void memcpy(void* dst, void* src, size_t sz);
void hex_to_str(uint64_t n,char* buf,int leng);
void hex_to_dec(uint64_t num, char* buf);

size_t strlen(char* c);
uint8_t strcmp(char* c1, char* c2);
uint8_t strcmp_l(char* c1, char* c2, size_t sz);
#endif