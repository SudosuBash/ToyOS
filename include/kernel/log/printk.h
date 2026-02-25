#ifndef _TOYOS_PRINTK
#define _TOYOS_PRINTK

#include <kernel/stdint.h>

size_t printk(const char* fmt, ...);
#endif