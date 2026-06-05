#include <kernel/cpu/smp.h>
#include <kernel/stdint.h>
#include <kernel/mm/mm.h>
#include <kernel/fault/fault.h>
#include <kernel/log/kprintf.h>
#include <kernel/atomic/atomic.h>
#include <kernel/config.h>
#include <kernel/cpu/archimpl.h>
#include <kernel/kernel.h>
#include <kernel/acpi/acpi.h>

DEFINE_PERCPU_VAR(percpu_preempt_count, atomic_t);

extern struct system_static_data sysdata;
struct ap_resource_pack* ap_respack;

inline void preempt_enable() {
    barrier();
    atomic_t* preempt_count = THIS_CPU_PTR(percpu_preempt_count);
    atomic_dec_and_test(preempt_count);
}

inline void preempt_disable() {
    atomic_t* preempt_count = THIS_CPU_PTR(percpu_preempt_count);
    atomic_inc(preempt_count);
    barrier();
}

void ap_boot() {
    uint64_t core = sysdata.smp_info->local_apic_count;
    ap_respack = (struct ap_resource_pack*)early_kmalloc(sizeof(struct ap_resource_pack));
    struct ap_necessary_resource* ap = (struct ap_necessary_resource*)PHYS2VADDR_MMIO(AP_REAL_MODE_DATA_ADDR);
    ap->pgroot = get_pgroot();
    for(int i=1;i<core;i++) {
        ap_respack[i].percpu_address = sysdata.percpu_start_addr + i * CONFIG_PERCPU_SZ;
        ap_respack[i].rsp = sysdata.ap_sp + (i-1) * CONFIG_PERCPU_SZ;
        ap_respack[i].cpuid = sysdata.smp_info->localapic[i]->Id;
        ap_respack[i].ready = 0;
        arch_ap_boot(sysdata.smp_info->localapic[i]->Id);
    }
    
}

void init_smp() {
    struct smp_data_acpi_info* info = sysdata.smp_info;
    kprintf("CPU: Found %d core(s) on this machine.\n",info->local_apic_count);
    set_smp_percpu_addr(sysdata.percpu_start_addr);
    ap_boot();
    atomic_t* preempt_count = THIS_CPU_PTR(percpu_preempt_count);
    atomic_set(preempt_count, 0);
}