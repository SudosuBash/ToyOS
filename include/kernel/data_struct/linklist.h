#ifndef _TOYOS_LINK
#define _TOYOS_LINK

#include <kernel/stdint.h>
#include <kernel/atomic/spinlock.h>
#define INIT_LIST_HEAD(var) do { \
    (var)->next=(var); \
    (var)->prev=(var); \
} while(0);

struct linklist_head {  
    struct linklist_head *next;
    struct linklist_head *prev;
};

void list_del(struct linklist_head* h1);
void list_del_init(struct linklist_head* h);
void list_insert(struct linklist_head* h, struct linklist_head* target);
uint8_t list_empty(struct linklist_head* h);
#endif