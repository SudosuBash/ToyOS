#include <kernel/syscall/syscall.h>
#include <kernel/fault/errno.h>

long empty_syscall() {
    return ENOSYS;
}