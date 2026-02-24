SECTIONS
{
    . = 0x20000; /* 假设你的加载地址 */
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}