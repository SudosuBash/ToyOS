#include <kernel/data_struct/linklist.h>

inline void list_del(struct linklist_head* h1) {
    if(h1->prev) {
        h1->prev->next = h1->next;
    }

    if(h1->next) {
        h1->next->prev = h1->prev;
    }
    h1->next = h1->prev = 0;
}

inline void list_del_head(struct linklist_head** h) {
    struct linklist_head* next = (*h)->next;
    list_del(*h);
    *h = next;
}

inline void list_next(struct linklist_head** h) {
    (*h) = (*h)->next;
}
inline void list_head_insert(struct linklist_head* h, struct linklist_head* *target) {
    (h)->next = (*target);
    if(*target != 0) (*target)->prev = (h);
    *target = h;
}

inline void list_tail_insert(struct linklist_head* tail, struct linklist_head** orig_tail) {
    tail->prev = (*orig_tail);
    if(*orig_tail!=0) (*orig_tail)->next = tail;
    (*orig_tail) = tail;
}