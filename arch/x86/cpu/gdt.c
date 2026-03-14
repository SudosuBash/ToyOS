#include <asm.h>
#include <cpu/smp_percpu.h>
#include <kernel/stdlib.h>
#include <gdt.h>
#include <kernel/mm/mm.h>

DEFINE_PERCPU_VAR(gdts[4],struct gdt);

static struct gdt* gdt_sa;
static struct gdtr gdtr;
static struct tss* tss_sa;

struct gdt gdt = {
    .limit = 0xffff,
    .base = 0,
    .base_2 = 0,
    .access_byte = 0b10011010,
    .limit_2 = 0b1111,
    .flags = 0b0010,
    .base = 0
}; //Kernel Code

//为了彻底脱离kloader准备的临时环境, 什么都要重新写过
static inline void prepare_gdt() {
    *(uint64_t*)gdt_sa = 0; //第一个设置为0

    struct gdt* gaddr = (struct gdt*)(gdt_sa) + 1;

    *gaddr = gdt;
    gaddr+=1; //8 Kernel Data
    gdt.access_byte = 0b10010110;
    *gaddr = gdt;
    gaddr+=1; //16
    gdt.access_byte = 0b11111010;
    *gaddr = gdt;
    gaddr+=1; //24
    gdt.access_byte = 0b11110110;
    *gaddr = gdt;
    gaddr+=1; //32

    tss_sa = (struct tss*) gaddr;
    gdtr.limit += ((uintptr_t)tss_sa - (uintptr_t)gdt_sa);
}

void reload_gdt() {
    uint64_t limit = ((uintptr_t)tss_sa - (uintptr_t)gdt_sa);
    gdtr.limit = limit;
    gdtr.base = (uintptr_t)gdt_sa;
    barrier();
    lgdt(gdtr);
}

void init_gdt() {
    extern struct gdt __gdt_bottom;
    gdt_sa = &__gdt_bottom;
    prepare_gdt();
    reload_gdt(); 
}

