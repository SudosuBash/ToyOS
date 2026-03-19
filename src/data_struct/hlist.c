#include <kernel/data_struct/htable.h>
#include <kernel/data_struct/linklist.h>
inline void hlist_init(struct htable_list* list) {
    for(int idx = 0;idx < HASH_BUCKET_CNT;idx++) {
        INIT_LIST_HEAD(&list->bucket[idx]);
    }
}

inline void hlist_insert(struct htable_list* list, struct linklist_head* sibling, uint64_t val) {
    uint64_t hash = hlist_calc_hash(val);
    list_insert(sibling, &list->bucket[hash]);
}
