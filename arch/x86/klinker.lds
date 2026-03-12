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
    .bss : {
        *(.bss)
    }
    .stack (NOLOAD): ALIGN(4096)
    {
        __stack_bottom = .;
        . += 0x4000;
        __stack_top = .;
    }
    .gdt (NOLOAD): ALIGN(4096) {
        __gdt_bottom = .;
        . += 0x1000;
        __gdt_top = .;
    }
    .pgtable (NOLOAD): ALIGN(4096) {
        __pgtable_bottom = .;
        . += 0x300000;
        __pgtable_top = .;
    }
    . = ALIGN(64);
    __percpu_start = .;
    .percpu : {
        *(.percpu)
        *(.pecpu.shared_aligned)
    }
    __percpu_end = .;
    __percpu_sz = __percpu_end - __percpu_start;
    __kernel_end = .;
}
