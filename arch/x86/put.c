#include <put.h>
#include <asm.h>

#define BASE_ADDR 0xB8000

void put_char(char c) {
    outb(0x3f8,c);
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