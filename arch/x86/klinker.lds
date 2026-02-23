ENTRY(_start)

SECTIONS {
    . = 0xC0000000;

    .text : {
        *(.text)
    }

    . = ALIGN(0x1000);
    .data : {
        *(.data)
    }

    /* BSS 段 */
    .bss : {
        *(.bss)
    }
}