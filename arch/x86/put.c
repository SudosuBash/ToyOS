#include <kernel/put.h>
#include <asm.h>
#include <kernel/stdint.h>
#include <kernel/math.h>
#include <kernel/stdlib.h>
#define BASE_ADDR 0xB8000

void put_char(char c) {
    outb(0x3f8,c);
}

void put_dec(uint64_t n) {
    char buf[20] = {0};
    hex_to_dec(n,buf);
    put_str(buf);
}

void put_hex(uint64_t n) {
    char buf[20] = {0};
    hex_to_str(n, buf);
    put_str(buf);
}
void put_str(const char* c) {
    while(*c!=0) {
        put_char(*c);
        c++;
    }
}



void clear_screen() {
    const char* clear = "\033[2J\033[H";
    put_str(clear);
}