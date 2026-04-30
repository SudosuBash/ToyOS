#ifndef _TOYOS_X86_FAULT_H
#define _TOYOS_X86_FAULT_H


#define __crash_name_concat(line) __crash_info##line
#define arch_crash(msg,cond) do { \
    static struct crash_info\
    __attribute__((used, section(".crash_data"))) __crash_name_concat(__LINE__)= { \
        .filename = __FILE__, \
        .line = __LINE__, \
        .func=__func__, \
        .condition = (cond) \
    }; \
    (__crash_name_concat(__LINE__)).message = (const char*)(msg); \
    asm volatile( \
        "leaq %0, %%r15\n\t" \
        "ud2\n\t" \
        :  \
        : "m"(__crash_name_concat(__LINE__)) \
    ); \
} while(0);

#define arch_crash_on_irq(msg,irq_info) do { \
    fault_irq(msg,irq_info); \
}while(0);

#endif