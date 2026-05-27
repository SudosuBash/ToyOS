#include <kernel/base/rio_broadcast.h>
#include <kernel/atomic/atomic.h>
#include <kernel/cpu/archimpl.h>
#include <kernel/def.h>
#include <kernel/kernel.h>
#include <kernel/limit.h>

#define RIO_DANGER_DELTA 16

void rio_broadcast_send(struct rio_broadcast* broadcast, const char* str, size_t len) {
    uint64_t tail, new_tail;
    struct rio_reader *target_reader;
    do {
        tail = broadcast->tail_counter;
        new_tail = tail + len;
        
        if(new_tail - broadcast->slowest_head >= PRINT_BUF_LEN - RIO_DANGER_DELTA) {
            struct linklist_head *curr;
            uint64_t slowest = ULLONG_MAX;
            //这个能保证遍历的时候不出错, 因为只有插入, 插入的逻辑保证链表不断.
            list_for_entry(&broadcast->readers, curr) {
                target_reader = container_of(curr, struct rio_reader, sibling);
                if(target_reader->head < slowest) {
                    slowest = target_reader->head;
                }
            }
            broadcast->slowest_head = slowest;
        }
    } while(atomic_cas(&broadcast->tail_counter, new_tail, tail) == 0);

    if(new_tail - broadcast->slowest_head >= PRINT_BUF_LEN)
        return;

    for(int i=0;i<len;i++)
        broadcast->buf[(tail+i) & (PRINT_BUF_LEN - 1)] = str[i];

    smp_wmb();
    while(atomic_cas(&broadcast->tail, new_tail, tail) == 0)
        pause(); //比 schedule 更加好, 因为开销小
}

char rio_broadcast_recv(struct rio_broadcast* broadcast, struct rio_reader* reader) {
    uint64_t head = reader->head;
    if(broadcast->tail == head)
        return 0;
    char ch = broadcast->buf[(head) & (PRINT_BUF_LEN - 1)];
    reader->head++;
    return ch;
}

void init_rio_broadcast(struct rio_broadcast* broadcast) {
    INIT_LIST_HEAD(&broadcast->readers);
    broadcast->slowest_head = 0;
    broadcast->tail = 0;
    broadcast->tail_counter = 0;
}

void register_reader_broadcast(struct rio_broadcast* broadcast, struct rio_reader* reader) {
    INIT_LIST_HEAD(&reader->sibling);
    reader->head = broadcast->slowest_head;
    list_insert(&reader->sibling, &broadcast->readers);
}