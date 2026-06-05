#ifndef _TOYOS_SYSCALL
#define _TOYOS_SYSCALL

#define DEFINE_SYSCALL(fn1) \
    long syscall_##fn1()
#define DEFINE_SYSCALL1(fn1, argname1, type1) \
    long syscall_##fn1(type1 argname1)
#define DEFINE_SYSCALL2(fn1, argname1, type1, argname2, type2) \
    long syscall_##fn1(type1 argname1, type2 argname2)
#define DEFINE_SYSCALL3(fn1, argname1, type1, argname2, type2, argname3, type3) \
    long syscall_##fn1(type1 argname1, type2 argname2, type3 argname3)
#define DEFINE_SYSCALL4(fn1, argname1, type1, argname2, type2, argname3, type3, argname4, type4) \
    long syscall_##fn1(type1 argname1, type2 argname2, type3 argname3, type4 argname4)
#define DEFINE_SYSCALL5(fn1, argname1, type1, argname2, type2, argname3, type3, argname4, type4, argname5, type5) \
    long syscall_##fn1(type1 argname1, type2 argname2, type3 argname3, type4 argname4, type5 argname5)
#define DEFINE_SYSCALL6(fn1, argname1, type1, argname2, type2, argname3, type3, argname4, type4, argname5, type5, argname6, type6) \
    long syscall_##fn1(type1 argname1, type2 argname2, type3 argname3, type4 argname4, type5 argname5, type6 argname6)

#define DECLARE_SYSCALL(fn1,argname1, type1) \
    extern long syscall_##fn1(type1 argname1)

long empty_syscall();
typedef long (*syscall_fn)(void*);
void ret_to_user();
void init_syscall_cpu();
#endif