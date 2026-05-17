#ifndef _TOYOS_LINK
#define _TOYOS_LINK

#include <kernel/stdint.h>

#define INIT_LIST_HEAD(var) do { \
    (var)->next=(var); \
    (var)->prev=(var); \
} while(0);

struct linklist_head {  
    struct linklist_head *next;
    struct linklist_head *prev;
};

#define list_for_entry(head, target) for((target) = (head)->next; (target) != (head); (target) = (target)->next)
#define list_head(list) ((list)->next)
#define list_tail(list) ((list)->prev)
#define list_empty(list) ((list)->next == (list) && (list)->prev == (list))

void list_del(struct linklist_head* h1);
void list_del_init(struct linklist_head* h);
void list_insert(struct linklist_head* insert, struct linklist_head* h);
void list_insert_rcu(struct linklist_head* h, struct linklist_head* target);
#endif