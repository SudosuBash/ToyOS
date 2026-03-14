#include <kernel/atomic/atomic.h>
#include <kernel/fault/fault.h>
//Compare And Set
inline uint64_t atomic_cas(volatile uint64_t* dest,uint64_t src, uint64_t target) {
    uint64_t ret;
    asm volatile (
        "cmpxchgq %[src],%[dest]"
        :[dest] "+m"(*dest),
         "=a" (ret)
        : [src]"r"(src),
         "a" (target)
        : "memory","cc"
    );
    return ret;
}

void atomic_inc(atomic_t *val) {
    asm volatile (
        "lock incq %[value]\t\n"
        : [value] "=m"(val->count)
        :
        :"memory","cc"
    );
}

uint8_t atomic_dec_and_test(atomic_t* val) {
    uint8_t zf;
    asm volatile (
        "lock decq %[value]\t\n"
        "setz %[zf]"
        : [value] "+m" (val->count),
          [zf] "=q" (zf)
        //这部分必须用zf, 否则引入竞争条件
        :
        :"cc","memory"
    );
    return !zf;
}

