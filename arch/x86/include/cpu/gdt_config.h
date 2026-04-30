#ifndef _TOYOS_X86_GDT_CONFIG
#define _TOYOS_X86_GDT_CONFIG

#define KERNEL_CS (1*8) //desc[0] gdts[1]
#define KERNEL_DS (2*8) //desc[1] gdts[0]
#define USER_DS (3*8) //desc[1] gdts[1]
#define USER_CS (4*8) //desc[2] gdts[0]

#define KERNEL_TSS (6*8)

#define USER_RPL 0b11
#define KERNEL_RPL 0

#endif