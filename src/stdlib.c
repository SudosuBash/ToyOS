#include <kernel/stdlib.h>

void memset(void* addr,uint8_t val, size_t size) {
    uint8_t* uaddr = (uint8_t*)addr;
    for(int i=0;i<size;i++) {
        uaddr[i] = val;
    }
}

void hex_to_dec(uint64_t n,char* buf) {
    char bufc[21] = {0};
    int len = 0;
    if(n == 0) {
        buf[0] = '0';
        return;
    }
    while(n != 0) {
        uint64_t n1 = div_10(n);
        bufc[20-len] = (n - n1 * 10) + 48;
        n = n1;
        len++;
    }
    for(int i=0;i<len;i++) {
        buf[i]=bufc[20-len+1+i];
    }
}

void hex_to_str(uint64_t n,char* buf) {
    char bufc[21] = {0};
    buf[0] = '0';
    buf[1] = 'x';
    buf+=2;
    int len = 0;
    if(n == 0) {
        buf[0] = '0';
        return;
    }
    while(n!=0) {
        uint8_t c = n & 15;
        n>>=4;
        bufc[20-len] = (c<10) ? (c+48) : (c+97-10);
        len++;
    }
    
    for(int i=0;i<len;i++) {
        buf[i]=bufc[20-len+1+i];
    }
}