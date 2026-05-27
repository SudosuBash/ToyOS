#ifndef _TOYOS_ACPI_H
#define _TOYOS_ACPI_H

#include <acpica/acpi.h>

#define ACPI_RSDP_OF(addr) ((ACPI_TABLE_RSDP*)(addr))

ACPI_TABLE_MADT* get_acpi_madt(ACPI_TABLE_RSDP* root);
ACPI_TABLE_HPET* get_acpi_hpet(ACPI_TABLE_RSDP* root);

void init_acpi();
#endif