#include <kernel/base/bitmap.h>
#include <kernel/mm/mm.h>
#include <kernel/stdlib.h>

#include <kernel/syscall/syscall.h>
void init_bitmap(struct bitmap* bitmap, uint64_t size) {
    bitmap->size = size;
    uint64_t blocks = BITMAP_BLOCKS(bitmap);
    bitmap->need_space = blocks * sizeof(uint64_t);
    bitmap->start_addr = (uint64_t*)kmalloc(size);
    memset(bitmap->start_addr,0, sizeof(uint64_t) * blocks);
}

uint64_t bitmap_find_first_zero(struct bitmap* bitmap) {
    uint64_t index = 0;
    uint64_t blocks = BITMAP_BLOCKS(bitmap);
    while(bitmap->start_addr[index] == BLOCK_MAX_VALUE && index < blocks) {
        index++;
    }
    if(index == blocks) return index << 6; //返回>=bitmap->size的代表找不到
    uint64_t val = bitmap->start_addr[index];
    uint64_t sub_index = __builtin_ctzll(~val); //加速
    uint64_t idx = (index << 6) + sub_index;
    return idx; //同上

}
void bitmap_bit_to_1(struct bitmap* bitmap, uint64_t index) {
    if(index >= bitmap->size) return;
    int idx = index >> 6, rest = index & 31;
    bitmap->start_addr[idx] |= BITMAP_BIT_MASK(rest);
}

void bitmap_bit_to_0(struct bitmap* bitmap, uint64_t index) {
    if(index >= bitmap->size) return;
    int idx = index >> 6, rest = index & 31;
    bitmap->start_addr[idx] &= BITMAP_BIT_UMASK(rest);
}

void bitmap_fill(struct bitmap* bitmap, uint64_t start_index) {
    uint64_t max_group = BITMAP_BLOCKS(bitmap);
    uint64_t group = start_index >> 6; //组数
    if(max_group <= group) return;
    bitmap->start_addr[group] = BLOCK_MAX_VALUE;
}

//fill 1 to [start, end)
void bitmap_fill_range(struct bitmap* bitmap, uint64_t start, uint64_t end) {
    while((start & BLOCK_BIT_COUNT_MASK) && start < end) {
        bitmap_bit_to_1(bitmap, start);
        start++;
    }
    uint64_t fill_count = (end - start) >> BLOCK_BIT_OFFSET;
    for(uint64_t i=0;i<fill_count;i++) {
        bitmap_fill(bitmap, start);
        start += BLOCK_BIT_COUNT;
    }

    while(start < end) {
        bitmap_bit_to_1(bitmap, start);
        start++;
    }
    
}
