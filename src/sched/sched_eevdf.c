#include <kernel/sched/sched.h>
#include <kernel/sched/sched_eevdf.h>
#include <kernel/config.h>
#include <kernel/base/math.h>

DEFINE_PERCPU_VAR(eevdf_sched, struct sched_eevdf);

#define eevdf_of(rq) container_of(rq, struct sched_eevdf, scheduler)
#define TASK_WEIGH(task) ((task->nice_level) + 30)

static uint64_t weigh[41] = {
    858993459200,780903144727,715827882666,660764199384,
    613566756571,572662306133,536870912000,505290270117,
    477218588444,452101820631,429496729600,409044504380,
    390451572363,373475417043,357913941333,343597383680,
    330382099692,318145725629,306783378285,296204641103,
    286331153066,277094664258,268435456000,260301048242,
    252645135058,245426702628,238609294222,232160394378,
    226050910315,220254733128,214748364800,209510599804,
    204522252190,199765920744,195225786181,190887435377,
    186737708521,182764565787,178956970666,175304787591,
    168430090039
};

static inline uint64_t task_get_weigh(uint64_t value, struct task_struct* task) {
    uint64_t target = (weigh[task->nice_level + 20] * value);
    return target >> 32;
}

static void eevdf_update_min_vd(struct rb_node* node) {
    struct task_struct* task = container_of(node, struct task_struct, rb_node);
    uint64_t vdeadtime = task->min_vdeadtime;
    struct task_struct *target;
    if(node->rb_left != NULL) {
        target = container_of(node->rb_left, struct task_struct, rb_node);
        vdeadtime = min(vdeadtime, target->min_vdeadtime);
    }

    if(node->rb_right != NULL) {
        target = container_of(node->rb_right, struct task_struct, rb_node);
        vdeadtime = min(vdeadtime, target->min_vdeadtime);
    }

    task->min_vdeadtime = vdeadtime;
}

static struct task_struct* eevdf_next_task(struct scheduler* sched, struct task_struct* prev) {
    struct sched_eevdf* eevdf = eevdf_of(sched);
    eevdf->vcputime += div_32bit(SCHED_TIMER_IRQ_VRUNTIME, eevdf->eevdf_sum_weigh);
    prev->vruntime += task_get_weigh(SCHED_TIMER_IRQ_VRUNTIME, prev);

    if(RB_EMPTY_ROOT(&eevdf->running_task)) {
        return NULL;
    }

    struct rb_node* current = eevdf->running_task.rb_node;
    struct task_struct* new_task;
    while(current) {
        new_task = container_of(current,struct task_struct, rb_node);
        if(new_task->vruntime > eevdf->vcputime) 
            current = current->rb_left;
        else
            break;
    }

    struct task_struct *left, *right;
    while(current) {
        if(new_task->vruntime > eevdf->vcputime) {
            current = current->rb_left;
            break;
        }

        if(new_task->vdeadtime == new_task->min_vdeadtime) {
            struct rb_node* node = rb_augment_erase_begin(&new_task->rb_node);
            rb_erase(&new_task->rb_node, &eevdf->running_task);
            if(node != NULL)
                rb_augment_erase_end(node, eevdf_update_min_vd, NULL);
        }

        if(current->rb_left != NULL) 
            left = container_of(current->rb_left,struct task_struct, rb_node);
        if(current->rb_right != NULL)
            right = container_of(current->rb_right,struct task_struct, rb_node);

    }

}

static bool eevdf_sched_has_task(struct scheduler* sched) {
    struct sched_eevdf* eevdf = eevdf_of(sched);
    return RB_EMPTY_ROOT(&eevdf->running_task);
}

static void eevdf_task_enqueue(struct scheduler* sched, struct task_struct* task) {
    struct sched_eevdf *eevdf = eevdf_of(sched);
    struct rb_node* node = eevdf->running_task.rb_node;
    
    uint64_t slice = task_get_weigh(task->request_time, task);
    task->vdeadtime = task->vruntime + slice;
    
    rb_init_node(&task->rb_node);
    if(node == NULL) {
        rb_link_node(&task->rb_node, NULL, node);
        goto eevdf_enqueue_end;
    }
    
    
eevdf_enqueue_end:
    task->vruntime = eevdf->vcputime - task->vruntime;
}

static void eevdf_sched_init(struct scheduler* scheduler) {
    struct sched_eevdf* eevdf = eevdf_of(scheduler);
    eevdf->vcputime = 0;
    eevdf->running_task.rb_node = NULL;
    eevdf->sleeping_task.rb_node = NULL;
}


static struct sched_class sc = {
    .next_task = eevdf_next_task,
    .sched_init = eevdf_sched_init,
    .task_enqueue = eevdf_task_enqueue,
    .sched_has_task = eevdf_sched_has_task
};

void register_eevdf() {
    struct sched_eevdf* eevdf = THIS_CPU_PTR(eevdf_sched);
    register_scheduler(&eevdf->scheduler, sc, SCHED_LEVEL_L4);
}

