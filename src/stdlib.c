#include <stdlib.h>

void memset(void* addr,uint8_t val, size_t size) {
    uint8_t* uaddr = (uint8_t*)addr;
    for(int i=0;i<size;i++) {
        uaddr[i] = val;
    }
}