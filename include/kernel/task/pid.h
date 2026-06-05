#ifndef _TOYOS_TASK_PID
#define _TOYOS_TASK_PID

#include <kernel/base/linklist.h>
#include <kernel/base/bitmap.h>
#include <kernel/task/task.h>
#include <kernel/atomic/atomic.h>
#include <kernel/base/htable.h>

struct task_struct;

struct pid_nr {
    struct bitmap pid_bitmap; //pid位图
    struct htable_list task_hash;
    atomic_t nr_ref;
};

struct pid_ns_layer {
    int pid_num;
    struct linklist_head sibling;
    struct pid_nr* nr;
};

struct pid {
    uint32_t pid_layers;
    struct pid_ns_layer layer[];
};

void init_pid_ns_layer(struct pid_ns_layer* layer);
void init_new_pid_nr(struct pid_nr* nr);
void alloc_pid(struct task_struct* task, uint64_t flags);
uint64_t pid_of(struct task_struct* task);
#endif