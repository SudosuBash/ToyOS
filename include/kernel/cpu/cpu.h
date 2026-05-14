#ifndef _TOYOS_CPU_H
#define _TOYOS_CPU_H

#include <kernel/stdint.h>
bool cpu_feature_rdtscq();
uint16_t smp_processor_id();

void init_smp();
void init_cpu();
#endif