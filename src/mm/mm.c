#include <kernel/mm/mm.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/data_struct/bitmap.h>
#include <mm/mm_info.h>
#include <kernel/data_struct/general.h>
static uint64_t kern_ldr_addr;
static uint64_t mem_sz;
static struct page *page_start;
static uint64_t mem_pages;

static struct mm_buddy buddy;

static void init_page_items() {
    uint64_t ptr_kend = PHYS2VADDR(get_kernel_end());
    page_start = (struct page*)((ptr_kend + PAGE_SZ -1) & PAGE_MASK);
}

static uint64_t get_page_index(struct page* page) {
    return (uint64_t)(page - page_start);
}

/**
 * 返回: return buddy的高页
 * 设置: page=当前的内核低页
*/
static inline struct page* buddy_page(struct page** page) {
    //操作的是索引
    uint64_t page_index = get_page_index(*page);
    uint64_t bit_mask = (*page)->buddy_level == 1 ? 0 : (1 << ((*page)->buddy_level-2));
    uint64_t high_page = (page_index | bit_mask);
    uint64_t low_page = page_index & ~bit_mask;
    (*page) = page_start + low_page;
    return page_start + high_page;
    //buddy_level = index + 1
    //1 << (index - 1)
    //index - 1 = buddy_level - 2
}

void free_page(struct page* page) {
    if(page == 0) return;
    page->in_buddy_system=1;
    //这个赋值语句放在这至关重要，因为它涉及了page=1的回收
    while(page->buddy_level <= MM_BUDDY_MAX_LEVEL) { //一直到超出索引为止
        struct page* orig_page = page;
        struct page* another_page;
        uint32_t level = orig_page->buddy_level;

        struct page* high_page = buddy_page(&page);
        //保证page是低页内存

        if(high_page == orig_page) another_page = page;
        else another_page = high_page;
        //获取另外一片内存块

        if(!(another_page->buddy_level == orig_page->buddy_level)) {
            page = orig_page; //不继续合并了,page要切换回去
            break;
        }
        //至关重要的边界条件，another_page的buddy_level要是比orig的小的话，证明another_page还有多余的页.
        if(!another_page->in_buddy_system) {
            page = orig_page;
            break;
        }
        //原来那块不在buddy_system中，已经被占用, 不再合并

        //删除原来在另外一个页中挂着的空闲空间
        int tpage_index = level-1;
        if(buddy.buddys[tpage_index] == &another_page->buddy_sibling) {
            list_next(&buddy.buddys[tpage_index]);
        }
        list_del(&(another_page->buddy_sibling));
        
        //高位内存buddy_level，因为已经不再需要，变更为小喽啰了
        high_page->buddy_level = 0;
        page->buddy_level = level + 1;
    }
    list_head_insert(&page->buddy_sibling, &buddy.buddys[page->buddy_level-1]);
    
}

struct page* alloc_page(uint64_t pages) {
    if(pages == 0) return 0;
    pages-=1;
    int64_t group = pages == 0 ? 0 : 64 - __builtin_clzll(pages - 1);
    //pages
    if(group >= MM_BUDDY_MAX_LEVEL) return 0; //直接拒绝
    int now_group = group;
    while(!buddy.buddys[now_group]) 
        now_group++;

    if(now_group >= MM_BUDDY_MAX_LEVEL) return 0; //分配失败 
    struct linklist_head* target = buddy.buddys[now_group];
    struct page* p = container_of(target,struct page, buddy_sibling); //要分割的页
    if(now_group == group) { //加速
        p->in_buddy_system = 0;
        list_del_head(&buddy.buddys[now_group]);
        return p;
    }
    for(int g = now_group-1;g>=group;g--) { //g是数组索引
        list_del_head(&buddy.buddys[now_group]);   
        p->buddy_level = g+1;
        // int page_sz = MM_BUDDY_LEVEL_PAGES(g+1); //g是索引, g+1才是组数. 我们的目的是切分数组.
        struct page* target = buddy_page(&p); 
        //分配1页的时候这个会误判, 所以必须在末尾再次重新设置in_buddy_system
        target->in_buddy_system=1;
        target->buddy_level = g+1;
        list_head_insert(&(target->buddy_sibling),&buddy.buddys[g]); //另一半插回去
    }
    p->in_buddy_system = 0;
    return p;
}

struct page* find_page_by_vaddr(uintptr_t ptr) {
    ptr &= PAGE_MASK;

    ptr = MM_PAGE_REL_ADDR(VADDR2PHYS(ptr));
    //获取相对于0x100000的offset, 从这儿开始计算ptr
    
    uint32_t index = ptr >> PAGE_OFFSET;
    if(index >= mem_pages) return 0;
    if(index < 0) return 0;
    return &page_start[index];
}

inline void* get_page_vaddr(struct page* page) {
    if(page->vaddr) return (void*) page->vaddr; //加速
    uintptr_t index = (uintptr_t)(page - page_start);
    if(index >= mem_pages) return 0;
    if(index < 0) return 0;
    uintptr_t ptr = PHYS2VADDR(MM_PAGE_ABS_ADDR(index << PAGE_OFFSET));

    page->vaddr = ptr;
    return (void*)ptr;
}

static void init_buddy() {
    uint64_t mem_buddy_pages = MM_BUDDY_MAX_LEVEL_PAGES();
    struct page* pg_end = page_start + mem_pages;
    for(struct page* pg = page_start;pg < pg_end; pg+= mem_buddy_pages) {
        list_head_insert(&(pg->buddy_sibling),&buddy.buddys[MM_BUDDY_MAX_LEVEL-1]);
        pg->buddy_level = MM_BUDDY_MAX_LEVEL; //最高级
    }
}

void* kmalloc(size_t sz) {
    
}

void init_mm() {
    init_mm_info(); 
    mem_sz = get_available_mem_sz();
    kern_ldr_addr = get_kern_addr();
    mem_pages = (mem_sz + PAGE_SZ - 1) >> PAGE_OFFSET;

    init_page_items();
    init_buddy();

    init_mm_slab();
}