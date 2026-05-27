#include <acpica/acpiosxf.h>
#include <kernel/task/task.h>
#include <kernel/irq/irq.h>
#include <kernel/mm/mm.h>
#include <kernel/fault/error.h>
#include <acpica/acexcep.h>
#include <kernel/acpi/acpi_hpet.h>

struct acpi_osi_convertor {
    ACPI_OSD_HANDLER handler;
    void* context;
}__attribute__((aligned(64)));

inline static uint32_t acpi_osi_handler(struct arch_regs* arch, void* device_data) {
    struct acpi_osi_convertor* convertor = (struct acpi_osi_convertor*)device_data;
    return convertor->handler(convertor->context);
}

ACPI_STATUS
AcpiOsInstallInterruptHandler (
    UINT32                  InterruptNumber,
    ACPI_OSD_HANDLER        ServiceRoutine,
    void                    *Context) {
    struct acpi_osi_convertor* convertor = kmalloc(sizeof(struct acpi_osi_convertor), GFP_KERNEL);

    if(IS_ERR(convertor))
        return AE_NO_MEMORY;
    convertor->context = Context;
    convertor->handler = ServiceRoutine;
    irq_register(InterruptNumber, acpi_osi_handler ,  (void*) convertor);
    return AE_OK;
}

ACPI_STATUS
AcpiOsRemoveInterruptHandler (
    UINT32                  InterruptNumber,
    ACPI_OSD_HANDLER        ServiceRoutine) {
    struct acpi_osi_convertor* convertor = container_of(ServiceRoutine, struct acpi_osi_convertor, handler);
    irq_remove(InterruptNumber, ServiceRoutine);
    kfree(convertor);
    return AE_OK;
}

ACPI_THREAD_ID
AcpiOsGetThreadId (
    void) {
    return pid_of(CURRENT_PROCESS());
}


ACPI_STATUS
AcpiOsExecute (
    ACPI_EXECUTE_TYPE       Type,
    ACPI_OSD_EXEC_CALLBACK  Function,
    void                    *Context) {
    return AE_OK;
}

ACPI_STATUS
AcpiOsTableOverride (
    ACPI_TABLE_HEADER       *ExistingTable,
    ACPI_TABLE_HEADER       **NewTable) {
    return AE_OK;
}

void AcpiOsStall (
    uint32_t                  Microseconds) {
    hpet_spin_wait(Microseconds);
}

void AcpiOsWaitEventsComplete(void) {

}

void AcpiOsSleep (UINT64 Milliseconds) {

}