
#ifndef __ACTOYOS_H__
#define __ACTOYOS_H__

/* Common (in-kernel/user-space) ACPICA configuration */

#define ACPI_USE_DO_WHILE_0
#define ACPI_IGNORE_PACKAGE_RESOLUTION_ERRORS

#define ACPI_USE_SYSTEM_INTTYPES
#define ACPI_USE_GPE_POLLING
/* Kernel specific ACPICA configuration */

#ifdef CONFIG_ACPI_REDUCED_HARDWARE_ONLY
#define ACPI_REDUCED_HARDWARE 1
#endif

#ifdef CONFIG_ACPI_DEBUGGER
#define ACPI_DEBUGGER
#endif

#ifdef CONFIG_ACPI_DEBUG
#define ACPI_MUTEX_DEBUG
#endif

#include <kernel/mm/mm_slab.h>
#include <kernel/atomic/sem.h>
// #include <linux/string.h>
// #include <linux/kernel.h>
// #include <linux/ctype.h>
// #include <linux/sched.h>
// #include <linux/atomic.h>
// #include <linux/math64.h>
// #include <linux/slab.h>
// #include <linux/spinlock_types.h>
// #ifdef EXPORT_ACPI_INTERFACES
// #include <linux/export.h>
// #endif
// #ifdef CONFIG_ACPI
// #include <asm/acenv.h>
// #endif

#define ACPI_INIT_FUNCTION __init

/* Use a specific bugging default separate from ACPICA */

#ifndef CONFIG_ACPI

/* External globals for __KERNEL__, stubs is needed */

/* Generating stubs for configurable ACPICA macros */


/* Generating stubs for configurable ACPICA functions */

#define ACPI_NO_ERROR_MESSAGES
#undef ACPI_DEBUG_OUTPUT

/* External interface for __KERNEL__, stub is needed */

#define ACPI_EXTERNAL_RETURN_STATUS(Prototype) Prototype;
#define ACPI_EXTERNAL_RETURN_OK(Prototype) Prototype;
#define ACPI_EXTERNAL_RETURN_VOID(Prototype) Prototype;

#define ACPI_EXTERNAL_RETURN_UINT32(Prototype) Prototype;
#define ACPI_EXTERNAL_RETURN_PTR(Prototype) Prototype;

#endif /* CONFIG_ACPI */

/* Host-dependent types and defines for in-kernel ACPICA */

#define ACPI_MACHINE_WIDTH          64
#define ACPI_USE_NATIVE_MATH64
#define ACPI_EXPORT_SYMBOL(symbol) 
#define strtoul                     simple_strtoul

#define ACPI_CACHE_T                struct kmem_cache
#define ACPI_SPINLOCK               spinlock_t *
#define ACPI_CPU_FLAGS              unsigned long
#define ACPI_SEMAPHORE              sem_t *

#define ACPI_UINTPTR_T              uintptr_t

#define ACPI_TO_INTEGER(p)          ((uintptr_t)(p))
#define ACPI_OFFSET(d, f)           offsetof(d, f)

/* Use native linux version of AcpiOsAllocateZeroed */

#define USE_NATIVE_ALLOCATE_ZEROED

#define ACPI_USE_SYSTEM_CLIB

#define ACPI_MSG_ERROR          "ACPI Error: "
#define ACPI_MSG_EXCEPTION      "ACPI Exception: "
#define ACPI_MSG_WARNING        "ACPI Warning: "
#define ACPI_MSG_INFO           "ACPI: "

#define ACPI_MSG_BIOS_ERROR     "ACPI BIOS Error (bug): "
#define ACPI_MSG_BIOS_WARNING   "ACPI BIOS Warning (bug): "

/*
 * Linux wants to use designated initializers for function pointer structs.
 */
#define ACPI_STRUCT_INIT(field, value)  .field = value

#endif /* __ACLINUX_H__ */