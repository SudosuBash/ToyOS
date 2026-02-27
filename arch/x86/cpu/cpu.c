#include <cpu/cpu.h>
#include <asm.h>
#include <mem.h>
#include <cpu/smp_percpu.h>
#include <kloader/kloader.h>
DEFINE_PERCPU_VAR(cpuinfo, struct cpuinfo);

extern struct boot_info* bl;

static inline struct gdtr get_gdtr(uint64_t paddr) {
    struct gdtr gdtr;
    gdtr.base = PHYS2VADDR(paddr);
    //gdtr.limit = 0; //我至今都想不通这是怎么通过的
    gdtr.limit = bl->gdtr_limit;
    return gdtr;
}
void init_cpu() {
    struct gdtr gdtr = get_gdtr(bl->gdt_paddr);
    lgdt(gdtr);
    open_cr0_wp();
}