#include <irq/pic/pic.h>
#include <asm.h>
void init_pic() {
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}