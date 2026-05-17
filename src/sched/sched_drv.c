#include <kernel/sched/sched.h>
#include <kernel/sched/sched_drv.h>
#include <kernel/base/linklist.h>
#include <kernel/kernel.h>
#include <kernel/put.h>

#define drv_class_of(rq) container_of(rq, struct sched_drv_fifo, scheduler)

static DEFINE_PERCPU_VAR(drv_sched, struct sched_drv_fifo);

static void fifo_sched_init(struct scheduler* scheduler) {
    struct sched_drv_fifo *base = drv_class_of(scheduler);
    INIT_LIST_HEAD(&base->run_queue.head);
    INIT_LIST_HEAD(&base->sleep);
    base->run_queue.tail = &base->run_queue.head;
    scheduler->s_flag |= SCHED_FLAG_DRIVERTYPE; 
}

static struct task_struct* fifo_pick_next_task(struct scheduler* scheduler) {
    struct sched_drv_fifo *fifo = drv_class_of(scheduler);
    if(&fifo->run_queue.head == fifo->run_queue.tail)
        return NULL;
    struct task_struct* task = container_of(list_head(&fifo->run_queue.head), struct task_struct, sibling);
    if(&task->sibling == fifo->run_queue.tail)
        fifo->run_queue.tail = task->sibling.prev;
    list_del_init(&task->sibling);
    return task;
}

static void fifo_sched_r_to_s(struct task_struct* task) {
    struct sched_drv_fifo *fifo = drv_class_of(task->scheduler);
    list_insert_rcu(&task->sibling, &fifo->sleep);
}

static void fifo_sched_s_to_r(struct task_struct* task) {
    struct sched_drv_fifo *fifo = drv_class_of(task->scheduler);
    list_del_init(&task->sibling);
    list_insert_rcu(&task->sibling, fifo->run_queue.tail);
    fifo->run_queue.tail = &task->sibling;
}

static void fifo_sched_enqueue(struct scheduler *sched, struct task_struct *t) {
    struct sched_drv_fifo *fifo = drv_class_of(sched);
    list_insert_rcu(&t->sibling, fifo->run_queue.tail);
    fifo->run_queue.tail = &t->sibling;
}

static struct sched_class sc = {
    .sched_init = fifo_sched_init,
    .task_sched_next_task = fifo_pick_next_task,
    .task_sched_enqueue = fifo_sched_enqueue,
    .task_fork_enqueue = fifo_sched_enqueue,
    .task_r_to_s = fifo_sched_r_to_s,
    .task_s_to_r = fifo_sched_s_to_r
};

void register_drv_sched() {
    struct sched_drv_fifo *fifo = THIS_CPU_PTR(drv_sched);
    register_scheduler(&fifo->scheduler, sc , SCHED_PRIO_DRV);
}

void switch_to_drv_sched(struct task_struct* task) { 
    struct task_struct* current = CURRENT_PROCESS();

    struct sched_drv_fifo *base = THIS_CPU_PTR(drv_sched);
    if(&base->scheduler == task->scheduler)
        return;
    preempt_disable();
    task->scheduler->s_class.task_smp_dequeue(task->scheduler, task);
    task->scheduler = &base->scheduler;
    if(current != task)
        task->scheduler->s_class.task_sched_enqueue(task->scheduler,task);
    preempt_enable();
    //需要手动调用 schedule
    barrier();
}

