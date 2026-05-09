#include <devicetree/acpica/acpiosxf.h>
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
}

void
AcpiOsUnmapMemory (
    void                    *LogicalAddress,
    ACPI_SIZE               Size) {

}

ACPI_STATUS
AcpiOsGetPhysicalAddress (
    void                    *LogicalAddress,
    ACPI_PHYSICAL_ADDRESS   *PhysicalAddress) {

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
    
}

void *
AcpiOsAcquireObject (
    ACPI_CACHE_T            *Cache) {
    
}

ACPI_STATUS
AcpiOsReleaseObject (
    ACPI_CACHE_T            *Cache,
    void                    *Object) {

}