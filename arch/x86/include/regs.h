#ifndef _TOYOS_REGS
#define _TOYOS_REGS

#include <kernel/stdint.h>

typedef struct {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint16_t ds;
    uint16_t es;
    uint16_t fs;
    uint8_t eflags;
    uint32_t eip;
    uint16_t cs;
    uint16_t esp;
    uint16_t ss;
} reg_t;
#endif