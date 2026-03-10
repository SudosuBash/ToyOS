#ifndef _TOYOS_X86_MM_INFO
#define _TOYOS_X86_MM_INFO

#include <kernel/stdint.h>
uint64_t get_available_mem_sz();
void init_mem();
#endif