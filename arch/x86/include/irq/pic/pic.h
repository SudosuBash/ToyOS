#ifndef _TOYOS_X86_PIC
#define _TOYOS_X86_PIC

#include <kernel/stdint.h>

uint8_t apic_support();
void init_pic();
#endif