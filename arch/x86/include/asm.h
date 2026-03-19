#ifndef _TOYOS_ASM
#define _TOYOS_ASM

#include <kernel/stdint.h>
#include <cpu/gdt.h>
#include <irq/irq.h>
#include <kernel/fault/fault.h>

#define ltr(selector) asm volatile ("ltr %0": :"r"((uint16_t)selector))

void outb(uint16_t port, uint8_t data);
void load_cr3(uint64_t paddr);
void open_cr0_wp();
void close_cr0_wp();
void lgdt(struct gdtr gdtr);
void lidt(struct idtr idtr);
uint64_t get_cr2();
void invlpg(uint64_t addr);
#endif