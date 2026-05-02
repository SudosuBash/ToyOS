#include <kernel/stdlib.h>
#include <kernel/math.h>
#include <kernel/cpu/smp.h>

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
        barrier();
        n = n1;
        len++;
    }
    for(int i=0;i<len;i++) {
        buf[i]=bufc[20-len+1+i];
    }
    buf[len] = '\0';
}

void hex_to_str(uint64_t n,char* buf,int leng) {
    char bufc[21] = {0};
    buf[0] = '0';
    buf[1] = 'x';
    buf+=2;
    int len = 0;
    if(leng >= 16) leng = 16;
    if(n == 0) {
        for(int i=0;i<leng;i++) {
            buf[i] = '0';
        }
        buf[leng]='\0';
        return;
    }
    while(n!=0) {
        uint8_t c = n & 15;
        n>>=4;
        bufc[20-len] = (c<10) ? (c+48) : (c+97-10);
        len++;
    }

    if(leng >= len) {
        for(int i=0;i<leng-len;i++) {
            buf[0]='0';
            buf++;
        }
    }

    for(int i=0;i<len;i++) {
        buf[i]=bufc[20-len+1+i];
    }
    buf[len] = '\0';
}

void memcpy(void* dst, void* src, size_t sz) {
    uint8_t* dst_e = (uint8_t*) dst;
    uint8_t* src_e = (uint8_t*) src;
    for(int i=0;i<sz;i++) {
        dst_e[i] = src_e[i];
    }
}

size_t strlen(char* c) {
    char* st = c;
    while(*c) c++;
    return (size_t)(c - st);
}

uint8_t strcmp(char* c1, char* c2) {
    while(*c1==*c2 && *c1 && *c2) {
        c1++;c2++;
    }
    if(*c1 > *c2) return 1;
    else if(*c1 == *c2) return 0;
    else return -1;
}

uint8_t strcmp_l(char* c1, char* c2, size_t sz) {
    char* c1_ori = c1;
    while(*c1 == *c2 && ((size_t)(c1 - c1_ori) < sz) && *c1 && *c2) {
        c1++;
        c2++;
    }
    if(*c1 == *c2 || ((size_t)(c1 - c1_ori) == sz)) return 0;
    else if(*c1 > *c2) return 1;
    else return -1;
}