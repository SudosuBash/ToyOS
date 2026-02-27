#include <config_arch.h>
ENTRY(_start)
SECTIONS {
    . = KERNEL_FINAL_LDR_VADDR;
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
    .pgtable (NOLOAD): ALIGN(4096) {
        __pgtable_bottom = .;
        . += 0x300000;
        __pgtable_top = .;
    }

    . = ALIGN(64); //64位对齐
    __percpu_start = .; 
    .percpu : {
        *(.percpu) //所有.percpu段
        *(.pecpu.shared_aligned) //需要特殊的多核共享的段
    }
    __percpu_end = .;
    __percpu_sz = __percpu_end - __percpu_start;
}
