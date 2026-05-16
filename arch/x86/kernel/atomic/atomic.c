#include <kernel/atomic/atomic.h>
#include <kernel/fault/fault.h>
#include <kernel/cpu/smp.h>

//Compare And Set
/**
 * * 这个并不适用赋值, 因为它会修改失败, 需要 while 循环
 * * 成功是1，失败是0
 */
inline uint64_t atomic_cas(uint64_t* dest,int64_t src, int64_t target) {
    uint8_t ret;
    barrier();
    asm volatile (
        "lock cmpxchgq %[src],%[dest]\n\t"
        "sete %[r]\n\t"
        :[dest] "+m"(*dest),
         [r] "=q"(ret)
        : [src]"r"(src),
         "a" (target)
        : "memory","cc"
    );
    return ret;
}

static inline uint64_t atomic_fas(uint64_t* dest, int64_t src) {
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

inline void atomic_set(atomic_t* val, int64_t new) {
    atomic_fas(&val->count, new);
}
