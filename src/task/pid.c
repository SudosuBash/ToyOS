#include <kernel/task/pid.h>
#include <kernel/base/bitmap.h>
#include <kernel/base/linklist.h>
#include <kernel/mm/mm.h>
#include <kernel/task/fork.h>
#include <kernel/stdlib.h>

static uint64_t alloc_nr_pid(struct pid_nr* nr) {
    uint64_t new_pid = bitmap_find_first_zero(&nr->pid_bitmap);
    bitmap_bit_to_1(&nr->pid_bitmap, new_pid);
    return new_pid;
}

inline void init_new_pid_nr(struct pid_nr* nr) {
    init_bitmap(&nr->pid_bitmap, CONFIG_GROUP_MAX_CNT);
    bitmap_bit_to_1(&nr->pid_bitmap, 0);
    hlist_init(&nr->task_hash);
}

inline void init_pid_ns_layer(struct pid_ns_layer* layer, struct task_struct* target) {
    INIT_LIST_HEAD(&layer->sibling);
    atomic_inc(&layer->nr->nr_ref);
    layer->target_task = target;
}

static inline void copy_pid_nr(struct pid_ns_layer* old,struct pid_ns_layer* new, struct task_struct* target) {
    new->nr = old->nr;
    init_pid_ns_layer(new, target);
    new->pid_num = alloc_nr_pid(new->nr);
    hlist_insert(&new->nr->task_hash, &new->sibling, new->pid_num);
}

void alloc_pid(struct task_struct* task, uint64_t flags) {
    struct pid* parent = task->pid; //保存父进程的指针
    uint64_t layers = parent->pid_layers;
    if(flags & CLONE_NEW_PID_NS) 
        layers += 1;
    struct pid* new = kmalloc(sizeof(struct pid) + (layers) * sizeof(struct pid_ns_layer), GFP_KERNEL);
    memset(new, 0, sizeof(struct pid));
    task->pid = new;
    task->pid->pid_layers = layers;

    if(flags & CLONE_NEW_PID_NS) {
        struct pid_ns_layer* last_layer = &new->layer[layers-1];
        last_layer->nr = kmalloc(sizeof(struct pid_nr), GFP_KERNEL);
        memset(last_layer->nr, 0, sizeof(struct pid_nr));
        init_new_pid_nr(last_layer->nr);
        init_pid_ns_layer(last_layer, task);
        last_layer->pid_num = alloc_nr_pid(last_layer->nr);
        hlist_insert(&last_layer->nr->task_hash, &last_layer->sibling, last_layer->pid_num);
    }

    for(int i=parent->pid_layers-1;i>=0;i--) {
        copy_pid_nr(&parent->layer[i],&new->layer[i], task);
    }
}

uint64_t pid_of(struct task_struct* task) {
    return task->pid->layer[0].pid_num;
}