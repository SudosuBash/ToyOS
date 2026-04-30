#ifndef _TOYOS_FAULT_ERROR
#define _TOYOS_FAULT_ERROR

#include <kernel/fault/errno.h>
#include <kernel/stdint.h>

#define ERR_PTR(val) ((void*)(-(uint64_t)val))
#define IS_ERR(val) (((uint64_t)val) < 0 && ((uint64_t)val) >= -4095)
#endif