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
        . += 0x2000;
        __stack_top = .;
    }
    .pid_0_stack (NOLOAD): ALIGN(4096)
    {
        __pid_0_stack_bottom = .;
        . += 0x2000;
    }
    .mem_info (NOLOAD): ALIGN(4096) {
        __mem_info_bottom = .;
        . += 0x1000;
        __mem_info_top = .;
    }
    .pgtable (NOLOAD): ALIGN(4096) {
        __pgtable_bottom = .;
        . += 0x300000;
        __pgtable_top = .;
    }
    . = ALIGN(4096);
    __percpu_seg_start = .;
    .percpu 0 : AT(__percpu_start) {
        __percpu_start = .;
        QUAD(0);
        *(.percpu);
        *(.percpu.shared_aligned);
        __percpu_end = .;
    }
    __percpu_sz = __percpu_end - __percpu_start;
    . = __percpu_seg_start + __percpu_sz;
    .bsp_percpu (NOLOAD): ALIGN(4096) {
        __bsp_percpu_start = .;
        . += __percpu_sz;
    }
    . = ALIGN(4096);
    .bsp_ist0 (NOLOAD): ALIGN(4096) {
        __bsp_ist0 = .;
        . += 8192;
    }
    .bsp_ist1 (NOLOAD): ALIGN(4096) {
        __bsp_ist1 = .;
        . += 8192;
    }
    .bsp_ist2 (NOLOAD): ALIGN(4096) {
        __bsp_ist2 = .;
        . += 8192;
    }
    .bsp_ist3 (NOLOAD): ALIGN(4096) {
        __bsp_ist3 = .;
        . += 8192;
    }
    . = ALIGN(4096);
    __kernel_end = .;
}
