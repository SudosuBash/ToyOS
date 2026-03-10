#ifndef _TOYOS_LOCK
#define _TOYOS_LOCK

#include <kernel/stdint.h>
typedef struct {
    uint8_t value;
} lock_t;


#endif