#ifndef _TOYOS_X86_SMP_PERCPU
#define _TOYOS_X86_SMP_PERCPU

#include <kernel/atomic/atomic.h>
#include <kernel/stdint.h>
#include <hal/hal.h>

#define DEFINE_PERCPU_VAR(name, type) __attribute__((section(".percpu"))) type name;
#define DECLARE_PERCPU_VAR(name, type) extern __attribute__((section(".percpu"))) type name;

#define THIS_CPU_PTR(var) ARCH_GET_PERCPU_VAR(var)
#define THIS_CPU_VAR(var) *(THIS_CPU_PTR(var))
#define SET_THIS_CPU_VAR(name, value) ARCH_SET_THIS_CPU_VAR(name, value)

#define READ_ONCE(var) (*(volatile typeof((var))*)&(var))
#define WRITE_ONCE(var, val) (*(volatile typeof((var))*)&(var)) = (val);

struct acpi_madt_local_apic; 
typedef struct acpi_madt_local_apic ACPI_MADT_LOCAL_APIC;
struct acpi_madt_interrupt_override; 
typedef struct acpi_madt_interrupt_override ACPI_MADT_INTERRUPT_OVERRIDE;
struct acpi_madt_io_apic; 
typedef struct acpi_madt_io_apic ACPI_MADT_IO_APIC;

struct smp_data_acpi_info {
    uint64_t irq_override_count;
    uint64_t local_apic_count;
    uint64_t io_apic_count;

    ACPI_MADT_LOCAL_APIC* localapic[MAX_SUPPORTED_CORES];
    ACPI_MADT_INTERRUPT_OVERRIDE* override[MAX_IRQ_OVERRIDE_ENTRIES];
    ACPI_MADT_IO_APIC* ioapic[MAX_IO_APICS];

    atomic_t cpu_ready_count;
} __attribute__((aligned(64)));

struct ap_resource_pack {
    uint64_t rsp;
    uint64_t percpu_address;
    uint64_t cpuid;
    uint8_t ready;
};
struct ap_necessary_resource {
    uint64_t pgroot;
}__attribute__((packed));


void set_smp_percpu_addr(uintptr_t base);
void preempt_enable();
void preempt_disable();
void arch_ap_boot(uint8_t cpuid);

void cpu_prepared();
#endif