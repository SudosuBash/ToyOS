#include <kernel/data_struct/linklist.h>

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
    if(target->next!=target) 
        target->next->prev=h;
    target->next = h;
}

inline uint8_t list_empty(struct linklist_head* h) {
    return h->next == h && h->prev == h;
}