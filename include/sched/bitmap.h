#ifndef _TOYOS_BITMAP
#define _TOYOS_BITMAP

#include <stdint.h>
typedef struct {
    uint8_t* data;
} bitmap;

void init_bitmap();
#endif