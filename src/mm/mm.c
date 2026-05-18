#include <kernel/mm/mm.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/base/linklist.h>
#include <kernel/kernel.h>
#include <kernel/fault/fault.h>
#include <kernel/fault/error.h>
#include <kernel/mm/mm_user.h>
#include <kernel/cpu/smp.h>
#include <kernel/base/math.h>
#include <kernel/task/task.h>
#include <kernel/log/kprintf.h>
#include <kernel/cpu/archimpl.h>

static struct page *page_start;
static volatile uint64_t mem_pages;
static volatile uint64_t mem_side_pages;
static volatile uint64_t mem_alloced_pages;
static struct mm_buddy buddy;
static uint64_t mem_alloc_M;

struct page* find_head_page(struct page* page) {
    uint64_t index = page - page_start;
    assert(index!=0);
    int level = lowest_1(index); //找最低位的1
    struct page* cur = page;
    while(level < MM_BUDDY_MAX_LEVEL && !(cur->page_flags & MM_BUDDY_FLAG_HEAD)) {
        index = index & (index - 1); //第i位清空
        cur =page_start + index;
        level++;
    }
    if(level == MM_BUDDY_MAX_LEVEL) {
        warn("find_head_page(): head page not found!");
        return NULL;
    }
    return cur;
}

static void init_page_items() {
    uint64_t ptr_kend = PHYS2VADDR(get_kernel_end());
    page_start = (struct page*)PAGE_ROUND_UP(ptr_kend);
}

static uint64_t get_page_index(struct page* page) {
    return (uint64_t)(page - page_start);
}

inline uint64_t get_system_mem_alloced() {
    return mem_alloced_pages;
}

inline uint64_t get_system_mem_sum() {
    return mem_pages;
}

inline void ref_page(struct page* page) {
    assert(page->in_use); //保证page不是buddy system外面的
    atomic_inc(&page->pg_ref);
}

inline uint8_t unref_and_test_page(struct page* page) {
    return atomic_dec_and_test(&page->pg_ref);
}

inline uint64_t get_mem_alloc_percentage() {
    if(mem_alloc_M == 0) return 101; //101代表出错
    return (mem_alloced_pages * mem_alloc_M * 100) >> 32;
}
/**
 * 返回: return buddy的高页
 * 设置: page=当前的内核低页
*/
static inline struct page* buddy_page(struct page** page) {
    //操作的是索引
    uint64_t page_index = get_page_index(*page);
    uint64_t bit_mask = (1 << ((*page)->buddy_level-1));
    uint64_t high_page = (page_index | bit_mask);
    uint64_t low_page = page_index & ~bit_mask;
    (*page) = page_start + low_page;
    return page_start + high_page;
}

void free_page(struct page* page) {
    assert(page != NULL);
    assert(page->in_use != 0);

    int pages = MM_BUDDY_LEVEL_PAGES(page->buddy_level);

    if(!unref_and_test_page(page)) {
        warn("page ref is not zero.");
    }
    page[0].page_flags &= ~MM_BUDDY_FLAG_HEAD;//取消head

    //这个赋值语句放在这至关重要，因为它涉及了page=1的回收
    page->in_use=0;
    spin_lock(&buddy.buddy_lock);
    while(page->buddy_level <= MM_BUDDY_MAX_LEVEL) { //一直到超出索引为止
        struct page* orig_page = page;
        uint32_t level = orig_page->buddy_level;
        struct page* high_page = buddy_page(&page);
        struct page* another_page;
        //保证page是低页内存
        if(high_page == orig_page) another_page = page;
        else another_page = high_page;
        //获取另外一片内存块
        
        //至关重要的边界条件，another_page的buddy_level要是比orig的小的话，证明another_page还有多余的页.
        if(another_page->buddy_level != orig_page->buddy_level) {
            page = orig_page; //不继续合并了,page要切换回去
            break;
        }
        
        if(another_page->in_use) {
            page = orig_page;
            break;
        }
        list_del_init(&(another_page->buddy_sibling));
        
        //高位内存buddy_level，因为已经不再需要，变更为小喽啰了
        high_page->buddy_level = 0;
        page->buddy_level = level + 1;
    }
    INIT_LIST_HEAD(&(page->buddy_sibling));
    list_insert(&page->buddy_sibling, &buddy.groups[page->buddy_level-1]);
    spin_unlock(&buddy.buddy_lock);
    barrier();
    mem_alloced_pages-=pages;
}

struct page* alloc_page(uint64_t pages) {
    assert(pages!=0);
    if(pages == 0) return 0;
    int64_t group = highest_up_1(pages);
    //pages
    assert(group < MM_BUDDY_MAX_LEVEL);

    int now_group = group;

    spin_lock(&buddy.buddy_lock);
    while(list_empty(&buddy.groups[now_group])) 
        now_group++;

    if(now_group >= MM_BUDDY_MAX_LEVEL) {
        spin_unlock(&buddy.buddy_lock);
        return ERR_PTR(ENOMEM);
    }
    struct linklist_head* target = buddy.groups[now_group].next;
    struct page* p = container_of(target,struct page, buddy_sibling); //要分割的页
    list_del_init(buddy.groups[now_group].next);
    if(now_group == group) { //加速
        goto func_return;
    }

