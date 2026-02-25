#include <kernel/clock.h>


static uint64_t initial_clock;

static uint64_t get_clock() {
    uint32_t eax,edx;
    asm volatile(
        "rdtscp"
        : "=a"(eax), "=d"(edx)
    );
    return ((uint64_t)edx<<32 | eax);
}

void clock_init() {
    initial_clock = get_clock();
}

uint64_t clock_current() {
    return get_clock() - initial_clock;
}