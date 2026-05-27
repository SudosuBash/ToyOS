#include <kernel/acpi/acpi_hpet.h>
#include <kernel/mm/mmap.h>
#include <kernel/base/math.h>

extern struct system_static_data sysdata;
static uint64_t hpet_freq;
inline uint64_t hpet_current_time() {
    return *(volatile uint64_t*)(sysdata.acpi_data.hpet_vaddr+ACPI_HPET_COUNTER_OFFSET);
}

inline uint32_t hpet_period() {
    uint64_t cap = *(volatile uint64_t*)(sysdata.acpi_data.hpet_vaddr+ACPI_HPET_PERIOD_OFFSET);
    return cap >> 32;
}

inline uint64_t hpet_gcfg() {
    return *(volatile uint64_t*)(sysdata.acpi_data.hpet_vaddr+ACPI_HPET_GCFG_OFFSET);
}

inline void hpet_write(uint64_t off, uint64_t value) {
    *(volatile uint64_t*)(sysdata.acpi_data.hpet_vaddr+off) = value;
}

void acpi_hpet_init() {
    ACPI_TABLE_HPET* hpet = get_acpi_hpet(sysdata.acpi_data.rsdp);
    uint64_t hpet_addr = hpet->Address.Address;
    sysdata.acpi_data.hpet_vaddr = (uint64_t)iomap((void*)hpet_addr, PAGE_SZ);

    uint64_t conf = hpet_gcfg();
    conf |= 1;
    hpet_write(ACPI_HPET_GCFG_OFFSET, conf);
    uint32_t period = hpet_period();
    hpet_freq = div_32bit(1000000000, period);
}

void hpet_spin_wait(uint64_t useconds) {
    uint64_t current = hpet_current_time();
    while(hpet_current_time() - current < hpet_freq);
}