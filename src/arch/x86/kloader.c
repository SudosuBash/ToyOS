#include <pgtable.h>

#define KERNEL_LDR_ADDR 0x100000
//1. 准备分页
//2. 加载 内核 ELF
#include <elf.h>
extern int rd_disc(int rd_count, int sector, void* data);

static inline void memcpy(void* src, void* dst, int size) {
    for(int i=0;i<size;i++) {
        *(char*)dst = *(char*)src;
    }
}

int _start() {
    int r = rd_disc(5,5,(void*)KERNEL_LDR_ADDR);
    
    while(1);
    return 0;
}