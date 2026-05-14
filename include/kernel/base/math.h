#ifndef _TOYOS_MATH
#define _TOYOS_MATH

#include <kernel/stdint.h>


#define min(a,b) ((a) < (b) ? (a) : (b))
#define signed_bigger(a,b) ((int64_t)((a)-(b)) > 0)
int highest_up_1(uint64_t x);
int highest_1(uint64_t x);
uint64_t div_10(uint64_t val);
int lowest_1(uint64_t x);
uint64_t div_32bit(uint64_t div, uint32_t num);
#endif