#ifndef _TOYOS_ASM
#define _TOYOS_ASM

#include <kernel/stdint.h>
#include <gdt.h>
#include <irq/irq.h>
#include <kernel/fault/fault.h>

#define CS_SELECTOR 0x08

void outb(uint16_t port, uint8_t data);

void load_cr3(uint64_t paddr);
void open_cr0_wp();
void close_cr0_wp();
void lgdt(struct gdtr gdtr);
void lidt(struct idtr idtr);
uint64_t get_cr2();
#endif