#include <kernel/stdint.h>
#include <kernel/base/rio_queue.h>
#include <kernel/atomic/atomic.h>
#include <kernel/mm/mm.h>
#include <kernel/cpu/archimpl.h>
#include <kernel/stdlib.h>
#include <kernel/base/math.h>

//不提供 empty 方法，因为压根不精确

//无锁队列
void write_to_buf(struct rio_buf_queue* queue, const char* str, size_t len) {
    uint64_t tail, target_tail;
    do {
        tail = queue->tail_counter;
        target_tail = tail + len;
    } while (atomic_cas(&queue->tail_counter, target_tail, tail) == 0);
    
    if(target_tail - queue->head >= RIO_QUEUE_MAXLEN) {
        return;
    }
    //非常巧妙
    //head 和 tail 都不用取mod, 只需要在这取mod就好了
    for(int index = 0; index < len; index++) {
        queue->buf[(tail + index) & (RIO_QUEUE_MAXLEN - 1)] = str[index];
    }
    smp_wmb();
    /**
     * 我们的目的是在写入数据写入完前, 任何人读取 tail 指针都必须得数据写完再读.
     * 上面的对 buf 的写入和下面的写入tail有没有关系? 没有!
     * Write-write重排序, 下面的移动到上面, 还符合我们的目的吗?
     * 必须加smp_wmb
     */
    while (atomic_cas(&queue->tail, target_tail, tail) == 0) {
        pause();
    }
    //因为queue->tail和tail不相等的时候, 它拒绝递增
    //所以间接保证了顺序
}

char read_from_buf(struct rio_buf_queue* queue) {
    uint64_t head, buf_head;
    do {
        head = queue->head;
        buf_head = head + 1;
        if(queue->tail - head == 0)
            return 0;
    } while(atomic_cas(&queue->head_counter, buf_head, head) == 0);

    char ch = queue->buf[(head) & (RIO_QUEUE_MAXLEN - 1)];
    smp_rmb();

    while(atomic_cas(&queue->head, buf_head, head) == 0) 
        pause();
    return ch;
}