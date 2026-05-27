#ifndef _TOYOS_X86_PIC
#define _TOYOS_X86_PIC

#include <kernel/stdint.h>
#include <hal/irq/irq.h>
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
#define APIC_ICR_LOW_OFFSET 0x300
#define APIC_ICR_HIGH_OFFSET 0x310
#define APIC_INITIAL_COUNT_OFFSET 0x380
#define APIC_CURRENT_COUNT_OFFSET 0x390

#define APIC_SVR_SOFTWARE_ENABLE_VALUE (1<<8) 
#define APIC_SVR_VALUE(ivt) ((ivt) | APIC_SVR_SOFTWARE_ENABLE_VALUE)

#define APIC_LVT_PERODIC_VALUE (1<<17)
#define APIC_ICR_LOW_DELIVERY_INIT (5 << 8)
#define APIC_ICR_LOW_DELIVERY_SIPI (6 << 8)
#define APIC_ICR_LOW_DEST_LOGICAL (1<<11)
#define APIC_ICR_LOW_LEVEL_ASSERT (1<<14)
#define APIC_ICR_LOW_TRIGGER_MODE_LEVEL (1<<15)

#define AP_START_ADDR 0x20000
//设置 LVT 为 Perodic
void init_apic();
void lapic_write(uint32_t offset, uint32_t data);
uint64_t apic_current_info();
void update_apic_info(uint64_t info);
uint32_t lapic_read(uint32_t offset);
#endif