SECTIONS
{
    . = 0x10000; /* 假设你的加载地址 */
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}