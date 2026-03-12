#ifndef _TOYOS_FAULT_H
#define _TOYOS_FAULT_H

#include <fault/fault.h>
struct panic_info {
    const char* func;
    int line;
    const char* filename;
    const char* message;
    const char* condition;
};

#define panic(msg) arch_panic(msg, NULL)
#define assert(condition) do { \
    if(!(condition)) arch_panic("Assertion failed: ", (#condition)); \
} while(0)
void fault_init();
#endif