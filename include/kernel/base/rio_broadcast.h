#ifndef _TOYOS_RIO_BROADCAST_H
#define _TOYOS_RIO_BROADCAST_H

#include <kernel/stdint.h>
#include <kernel/base/linklist.h>

#define RIO_QUEUE_MAXLEN 1024

struct rio_reader {
    struct linklist_head sibling;
    uint64_t head;
};

struct rio_broadcast {
    char buf[RIO_QUEUE_MAXLEN];
    uint64_t tail_counter; //锁机制抢的
    uint64_t tail; 

    uint64_t slowest_head;
    struct linklist_head readers;
};

void rio_broadcast_send(struct rio_broadcast* broadcast, const char* str, size_t len);
char rio_broadcast_recv(struct rio_broadcast* broadcast, struct rio_reader* reader);
void init_rio_broadcast(struct rio_broadcast* broadcast);
void register_reader_broadcast(struct rio_broadcast* broadcast, struct rio_reader* reader);

#endif