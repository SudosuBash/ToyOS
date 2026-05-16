#include <kernel/stdint.h>
#include <kernel/base/rio_queue.h>
#include <kernel/atomic/atomic.h>
#include <kernel/mm/mm.h>
#include <kernel/cpu/archimpl.h>
#include <kernel/stdlib.h>

//不提供 empty 方法，因为压根不精确

//无锁队列
void write_to_buf(struct rio_buf_queue* queue, const char* str, size_t len) {
    preempt_disable();
    uint64_t tail, target_tail;
    do {
        tail = queue->tail_counter;
        target_tail = tail + len;
        if(target_tail - queue->head >= RIO_QUEUE_MAXLEN)
            return;
    } while (atomic_cas(&queue->tail_counter, target_tail, tail) == 0);

    //非常巧妙
    //head 和 tail 都不用取mod, 只需要在这取mod就好了
    for(uint64_t index = 0; index < len; index++) {
        queue->buf[(tail + index) & (RIO_QUEUE_MAXLEN - 1)] = str[index];
    }
    smp_wmb();
    //上面的对 buf 的写入和下面的写入tail有没有关系? 没有!
    //Write-write重排序!
    //必须加smp_wmb
    while (atomic_cas(&queue->tail, target_tail, tail) == 0) {
        pause();
    }
    preempt_enable();
    //因为queue->tail和tail不相等的时候, 它拒绝递增
    //所以间接保证了顺序
}

char read_from_buf(struct rio_buf_queue* queue) {
    uint64_t head, buf_head;
    do {
        head = queue->head_counter;
        buf_head = head + 1;
        if(queue->tail - head == 0)
            return 0;
    } while(atomic_cas(&queue->head_counter, buf_head, head) == 0);

    char ch = queue->buf[(head) & (RIO_QUEUE_MAXLEN - 1)];
    barrier();

    while(atomic_cas(&queue->head, buf_head, head) == 0) {
        pause();
    }
    return ch;
}