#ifndef _TOYOS_KERNEL
#define _TOYOS_KERNEL

#include <kernel/atomic/rwlock.h>

#define offsetof(type, member) __builtin_offsetof(type, member)
#define container_of(object, type, member) \
     (type*)((char*)(object) - offsetof(type, member))

#define MAGIC_NUMBER 0x1D105D5B

#define FILE_DIR_NAME_MAX 64
#define FILE_DIR_PREFIX_MAX 256

struct sysinfo { 
     uint64_t sysinfo_time;
     rwlock_t sysinfo_rwlock;
};
#endif