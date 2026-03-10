#ifndef _TOYOS_LINK
#define _TOYOS_LINK

#define INIT_LIST_HEAD(var) { \
    struct linklist_head (var) = {.next = NULL, .prev = NULL}; \
}

struct linklist_head {  
    struct linklist_head *next;
    struct linklist_head *prev;
};

void list_del(struct linklist_head* h1);
void link_next(struct linklist_head** h);
void list_head_insert(struct linklist_head* head, struct linklist_head** orig_head);
#endif