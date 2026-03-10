#ifndef _TOYOS_KERNEL_BITMAP
#define _TOYOS_KERNEL_BITMAP

#include <kernel/stdint.h>

#define SINGLE_BLOCK 64
#define BITMAP_BLOCKS(bitmap) (((bitmap)->size+ SINGLE_BLOCK - 1) >> 6)
#define BLOCK_MAX_VALUE (0xffffffffffffffff)
#define BLOCK_BIT_COUNT 64
#define BLOCK_BIT_OFFSET 6
#define BLOCK_BIT_COUNT_MASK (BLOCK_BIT_COUNT - 1)
#define BITMAP_BIT_MASK(val) (1 << (val))
#define BITMAP_BIT_UMASK(val) ~BITMAP_BIT_MASK((val))
#define BITMAP_NOT_FOUND(bitmap,val) ((val) >= (bitmap)->size)

struct bitmap {
    uint64_t* start_addr; //开始的 addr
    uint64_t size; //实际管理的大小
    uint64_t need_space; //占用的空间
};


void init_bitmap(struct bitmap* bitmap, uint64_t size);
uint64_t bitmap_find_first_zero(struct bitmap* bitmap);
void bitmap_bit_to_1(struct bitmap* bitmap, uint64_t index);
void bitmap_bit_to_0(struct bitmap* bitmap, uint64_t index);
void bitmap_fill(struct bitmap* bitmap, uint64_t group);
void bitmap_fill_range(struct bitmap* bitmap, uint64_t start, uint64_t end);
#endif