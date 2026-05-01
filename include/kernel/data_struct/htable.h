#ifndef _TOYOS_DS_HASH
#define _TOYOS_DS_HASH

#include <kernel/data_struct/linklist.h>
#include <kernel/kernel.h>

#define HASH_BUCKET_OFFSET 10
#define HASH_BUCKET_CNT (1 << HASH_BUCKET_OFFSET)
#define HASH_BUCKET_LOWMASK (HASH_BUCKET_CNT-1)

#define hlist_calc_hash(num) ((num) & HASH_BUCKET_LOWMASK)

struct htable_list {
    struct linklist_head bucket[HASH_BUCKET_CNT];
};

void hlist_init(struct htable_list* list);
void hlist_insert(struct htable_list* list, struct linklist_head* sibling, uint64_t val);
void hlist_insert_rcu(struct htable_list* list, struct linklist_head* sibling, uint64_t val);
#endif