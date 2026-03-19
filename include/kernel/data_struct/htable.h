#ifndef _TOYOS_DS_HASH
#define _TOYOS_DS_HASH

#include <kernel/data_struct/linklist.h>
#include <kernel/data_struct/general.h>

#define HASH_BUCKET_OFFSET 10
#define HASH_BUCKET_CNT (1 << HASH_BUCKET_OFFSET)
#define HASH_BUCKET_LOWMASK (HASH_BUCKET_CNT-1)

#define hlist_calc_hash(num) ((num) & HASH_BUCKET_LOWMASK)

#define hlist_find(hlist, target, target_member, hlist_member, type) ({ \
    uint64_t __hlist_hash__ = hlist_calc_hash(target); \
    struct linklist_head *___hlist_head = &(hlist)->bucket[(__hlist_hash__)],*___hlist_target; \
    type* __target__obj = NULL; \
    list_for_entry(___hlist_head, ___hlist_target) {\
        type* __hlist__obj__ = container_of( ___hlist_target, type,  hlist_member); \
        if(__hlist__obj__->target_member == target) { \
            __target__obj = __hlist__obj__; \
            break; \
        } \
    } \
    (type*)__target__obj; \
})

struct htable_list {
    struct linklist_head bucket[HASH_BUCKET_CNT];
};

void hlist_init(struct htable_list* list);
void hlist_insert(struct htable_list* list, struct linklist_head* sibling, uint64_t val);
#endif