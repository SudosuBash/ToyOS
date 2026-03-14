#include <kernel/math.h>

inline uint64_t div_10(uint64_t num) {
    __uint128_t res = (__uint128_t)num * 0xCCCCCCCCCCCCCCCDULL;
    return res >> 67;
}

inline int highest_up_1(uint64_t x)  {
    return x == 1 ? 0 : 64 - __builtin_clzll(x - 1);
}

inline int highest_1(uint64_t x)  {
    return 64 - __builtin_clzll(x);
}