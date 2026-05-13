#include <kernel/sched/sched.h>
#include <kernel/cpu/smp.h>
#include <kernel/sched/sched.h>
#include <kernel/fault/fault.h>

struct se_info {
    struct linklist_head lhead;
    struct linklist_head *ltail;
};

DEFINE_PERCPU_VAR(se_info, struct se_info);

void sched_task_switch_stat(struct task_struct *task, task_stat_t new_stat) {
    preempt_disable();
    struct scheduler* m = task->scheduler;
    switch(new_stat) {
        case TASK_BLOCKED_SLEEP_STAT: {
            if(task->status == TASK_RUNNING_STAT) 
                m->s_class.task_r_to_bs(task);
            task->status = TASK_BLOCKED_SLEEP_STAT;
        }
        case TASK_SIGNAL_SLEEP_STAT: {
            if(task->status == TASK_RUNNING_STAT) 
                m->s_class.task_r_to_ss(task);
            task->status = TASK_SIGNAL_SLEEP_STAT;
        }

        case TASK_RUNNING_STAT: {
            if(task->status == TASK_SIGNAL_SLEEP_STAT)  
                m->s_class.task_ss_to_r(task);
            else if(task->status == TASK_BLOCKED_SLEEP_STAT)
                m->s_class.task_bs_to_r(task);
            task->status = TASK_RUNNING_STAT;
        }
    }
    preempt_enable();
}

void current_sched_task_switch_stat(task_stat_t new_stat) {
    preempt_disable();
    struct task_struct* current = CURRENT_PROCESS();
    sched_task_switch_stat(current, new_stat);
    preempt_enable();
}

struct scheduler* pick_scheduler() {
    struct se_info* info = THIS_CPU_PTR(se_info);
    struct scheduler *sched, *target;
    struct linklist_head* curr;

    assert(!list_empty(&info->lhead));
    preempt_disable(); //rmw干mesi, 这玩意只需要关中断, 性价比还是后者高
pick_scheduler_start:
    sched = container_of(list_head(&info->lhead), struct scheduler, s_sibling);
    list_del(sched);

    sched->s_count += sched->s_level;

    if(list_empty(&info->lhead)) {
        assert(sched->s_class.sched_has_task(sched));
        list_insert(&sched->s_sibling, info->ltail);
        info->ltail = &sched->s_sibling;
        goto pick_scheduler_end;
    }

    if(!sched->s_class.sched_has_task(sched)) {
        target = container_of(info->ltail, struct scheduler, s_sibling);
        sched->s_count = target->s_count + 1;
        list_insert(&sched->s_sibling, info->ltail);
        info->ltail = &sched->s_sibling;
        goto pick_scheduler_start;
    }
    
    list_for_entry(&info->lhead, curr) {
        target = container_of(curr, struct scheduler, s_sibling);
        if((int64_t)(target->s_count - sched->s_count) > 0) {
            list_insert(&sched->s_sibling, curr->prev);
            goto pick_scheduler_end;
        }
    }
    if(curr == &info->lhead) {
        list_insert(&sched->s_sibling, info->ltail);
        info->ltail = &sched->s_sibling;
    }
pick_scheduler_end:
    preempt_enable();
    return sched;
}

void register_scheduler(struct scheduler* sched, struct sched_class sc, uint16_t level) {
    struct se_info* info = THIS_CPU_PTR(se_info);
    struct scheduler* tsched;
    
    preempt_disable();
    sched->s_level = level;
    sched->s_class = sc;
    INIT_LIST_HEAD(&sched->s_sibling);
    list_insert_rcu(&info->lhead, &sched->s_sibling);
    if(info->ltail == &info->lhead) { //无进程
        sched->s_count = 0;
        info->ltail = &sched->s_sibling;
    } else { //有进程
        tsched = container_of(list_head(&info->lhead), struct scheduler, s_sibling);
        sched->s_count = tsched->s_count;
    }
    sched->s_class.sched_init(sched); // 触发 sched_init 事件初始化调度器
    preempt_enable();
}

//核心初始化的时候调用的, 所以这个函数没必要preempt disable
void init_scheduler() {
    struct se_info* info = THIS_CPU_PTR(se_info);
    INIT_LIST_HEAD(&info->lhead);
    info->ltail = &info->lhead;
}