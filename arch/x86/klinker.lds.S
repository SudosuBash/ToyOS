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
        _stack_bottom = .;
        . += 0x4000;
        _stack_top = .;
    }
    .pgtable (NOLOAD): ALIGN(4096) {
        _pgtable_bottom = .;
        . += 0x300000;
        _pgtable_top = .;
    }
}
