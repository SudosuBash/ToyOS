/**
 * 负责 Init 部分
*/
#include <acpica/acpiosxf.h>
#include <kernel/log/kprintf.h>
#include <acpica/acexcep.h>
#include <hal/cpu/cpu.h>

ACPI_STATUS AcpiOsInitialize (void) {
    kprintf("[ACPI] Initializing...\n");
    return AE_OK;
}

ACPI_STATUS AcpiOsTerminate (void) {
    kprintf("[ACPI] Terminating...\n");
    return AE_OK;
}

ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer (void) {
    return get_acpi_rsdp_addr();
}

ACPI_STATUS AcpiOsPredefinedOverride (
    const ACPI_PREDEFINED_NAMES *InitVal,
    ACPI_STRING                 *NewVal) {
    return AE_OK;
}

void AcpiOsPrintf (
    const char              *Format,
    ...) {
        
    kprintf(Format);
}

ACPI_STATUS
AcpiOsPhysicalTableOverride (
    ACPI_TABLE_HEADER       *ExistingTable,
    ACPI_PHYSICAL_ADDRESS   *NewAddress,
    UINT32                  *NewTableLength) {

    }