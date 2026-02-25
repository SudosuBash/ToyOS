#include <kernel/math.h>

inline uint64_t div_10(uint64_t num) {
    __uint128_t res = (__uint128_t)num * 0xCCCCCCCCCCCCCCCDULL;
    return res >> 67;
}