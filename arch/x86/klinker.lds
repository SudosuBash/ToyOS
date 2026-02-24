ENTRY(_start)

SECTIONS {
    . = 0xffffffff80000000;

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

    .stack (NOLOAD): ALIGN(4096)
    {
        _stack_bottom = .;
        . += 0x4000;
        _stack_top = .;
    }
}