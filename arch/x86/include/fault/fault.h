#ifndef _TOYOS_X86_FAULT_H
#define _TOYOS_X86_FAULT_H

#define arch_panic(msg,cond) { \
    static const struct panic_info __panic_info = { \
        .filename = __FILE__, \
        .line = __LINE__, \
        .func=__func__, \
        .message = (msg), \
        .condition = (cond) \
    }; \
    asm volatile( \
        "movq %0,%%r15\n\t" \
        "ud2\n\t" \
        :  \
        : "r"(&__panic_info) \
    ); \
}

#endif