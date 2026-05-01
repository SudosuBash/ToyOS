#include <kernel/atomic/atomic.h>
#include <kernel/fault/fault.h>
#include <kernel/cpu/smp.h>

//Compare And Set
/**
 * 这个并不适用赋值, 因为它会修改失败, 需要 while 循环
 */
inline uint64_t atomic_cas(volatile uint64_t* dest,uint64_t src, uint64_t target) {
    uint64_t ret;
    barrier();
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

static inline uint64_t atomic_fas(volatile uint64_t* dest, uint64_t src) {
    uint64_t ret = src;
    barrier();
    asm volatile(
        "xchgq %[src], %[dest]"
        : [dest] "+m"(*dest),
          [src] "+r"(ret)
        :
        :"memory"
    );
    return ret;
}
void atomic_inc(atomic_t *val) {
    barrier();
    asm volatile (
        "lock incq %[value]\t\n"
        : [value] "+m"(val->count)
        :
        :"memory","cc"
    );
}

uint8_t atomic_dec_and_test(atomic_t* val) {
    uint8_t zf;
    barrier();
    asm volatile (
        "lock decq %[value]\t\n"
        "setz %[zf]"
        : [value] "+m" (val->count),
          [zf] "=q" (zf)
        //这部分必须用zf, 否则引入竞争条件
        :
        :"cc","memory"
    );
    return zf;
}

inline void atomic_set(atomic_t* val, uint64_t new) {
    atomic_fas(&val->count, new);
}
