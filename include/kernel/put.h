#ifndef _TOYOS_PUT
#define _TOYOS_PUT
#include <kernel/stdint.h>
void put_char(char c);
void put_str(const char* c);
void put_dec(uint64_t n);
void put_hex(uint64_t n);
void clear_screen();
#endif