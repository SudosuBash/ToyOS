#include <asm.h>
#include <kernel/atomic/atomic_type.h>

inline void outb(uint16_t port, uint8_t data) {
    asm volatile(
        "outb %0,%1"
        :
        : "a"(data), "Nd"(port)
    );
}

void atomic_inc(atomic_t *val) {
    asm volatile (
        "lock inc %0"
        : "+m" (val->count)
        :
        :"cc"
    );
}

uint8_t atomic_dec_and_test(atomic_t* val) {
    uint8_t zf;
    asm volatile (
        "lock dec %0\t\n"
        "setz %1"
        : "+m" (val->count),
          "=q" (zf)
        //这部分必须用zf, 否则引入竞争条件
        :
        :"cc"
    );
    return zf;
}