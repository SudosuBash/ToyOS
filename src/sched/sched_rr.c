#include <kernel/sched/sched.h>
#include <kernel/cpu/smp.h>
#include <kernel/mm/mm.h>
#include <kernel/task/task_manager.h>
#include <kernel/data_struct/general.h>

//暂时先实现rr算法, 先跑起来, 我就做一个实验, 后续实现mlfq
struct task_struct* next_task_rr() {
    struct cpu_task_manager* manager = get_cpu_manager();
    if(list_empty(&manager->task_head)) return NULL;
    struct linklist_head* next = list_head(&manager->task_head);
    struct task_struct* head = container_of(next, struct task_struct, sibling);
    list_del(next);
    return head;
}

void sched_init_rr() {
    
}

void task_enqueue_rr(struct task_struct* entity) {
    struct cpu_task_manager* manager = get_cpu_manager();
    list_insert(&entity->sibling,&manager->task_head);
}


struct sched_class rr_se = {
    .next_task = next_task_rr,
    .sched_class_init = sched_init_rr,
    .task_enqueue = task_enqueue_rr
};