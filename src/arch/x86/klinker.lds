ENTRY(_start)

SECTIONS {
    . = 0x100000;

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