    for(int g = now_group-1;g>=group;g--) { //g是数组索引
        p->buddy_level = g+1;
        struct page* ptarget = buddy_page(&p); 
        //分配1页的时候这个会误判, 所以必须在末尾再次重新设置in_buddy_system
        ptarget->in_use=0;
        ptarget->buddy_level = g+1;
        
        INIT_LIST_HEAD(&(ptarget->buddy_sibling));
        list_insert(&(ptarget->buddy_sibling),&buddy.groups[g]); //另一半插回去
    }
    p->buddy_level = group+1;
func_return:
    pages = MM_BUDDY_LEVEL_PAGES(p->buddy_level);
    spin_unlock(&buddy.buddy_lock);
    //指向头部
    p[0].page_flags |= MM_BUDDY_FLAG_HEAD;
    p->in_use = 1;
    atomic_set(&p->pg_ref, 1);
    mem_alloced_pages+=pages;
    return p;
}

inline struct page* find_page_by_paddr(uintptr_t ptr) {
    uint32_t index = MM_PAGE_PINDEX(ptr);
    if(index >= mem_side_pages) return 0;
    if(index < 0) return 0;
    return &page_start[index];
}

inline struct page* find_page_by_vaddr(uintptr_t ptr) {
    uint32_t index = MM_PAGE_VINDEX(ptr);
    if(index >= mem_side_pages) return 0;
    if(index < 0) return 0;
    return &page_start[index];
}

inline void* get_page_paddr(struct page* page) {
    uintptr_t index = (uintptr_t)(page - page_start);
    if(index >= mem_side_pages) return 0;
    if(index < 0) return 0;
    return (void*)(index << PAGE_OFFSET);
}

inline void* get_page_vaddr(struct page* page) {
    uintptr_t index = (uintptr_t)(page - page_start);
    if(index >= mem_side_pages) return 0;
    if(index < 0) return 0;
    uintptr_t ptr = PHYS2VADDR(index << PAGE_OFFSET);
    return (void*)ptr;
}

static inline void build_up_buddy_sys(uint64_t index_left,uint64_t index_right) {
    static const uint64_t mem_buddy_pages = MM_BUDDY_MAX_LEVEL_PAGES();
    struct page *p_start = page_start + index_left, *p_end = page_start + index_right, *pg;
    p_start = (struct page*)(((uint64_t)p_start + mem_buddy_pages) & ~(mem_buddy_pages-1));
    for(pg = p_start; pg <= p_end - mem_buddy_pages;pg+=mem_buddy_pages) {
        barrier();
        INIT_LIST_HEAD(&(pg->buddy_sibling));
        list_insert(&(pg->buddy_sibling),&buddy.groups[MM_BUDDY_MAX_LEVEL-1]);
        pg->buddy_level = MM_BUDDY_MAX_LEVEL; //最高级
    }
    mem_pages += (uint64_t)(pg - p_start);
}

//现在不会获得
static void init_buddy() {
    struct mm_area_record* mem_record = get_mem_records();
    
    spin_init(&buddy.buddy_lock);
    //计算开始的page
    for(int i=0;i<MM_BUDDY_MAX_LEVEL;i++) {
        INIT_LIST_HEAD(&buddy.groups[i]);
    }

    uint64_t kern_start_idx = MM_PAGE_PINDEX(get_kern_addr()); //内核开始页
    mem_side_pages = get_mem_all_pages();
    uint64_t kern_end_idx = MM_PAGE_VINDEX(PAGE_ROUND_UP((uint64_t)(page_start + mem_side_pages)));
    barrier();

    for(int i=0;i<mem_record->num;i++) {
        uint64_t start_idx = MM_PAGE_PINDEX(PAGE_ROUND_UP(mem_record->area[i].from));
        uint64_t end_idx = MM_PAGE_PINDEX(mem_record->area[i].to);
        if((kern_start_idx >= start_idx && kern_end_idx > end_idx) || 
            (kern_start_idx < start_idx && kern_end_idx > start_idx)) {
            crash("Aiee, kernel was loaded to an invaild address!");
        }   
        barrier();
        if(kern_start_idx >= start_idx && kern_end_idx <= end_idx) {
            build_up_buddy_sys(start_idx, kern_start_idx);
            build_up_buddy_sys(kern_end_idx,end_idx);
        } else {
            build_up_buddy_sys(start_idx, end_idx);
        }   
    }
    barrier();
    kprintf("MEM: Buddy System Initialized.\n - Available page: %ld\n - Available mem: %ld Bytes\n", mem_pages, (mem_pages << PAGE_OFFSET));
    mem_alloc_M = ((uint64_t)1 << 32) / (mem_pages);
}

void init_mm() {
    init_mm_info(); 
    kprintf("MEM: Physical Memory: %ld Bytes.\n", get_machine_available_mem_sz());
    kprintf("MEM: Kernel Page Table Address: 0x%016x.\n", get_pgroot());
    mem_alloced_pages = 0;
    mem_pages = 0;
    init_page_items();
    init_buddy();
    
    init_mm_slab();
    init_vma_area();
    kmalloc_init();
}
