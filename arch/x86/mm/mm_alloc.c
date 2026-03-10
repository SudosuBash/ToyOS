#include <kernel/mm/mm_page.h>
#include <kernel/data_struct/bitmap.h>
#include <early_boot.h>
#include <mm/mm_info.h>
#include <kernel/mm/mm.h>
#include <kernel/mm/mm_slab.h>
#define KERNTOPADDR(addr) ((addr) - kern_ldr_vaddr)
#define INDEX_OF_PAGE(page) ((page) - page_start)

static uint64_t kern_ldr_vaddr;
static uint64_t kern_ldr_addr;
static uint64_t mem_sz;
static struct bitmap mem_bitmap;
static uint32_t* bitmap_start;
static uint64_t kern_sz;
static struct page *page_start;
static uint64_t mem_pages;

#define LOW_MEM 0x100000
//1 MB 以下的低端内存不在考虑范围内
static void fill_bitmap() {
    uint64_t kern_pos_start = (kern_ldr_addr - LOW_MEM) >> PAGE_OFFSET;
    uint64_t kern_pos_end = (uint64_t)(page_start+mem_pages);

    kern_pos_end = (kern_pos_end + PAGE_SZ - 1); 
    kern_pos_end = (VADDR2PHYS(kern_pos_end) - LOW_MEM) >> PAGE_OFFSET;
    //结束的索引
    bitmap_fill_range(&mem_bitmap, kern_pos_start, kern_pos_end);
}

//分配一个 4kb 的物理页
struct page* alloc_new_phys_page() {
    uint64_t index = bitmap_find_first_zero(&mem_bitmap);
    if(BITMAP_NOT_FOUND(&mem_bitmap, index)) return 0;
    bitmap_bit_to_1(&mem_bitmap, index);
    return &page_start[index];
}

struct page* find_page_by_vaddr(uintptr_t ptr) {
    ptr &= PAGE_MASK;
    ptr = VADDR2PHYS(ptr) - LOW_MEM;
    //获取相对于0x100000的offset, 从这儿开始计算ptr

    uint32_t index = ptr >> PAGE_OFFSET;
    if(index >= mem_pages) return 0;
    if(index < 0) return 0;
    return &page_start[index];
}
static void init_page_items() {
    uintptr_t pg = (uintptr_t)mem_bitmap.start_addr + mem_bitmap.need_space; //内核末尾
    page_start = (struct page*)((pg + PAGE_SZ - 1) & PAGE_MASK); //对齐
}

inline void* get_page_vaddr(struct page* page) {
    uintptr_t index = (uintptr_t)(page - page_start);
    if(index >= mem_pages) return 0;
    if(index < 0) return 0;
    return (void*)PHYS2VADDR((index << PAGE_OFFSET) + LOW_MEM);
}

void init_mm() {
    init_mem(); 

    mem_sz = get_available_mem_sz();
    kern_ldr_vaddr = get_kern_vaddr();
    kern_sz = get_kern_sz();
    kern_ldr_addr = get_kern_addr();


    extern uint64_t __kernel_end;
    uintptr_t ptr_kend = KERNTOPADDR((uintptr_t)&__kernel_end);

    mem_pages = (mem_sz + PAGE_SZ - 1) >> PAGE_OFFSET;
    //内存总共的页数
    mem_bitmap.start_addr = (uint64_t*)PHYS2VADDR(ptr_kend); //位图开始地址
    init_bitmap(&mem_bitmap, mem_pages); //初始化
    init_page_items(); //初始化页表
    fill_bitmap();

    init_mm_slab();
}