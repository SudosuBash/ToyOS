#include <acpica/acpiosxf.h>
#include <acpica/acexcep.h>
#include <kernel/fault/error.h>
#include <kernel/mm/mm.h>
#include <kernel/mm/mmap.h>

void *
AcpiOsAllocate (
    ACPI_SIZE               Size) {
    void* mem = kmalloc(Size, GFP_KERNEL);
    if(IS_ERR(mem)) 
        return ERR_PTR(mem);
    return mem;
}

void *
AcpiOsAllocateZeroed (
    ACPI_SIZE               Size) {
    void* mem = kzalloc(Size, GFP_KERNEL);
    if(IS_ERR(mem))
        return ERR_PTR(mem);
    return mem;
}

void
AcpiOsFree (
    void *                  Memory) {
    kfree(Memory);
}

void *
AcpiOsMapMemory (
    ACPI_PHYSICAL_ADDRESS   Where,
    ACPI_SIZE               Length) {
    return do_remap((void*)Where, (void*)PHYS2VADDR_MMIO(Where), Length, PERM_X);
}

void
AcpiOsUnmapMemory (
    void                    *LogicalAddress,
    ACPI_SIZE               Size) {
    //暂时先不实现
}
ACPI_STATUS
AcpiOsCreateCache (
    char                    *CacheName,
    UINT16                  ObjectSize,
    UINT16                  MaxDepth,
    ACPI_CACHE_T            **ReturnCache) {
    struct kmem_cache *pool = kmem_cache_get(ObjectSize);
    if(IS_ERR(pool))
        return AE_NO_MEMORY;
    return AE_OK;
}

ACPI_STATUS
AcpiOsDeleteCache (
    ACPI_CACHE_T            *Cache) {
    kmem_cache_destroy(&Cache);
    return AE_OK;
}

ACPI_STATUS
AcpiOsPurgeCache (
    ACPI_CACHE_T            *Cache) {
    return AE_OK;
    //没必要啊, 我这个缓存自动归还 Buddy 的
}

void *
AcpiOsAcquireObject (
    ACPI_CACHE_T            *Cache) {
    return kmem_cache_alloc(Cache, GFP_ATOMIC);
}

ACPI_STATUS
AcpiOsReleaseObject (
    ACPI_CACHE_T            *Cache,
    void                    *Object) {
    kmem_cache_free(Object);
    return AE_OK;
}