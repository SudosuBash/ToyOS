#ifndef _TOYOS_LINK
#define _TOYOS_LINK

#define INIT_LIST_HEAD(var) { \
    struct linklist_head (var) = {.next = NULL, .prev = NULL}; \
}

struct linklist_head {  
    struct linklist_head *next;
    struct linklist_head *prev;
};


#endif