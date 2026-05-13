#include <pgtable/pgtable_kern.h>
#include <packed_e820.h>
#include <asm.h>
#include <cpu/cpu.h>
#include <kernel/mm/mm.h>
#include <kernel/put.h>
#include <kernel/irq/irq.h>
#include <early_boot.h>
#include <cpu/regs.h>
#include <kernel/fault/error.h>
#include <kernel/task/task.h>
#include <kernel/mm/mm_user.h>
#include <kernel/cpu/archimpl.h>
#include <kernel/mm/mmap.h>
#include <kernel/mm/mm_page.h>
#include <kernel/stdlib.h>

static struct e820_entry* edr_table;
static uint32_t edr_entry;
static uint64_t kernel_end;
static uint64_t kernel_vstart;


static inline void update_page_pte(void* paddr, uint16_t prot, pte_t* pte) {
    if(prot & PERM_W) pte->rw = 1;
    if(!(prot & PERM_X)) pte->nx = 1;
}

static void pagefault_irq(struct arch_regs* frame) {
    struct page *origin_pg, *new_pg;
    pte_t* target_pte;
    uint64_t target_addr, cow_addr = 0;
    struct user_vm_area* area;
    struct task_struct *current;
    
    if(!(frame->error_code & PG_ERR_PERM)) //内核操作
        arch_crash_on_irq("Page Fault", frame);
    
    current = CURRENT_PROCESS();

    if(frame->error_code & PG_ERR_PRESENT) //用户操作, 权限问题
        if(!(frame->error_code & PG_ERR_RW)) //读操作引起的
            goto sigsegv; 
    
    //页不存在或者 CoW
    target_addr = get_cr2();
    rwlock_read_lock(&current->mm_user.rwlock);
    area = find_vm_area(&current->mm_user, target_addr);

    if(area == NULL) 
        goto sigsegv;

    target_pte = get_user_pte(target_addr, current->mm_user.pg_root); //用户 pte

    if((frame->error_code & PG_ERR_PRESENT)) {//权限问题
        if(target_pte->us == 0 || !(area->perm & PERM_W)) //内核页? 或者压根就不让读?
            goto sigsegv;
        else { //此处处理 CoW
            origin_pg = find_page_by_paddr(target_addr);
            if(unref_and_test_page(origin_pg)) { //CoW 处理程序, 只剩下一个页了
                target_pte->rw = 1; //直接变成可写, 不再需要重新 Alloc Page
                goto go_back;
            }
            cow_addr = PHYS2VADDR(target_pte->base_addr << PAGE_OFFSET);
        }
    }

    new_pg = alloc_page(1); //New Page
    void* paddr = get_page_paddr(new_pg); //Phys Addr

    update_page_pte(paddr, area->perm, target_pte); 
    //填入页表基本信息
    user_cow_remap(paddr, target_pte); 
    //这个时候就需要重新 remap target_pte 了, 不管是 CoW 还是按需加载不存在的 page.
    //将地址设置为新的页, 然后增加引用计数
    invlpg(target_addr); //刷新页表
    if(cow_addr != 0) //CoW 复制
        memcpy((void*)target_addr, (void*)cow_addr, PAGE_SZ);
    goto go_back;
sigsegv: //此处发送 Segmentation Fault 信号, 但是信号暂时没写
go_back:
    rwlock_read_unlock(&current->mm_user.rwlock);
    return;
}

uintptr_t get_kernel_end() {
    return kernel_end;
}
//返回可用的字节

void init_mm_info() {
    uint32_t* mem_info_addr = (uint32_t*)PHYS2VADDR(get_mem_info_paddr());
    edr_entry = *mem_info_addr;
    edr_table = (struct e820_entry*)(mem_info_addr+1);
    kernel_vstart = get_kern_vaddr();
    extern uint64_t __kernel_end;
    kernel_end = KERN_VADDR_TO_PADDR((uintptr_t)&__kernel_end);
    
    irq_register(IRQ_PG_ERR, pagefault_irq);
}