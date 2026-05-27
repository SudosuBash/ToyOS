#ifndef _TOYOS_ARCH_ABSTRACT
#define _TOYOS_ARCH_ABSTRACT

#include <hal/hal.h>
struct arch_regs;

uintptr_t get_pf_addr();
uintptr_t get_pgroot();
uint64_t read_pci_configuration(uint16_t bus, uint16_t device, uint16_t function, uint16_t reg, uint64_t width);
void write_pci_configuration(uint16_t bus, uint16_t device, uint16_t function, uint16_t reg, uint64_t width, uint64_t value);
//其他架构的页表刷新照样用这玩意
#endif