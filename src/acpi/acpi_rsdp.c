#include <kernel/acpi/acpi.h>
#include <kernel/acpi/acpi_hpet.h>
#include <kernel/acpi/acpi_apic.h>
#include <kernel/stdlib.h>
#include <kernel/mm/mmap.h>
extern struct system_static_data sysdata;

ACPI_TABLE_MADT* get_acpi_madt(ACPI_TABLE_RSDP* root) {
    uint64_t rsdt_phys_addr = root->RsdtPhysicalAddress;
    if(root->Revision > 1)
        rsdt_phys_addr = root->XsdtPhysicalAddress;
    ACPI_TABLE_RSDT* rsdt = (ACPI_TABLE_RSDT*)PHYS2VADDR_MMIO(rsdt_phys_addr);
    ACPI_TABLE_MADT* madt = NULL;
    
    uint64_t indexes = (rsdt->Header.Length - sizeof(ACPI_TABLE_HEADER)) >> 2;
    for(int i=0;i<indexes; i++) {
        char (*table_addr)[4] = (char (*)[4])PHYS2VADDR_MMIO(rsdt->TableOffsetEntry[i]);
        if(strncmp(*table_addr, "APIC", 4) == 0) {
            madt = (ACPI_TABLE_MADT*)table_addr;
            iomap((void*)(uintptr_t)rsdt->TableOffsetEntry[i], madt->Header.Length);
            break;
        }
    }
    return madt;
}

ACPI_TABLE_HPET* get_acpi_hpet(ACPI_TABLE_RSDP* root) {
    uint64_t rsdt_phys_addr = root->RsdtPhysicalAddress;
    if(root->Revision > 1) {
        rsdt_phys_addr = root->XsdtPhysicalAddress;   
    }
    ACPI_TABLE_RSDT* rsdt = (ACPI_TABLE_RSDT*)PHYS2VADDR_MMIO(rsdt_phys_addr);
    ACPI_TABLE_HPET* hpet = NULL;
    barrier();
    uint64_t indexes = (rsdt->Header.Length - sizeof(ACPI_TABLE_HEADER)) >> 2;
    for(int i=0;i<indexes; i++) {
        char (*table_addr)[4] = (char (*)[4])PHYS2VADDR_MMIO(rsdt->TableOffsetEntry[i]);
        if(strncmp(*table_addr, "HPET", 4) == 0) {
            hpet = (ACPI_TABLE_HPET*)table_addr;
            iomap((void*)(uintptr_t)rsdt->TableOffsetEntry[i], hpet->Header.Length);
            break;
        }
    }
    return hpet;
}

void init_acpi() {
    ACPI_TABLE_RSDP *root = sysdata.acpi_data.rsdp;
    uint64_t rsdt_phys_addr = root->RsdtPhysicalAddress;
    iomap((void*)(uintptr_t)rsdt_phys_addr, sizeof(ACPI_TABLE_HEADER));
    if(root->Revision > 1) {
        rsdt_phys_addr = root->XsdtPhysicalAddress;   
    }
    ACPI_TABLE_RSDT* rsdt = (ACPI_TABLE_RSDT*)PHYS2VADDR_MMIO(rsdt_phys_addr);
    iomap((void*)(uintptr_t)rsdt_phys_addr, rsdt->Header.Length);
    uint64_t indexes = (rsdt->Header.Length - sizeof(ACPI_TABLE_HEADER)) >> 2;
    for(int i=0;i<indexes; i++) 
        iomap((void*)(uintptr_t)rsdt->TableOffsetEntry[i], sizeof(ACPI_TABLE_HEADER));

    
    acpi_hpet_init();
    init_apic();
}