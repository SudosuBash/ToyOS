#ifndef _TOYOS_X86_PIC
#define _TOYOS_X86_PIC

#include <kernel/stdint.h>
#include <irq/irq.h>
#define APIC_BSP(info) (((info) >> 8) & 1)
#define APIC_EXTD(info) (((info) >> 10) & 1)
#define APIC_EN(info) (((info) >> 11) & 1)
#define APIC_BASE_MEM(info) ((info) >> 12)

#define APIC_ENABLE_EXTD(info) ((info) | (1 << 10))
#define APIC_ENABLE_EN(info) ((info) | (1 << 11))

#define APIC_SVR_OFFSET 0xf0
#define APIC_TPR_OFFSET 0x80
#define APIC_EOI_OFFSET 0xB0
#define APIC_DCR_OFFSET 0x3e0
#define APIC_LVT_OFFSET 0x320
#define APIC_INITIAL_COUNT_OFFSET 0x380

#define APIC_SVR_SOFTWARE_ENABLE_VALUE (1<<8) 
#define APIC_SVR_VALUE(ivt) ((ivt) | APIC_SVR_SOFTWARE_ENABLE_VALUE)

#define APIC_LVT_PERODIC_VALUE (1<<17)
//设置 LVT 为 Perodic
void init_apic();
#endif