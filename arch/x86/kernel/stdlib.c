#include <kernel/stdlib.h>

inline void memset(void* addr,int32_t val, size_t size) {
    asm volatile(
        "repz stosb"
        : "+D"(addr), "+c"(size)
        : "a"((unsigned long)val)
        : "memory"
    );
}

inline void memcpy(void* dst, void* src, size_t sz) {
    asm volatile (
        "repz movsb"
        : "+D"(dst), "+S"(src), "+c"(sz)
        :
        : "memory"
    );
}