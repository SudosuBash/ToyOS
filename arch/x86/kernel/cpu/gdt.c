#include <asm.h>
#include <kernel/cpu/smp.h>
#include <kernel/stdlib.h>
#include <cpu/gdt.h>
#include <kernel/cpu/smp.h>

DEFINE_PERCPU_VAR(gdts[5],union x86_gdt_tss_desc);
DEFINE_PERCPU_VAR(tss, struct tss);

static struct gdtr gdtr;
static struct gdt_desc gdt_desc = {
    .limit = 0xffff,
    .base_1 = 0,
    .base_2 = 0,
    .access_byte = 0b10011010,
    .limit_2 = 0b1111,
    .flags = 0b0010,
    .base_3 = 0
}; //Kernel Code

static struct tss_gdt_desc tss_gdt_desc = {
    .desc.limit = 0xffff,
    .desc.base_1 = 0,
    .desc.base_2 = 0,
    .desc.access_byte = 0b10001001,
    .desc.limit_2 = 0b1111,
    .desc.flags = 0b0010,
    .desc.base_3 = 0,
    .high_32 = 0,
    .reserved = 0
}; //Kernel Code

inline void set_tss_rsp_r0(uintptr_t addr) {
    struct tss* cpu_tss = THIS_CPU_PTR(tss);
    cpu_tss->rsp_ring0 = addr;
}

inline void set_tss_ist(uint8_t index,uint64_t addr) {
    if(index >= 7) return;
    struct tss* cpu_tss = THIS_CPU_PTR(tss);
    cpu_tss->ist[index] = addr;
    barrier();
}

//为了彻底脱离kloader准备的临时环境, 什么都要重新写过
static inline void prepare_gdt() {
    union x86_gdt_tss_desc* cpu_desc = THIS_CPU_VAR(gdts);
    cpu_desc[0].gdts[1] = cpu_desc[1].gdts[0] = cpu_desc[1].gdts[1] = cpu_desc[2].gdts[0] = gdt_desc;

    cpu_desc[0].gdts[1].access_byte = 0b10011010;
    cpu_desc[1].gdts[0].access_byte = 0b10010110;
    cpu_desc[1].gdts[1].access_byte = 0b11110110;
    cpu_desc[2].gdts[0].access_byte = 0b11111010;

    cpu_desc[3].tss = tss_gdt_desc;

    struct tss* cpu_tss = THIS_CPU_PTR(tss);
    uintptr_t tss_addr = (uintptr_t) cpu_tss;
    cpu_desc[3].tss.desc.base_1 = tss_addr & 0xffff;
    cpu_desc[3].tss.desc.base_2 = (tss_addr >> 16) & 0xff;
    cpu_desc[3].tss.desc.base_3 = (tss_addr >> 24) & 0xff;
    cpu_desc[3].tss.high_32 = (tss_addr >> 32);
}

void reload_gdt() {
    union x86_gdt_tss_desc* cpu_gdt = THIS_CPU_VAR(gdts);
    gdtr.limit = sizeof(gdts) - 1;
    gdtr.base = (uintptr_t)cpu_gdt;
    barrier();
    lgdt(gdtr);
}

void load_tss() {
    barrier();
    ltr(KERNEL_TSS);
}

void init_gdt() {
    prepare_gdt();
    reload_gdt(); 
    load_tss();
}