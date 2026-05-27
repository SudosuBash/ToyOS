#include <kernel/acpi/acpi_apic.h>
#include <hal/cpu/msr_base.h>

inline uint64_t apic_current_info() {
    uint32_t edx,eax;
    uint32_t ecx = MSR_APIC_BASE_ADDR;
    barrier();
    asm volatile (
        "rdmsr"
        :"=d"(edx),
         "=a"(eax) 
        : "c"(ecx): );
    return ((uint64_t)edx << 32) | (eax);
}


inline void update_apic_info(uint64_t info) {
    uint32_t ecx = MSR_APIC_BASE_ADDR;
    uint32_t eax = info & 0xffffffff, edx = info >> 32;
    barrier();
    asm volatile (
        "wrmsr"
        :
        :"c"(ecx),
        "d"(edx),
        "a"(eax)
    );
}
