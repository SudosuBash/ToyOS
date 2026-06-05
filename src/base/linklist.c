#include <kernel/base/linklist.h>
#include <kernel/cpu/smp.h>

inline void list_del(struct linklist_head* h1) {
    h1->prev->next = h1->next;
    h1->next->prev = h1->prev;
}

inline void list_del_init(struct linklist_head* h) {
    list_del(h);
    INIT_LIST_HEAD(h);
}

inline void list_insert(struct linklist_head* h, struct linklist_head* target) {
    h->prev = target;
    h->next = target->next;
    target->next = h;
    h->next->prev=h;
}

inline void list_insert_rcu(struct linklist_head* h, struct linklist_head* target) {
    h->prev = target;
    h->next = target->next;
    smp_wmb();
    WRITE_ONCE(target->next, h);
    WRITE_ONCE(h->next->prev, h);
}