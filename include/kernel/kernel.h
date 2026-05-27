#ifndef _TOYOS_KERNEL
#define _TOYOS_KERNEL

#include <kernel/atomic/rwlock.h>

#define offsetof(type, member) __builtin_offsetof(type, member)
#define container_of(object, type, member) \
     (type*)((char*)(object) - offsetof(type, member))

#define MAGIC_NUMBER 0x1D105D5B

#define FILE_DIR_NAME_MAX 64
#define FILE_DIR_PREFIX_MAX 256

struct acpi_table_rsdp;
typedef struct acpi_table_rsdp ACPI_TABLE_RSDP;

struct system_acpi_data {
     ACPI_TABLE_RSDP* rsdp;
     uint64_t hpet_vaddr;
};

struct system_static_data {
     struct system_acpi_data acpi_data;
     uint64_t mem_sz;
     uint64_t mem_all_pages;
     uint64_t kernel_size;
     uint64_t kernel_load_address;
     uint64_t kernel_load_vaddr;
     uint64_t kernel_end;
     uint64_t kernel_exception_stack;
     uintptr_t percpu_start_addr;
     uintptr_t ap_sp;
     struct page* page_start;
     struct mm_area_record* mm_area;
     struct smp_data_acpi_info* smp_info;
};

#endif