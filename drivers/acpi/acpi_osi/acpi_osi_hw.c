#include <acpica/acpiosxf.h>
#include <acpica/acexcep.h>
#include <hal/hal.h>
#include <kernel/mm/mm.h>
#include <kernel/cpu/archimpl.h>

ACPI_STATUS
AcpiOsReadPort (
    ACPI_IO_ADDRESS         Address,
    UINT32                  *Value,
    UINT32                  Width) {
    switch(Width) {
        case 8:
            *Value = (UINT32)inb((UINT32)Address);
            break;
        case 16:
            *Value = (UINT32)inw((UINT32)Address);
            break;
        case 32:
            *Value = (UINT32)inl((UINT32)Address);
            break;
        default:
            return AE_BAD_PARAMETER;
    }
    return AE_OK;
}

ACPI_STATUS
AcpiOsWritePort (
    ACPI_IO_ADDRESS         Address,
    UINT32                  Value,
    UINT32                  Width) {
    switch(Width) {
        case 8:
            outb((uint32_t)Address, (uint8_t) Value);
            break;
        case 16:
            outw((uint32_t)Address, (uint16_t) Value);
            break;
        case 32:
            outl((uint32_t)Address, Value);
            break;
        default:
            return AE_BAD_PARAMETER;
    }
    return AE_OK;
}

ACPI_STATUS
AcpiOsReadMemory (
    ACPI_PHYSICAL_ADDRESS   Address,
    UINT64                  *Value,
    UINT32                  Width) {
    switch(Width) {
        case 8:
            *(UINT8*)Value = *(UINT8*)PHYS2VADDR(Address);
            break;
        case 16:
            *(UINT16*)Value = *(UINT16*)PHYS2VADDR(Address);
            break;
        case 32:
            *(UINT32*)Value = *(UINT32*)PHYS2VADDR(Address);
            break;
        case 64:
            *(UINT64*)Value = *(UINT64*)PHYS2VADDR(Address);
            break;
        default:
            return AE_BAD_PARAMETER;
    }
    return AE_OK;
}

ACPI_STATUS
AcpiOsWriteMemory (
    ACPI_PHYSICAL_ADDRESS   Address,
    UINT64                  Value,
    UINT32                  Width) {
    switch(Width) {
        case 8:
            *(UINT8*)PHYS2VADDR(Address) = *(UINT8*)Value;
            break;
        case 16:
            *(UINT16*)PHYS2VADDR(Address) = *(UINT16*)Value;
            break;
        case 32:
            *(UINT32*)PHYS2VADDR(Address) = *(UINT32*)Value;
            break;
        case 64:
            *(UINT64*)PHYS2VADDR(Address) = *(UINT64*)Value;
            break;
        default:
            return AE_BAD_PARAMETER;
    }
    return AE_OK;
}

ACPI_STATUS
AcpiOsReadPciConfiguration (
    ACPI_PCI_ID             *PciId,
    UINT32                  Reg,
    UINT64                  *Value,
    UINT32                  Width) {
    *Value = read_pci_configuration(PciId->Bus, PciId->Device, PciId->Function, Reg, Width);
    //4字节对齐
    return AE_OK;
}

ACPI_STATUS
AcpiOsWritePciConfiguration (
    ACPI_PCI_ID             *PciId,
    UINT32                  Reg,
    UINT64                  Value,
    UINT32                  Width) {
    write_pci_configuration(PciId->Bus, PciId->Device, PciId->Function, Reg, Width, Value);
    return AE_OK;
}

ACPI_STATUS
AcpiOsEnterSleep (
    UINT8                   SleepState,
    UINT32                  RegaValue,
    UINT32                  RegbValue) {
    return AE_OK;
}

uint64_t
AcpiOsGetTimer (
    void) {
    
}