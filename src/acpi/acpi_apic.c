#include <kernel/acpi/acpi_apic.h>
#include <kernel/cpu/smp.h>
#include <kernel/timer/timer.h>
#include <kernel/irq/irq.h>
#include <kernel/mm/mm.h>
#include <kernel/ptable/ptable.h>
#include <kernel/mm/mmap.h>
#include <kernel/acpi/acpi.h>
#include <kernel/acpi/acpi_hpet.h>
#include <kernel/base/math.h>
#include <kernel/log/kprintf.h>

static volatile uint64_t apic_vaddr;
extern struct system_static_data sysdata;

inline void lapic_write(uint32_t offset, uint32_t data) {
    *(uint32_t*)(apic_vaddr + offset) = data;
}

inline uint32_t lapic_read(uint32_t offset) {
    return *(uint32_t*)(apic_vaddr+offset);
}


void init_apic() {
    uint64_t apic_status = apic_current_info();
    uint64_t base_addr = APIC_BASE_MEM(apic_status);
    base_addr <<= 12;
    
    apic_vaddr = (uint64_t)iomap((void*)base_addr, PAGE_SZ);
}