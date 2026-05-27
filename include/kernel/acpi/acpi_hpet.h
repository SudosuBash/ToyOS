#ifndef _TOYOS_ACPI_HPET_H
#define _TOYOS_ACPI_HPET_H

#include <kernel/acpi/acpi.h>
#include <kernel/stdint.h>

#define ACPI_HPET_PERIOD_OFFSET 0
#define ACPI_HPET_COUNTER_OFFSET 0xF0
#define ACPI_HPET_GCFG_OFFSET 0x10

uint64_t hpet_current_time();
uint32_t hpet_period();
void acpi_hpet_init();
void hpet_spin_wait(uint64_t milleseconds);
#endif