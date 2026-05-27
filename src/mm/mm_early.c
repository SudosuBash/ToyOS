#include <kernel/mm/mm.h>
#include <kernel/kernel.h>

extern struct system_static_data sysdata;

static uint64_t temp_start;
void* early_kmalloc(size_t sz) {
   void* addr = (void*)temp_start;
   temp_start+=sz;
   return addr;
}

inline void init_mm_early() {
    temp_start = sysdata.kernel_end;
}