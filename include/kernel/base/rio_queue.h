#ifndef _TOYOS_RIO_QUEUE_H
#define _TOYOS_RIO_QUEUE_H

#include <kernel/stdint.h>
#define RIO_QUEUE_MAXLEN 1024
struct rio_buf_queue {
    char buf[RIO_QUEUE_MAXLEN];
    uint64_t tail_counter; //锁机制抢的
    uint64_t tail; 
    uint64_t head;
    uint64_t head_counter;
};

void write_to_buf(struct rio_buf_queue* queue,const char* str, size_t len);
char read_from_buf(struct rio_buf_queue* queue);
#endif