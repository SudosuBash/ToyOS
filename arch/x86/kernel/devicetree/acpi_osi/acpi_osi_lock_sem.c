#include <devicetree/acpica/acpiosxf.h>

ACPI_STATUS AcpiOsCreateLock (
    ACPI_SPINLOCK           *OutHandle) {
    spin_init(OutHandle);
}


void AcpiOsDeleteLock (ACPI_SPINLOCK Handle) {
    
}

ACPI_CPU_FLAGS
AcpiOsAcquireLock (
    ACPI_SPINLOCK Handle) {

}

void AcpiOsReleaseLock (
    ACPI_SPINLOCK  Handle,
    ACPI_CPU_FLAGS  Flags) {

}

ACPI_STATUS
AcpiOsCreateSemaphore (
    UINT32                  MaxUnits,
    UINT32                  InitialUnits,
    ACPI_SEMAPHORE          *OutHandle) {

}

ACPI_STATUS
AcpiOsDeleteSemaphore (
    ACPI_SEMAPHORE          Handle) {

}

ACPI_STATUS
AcpiOsSignalSemaphore (
    ACPI_SEMAPHORE          Handle,
    UINT32                  Units) {

}

ACPI_STATUS
AcpiOsSignal (
    UINT32                  Function,
    void                    *Info) {

}