#include <kernel/stdint.h>

#define ACPI_RSDT_EXT_BA 0x40E
#define ACPI_2_RSDT_SZ 36
#define ACPI_RDST_SZ 20
#define ACPI_BIOS_BA 0xE0000
#define ACPI_BIOS_EA 0xFFFFF

static int8_t acpi_match(uint8_t* start) {
    uint64_t csum = 0, ret = 0;
    uint8_t target[8] = "RSD PTR ";
    for(int i=0;i<8;i++) {
        if(start[i] != target[i])
            return 0;
    }

    for(int i=0;i<20;i++) {
        csum += (uint64_t)start[i];
    }
    
    if(csum & 255) return 0;
    else ret = 1;

    for(int i=20;i<36;i++) {
        csum += (uint64_t)start[i];
    }

    if((csum & 255) == 0)
        ret = 2;

    return ret;
}

uint32_t find_acpi_pointer() {
    uint32_t base_addr = *(uint16_t*)ACPI_RSDT_EXT_BA;
    base_addr <<= 4;
    uint8_t* end = (uint8_t*)(base_addr + 1023);

    uint32_t addr = 0;
    for(uint8_t* start = (uint8_t*)base_addr; start < end; start += 16) {
        int8_t ret = acpi_match(start);
        if(ret != 0)
            addr = (uint32_t) start;
        if(ret == 2) 
            return addr;
    }

    if(addr != 0)
        return addr;

    for(uint8_t* start = (uint8_t*)ACPI_BIOS_BA; start < (uint8_t*)ACPI_BIOS_EA; start += 16) {
        int8_t ret = acpi_match(start);
        if(ret != 0)
            return (uint32_t)start;
    }
    return 0;
}