#include <irq/pic/apic.h>
#include <pgtable/pgtable_kern.h>
#include <config_arch.h>
#include <asm.h>
#include <kernel/cpu/smp.h>
#include <kernel/irq/timer.h>
#include <kernel/irq/irq.h>
#include <kernel/mm/mm.h>
#include <cpu/msr_base.h>

static volatile uint64_t apic_status;
static volatile uint64_t apic_vaddr;

static volatile timer_handler_t handler;


uint8_t is_bsp_core() {
    return APIC_BSP(apic_status);
}

static inline uint64_t apic_current_info() {
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

static inline void update_apic_info(uint64_t info) {
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

static inline void enable_x2apic() {
    apic_status = APIC_ENABLE_EXTD(apic_status);
    update_apic_info(apic_status);
}


static inline void enable_apic() {
    apic_status = APIC_ENABLE_EN(apic_status);
    update_apic_info(apic_status);
}

static inline void lapic_write(uint32_t offset, uint32_t data) {
    *(uint32_t*)(apic_vaddr + offset) = data;
}

static inline uint32_t lapic_read(uint32_t offset) {
    return *(uint32_t*)(apic_vaddr+offset);
}

static inline void write_ivt(uint8_t ivt) {
    lapic_write(APIC_SVR_OFFSET,APIC_SVR_VALUE(ivt));
}

static inline void init_tpr() {
    lapic_write(APIC_TPR_OFFSET, 0);
}

static inline void init_dcr() {
    lapic_write(APIC_DCR_OFFSET, APIC_FREQ_NUM);
}

static inline void init_freq() {
    lapic_write(APIC_INITIAL_COUNT_OFFSET, APIC_FREQ_TEMP);
}

static inline void init_lvt() {
    lapic_write(APIC_LVT_OFFSET, (APIC_LVT_PERODIC_VALUE) | APIC_LVT_IVT);
}

static inline void lapic_handle_ok() {
    lapic_write(APIC_EOI_OFFSET, 0);
}

static inline void default_timer_irq_fn(struct arch_regs* frame) {
    fault_irq("Timer IRQ Triggered!",frame);
}

static inline void timer_irq_reg_fn(struct arch_regs* frame) {
    lapic_handle_ok();
    handler(frame);
}

void timer_irq_register(timer_handler_t fn) {
    handler = fn;
}

void init_apic() {

    apic_status = apic_current_info();
    uint64_t base_addr = APIC_BASE_MEM(apic_status);
    base_addr <<= 12;
    link_new_pte_addr(base_addr,PHYS2VADDR_MMIO(base_addr));
    apic_vaddr = PHYS2VADDR_MMIO(base_addr);
    set_pte_pcd(apic_vaddr,0);
    set_pte_pwt(apic_vaddr,0);

    write_ivt(APIC_IVT);

    init_tpr();
    init_dcr();
    init_lvt();
    init_freq();

    irq_register(APIC_LVT_IVT, timer_irq_reg_fn);
    timer_irq_register(default_timer_irq_fn);
}