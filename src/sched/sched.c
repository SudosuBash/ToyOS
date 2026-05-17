#include <kernel/sched/sched.h>
#include <kernel/cpu/smp.h>
#include <kernel/sched/sched.h>
#include <kernel/fault/fault.h>
#include <kernel/cpu/archimpl.h>
#include <kernel/sched/sched_eevdf.h>
#include <kernel/sched/sched_drv.h>

struct se_info {
    struct linklist_head lhead;
    struct linklist_head *ltail;
};

DEFINE_PERCPU_VAR(se_info, struct se_info);

void task_switch_stat(struct task_struct *task, task_stat_t new_stat) {
    preempt_disable();
    struct scheduler* m = task->scheduler;
    switch(new_stat) {
        case TASK_BLOCKED_SLEEP_STAT: {
            if(task->status == TASK_RUNNING_STAT) 
                m->s_class.task_r_to_s(task);
            task->status = TASK_BLOCKED_SLEEP_STAT;
            break;
        }
        case TASK_SIGNAL_SLEEP_STAT: {
            if(task->status == TASK_RUNNING_STAT) 
                m->s_class.task_r_to_s(task);
            task->status = TASK_SIGNAL_SLEEP_STAT;
            break;
        }

        case TASK_RUNNING_STAT: {
            if(task->status == TASK_SIGNAL_SLEEP_STAT)  
                m->s_class.task_s_to_r(task);
            else if(task->status == TASK_BLOCKED_SLEEP_STAT)
                m->s_class.task_s_to_r(task);
            task->status = TASK_RUNNING_STAT;
            break;
        }
    }
    preempt_enable();
}

void current_task_switch_stat(task_stat_t new_stat) {
    preempt_disable();
    struct task_struct* current = CURRENT_PROCESS();
    task_switch_stat(current, new_stat);
    preempt_enable();
    schedule(); //当前任务切换状态后, 直接调用 schedule 换走任务
}

//让 callee 关中断
struct task_struct* pick_next_task() {
    struct se_info* info = THIS_CPU_PTR(se_info);
    struct scheduler *sched, *target;
    struct linklist_head* curr;

    assert(!list_empty(&info->lhead));

pick_scheduler_start:
    sched = container_of(list_head(&info->lhead), struct scheduler, s_sibling);
    list_del_init(&sched->s_sibling);
    
    struct task_struct* next = sched->s_class.task_sched_next_task(sched);

    if(list_empty(&info->lhead)) { //几乎不可能发生, 防御性编程
        list_insert(&sched->s_sibling, info->ltail);
        info->ltail = &sched->s_sibling;
        if(next == NULL) { //这是彻底没任务了
            hlt(); //那直接睡吧, 等任务
            goto pick_scheduler_start;
        }
        goto pick_scheduler_end;
    }
    
    sched->s_count += sched->s_level;
    
    if(!next) {
        target = container_of(info->ltail, struct scheduler, s_sibling);
        if(!(sched->s_flag & SCHED_FLAG_DRIVERTYPE)) {
            sched->s_count = target->s_count + 1;
            list_insert(&sched->s_sibling, info->ltail);
            info->ltail = &sched->s_sibling;
        } else {
            sched->s_flag |= SCHED_FLAG_TEMPORATORY_REMOVED;
            //逻辑是这样的, 要是调度驱动程序的调度器没任务了, 直接删了, 后续驱动触发了再挂回来
            //没必要设计成通用接口, 这属于过度设计
        }
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
    return next;
}

void register_scheduler(struct scheduler* sched, struct sched_class sc, uint16_t level) {
    struct se_info* info = THIS_CPU_PTR(se_info);
    struct scheduler* tsched;
    
    preempt_disable();
    sched->s_level = level;
    sched->s_class = sc;
    INIT_LIST_HEAD(&sched->s_sibling);
    list_insert_rcu(&sched->s_sibling, &info->lhead);
    if(info->ltail == &info->lhead) { //无调度器
        sched->s_count = 0;
        info->ltail = &sched->s_sibling;
    } else { //有调度器
        tsched = container_of(list_head(&info->lhead), struct scheduler, s_sibling);
        sched->s_count = tsched->s_count;
    }
    sched->s_class.sched_init(sched); // 触发 sched_init 事件初始化调度器
    preempt_enable();
}

void activate_driver_scheduler(struct scheduler* sched) {
    struct se_info* info = THIS_CPU_PTR(se_info);
    struct scheduler* tsched;
    if(!(sched->s_flag & SCHED_FLAG_TEMPORATORY_REMOVED))
        return;
    preempt_disable();
    sched->s_flag &= ~(SCHED_FLAG_TEMPORATORY_REMOVED);
    INIT_LIST_HEAD(&sched->s_sibling);
    list_insert_rcu(&sched->s_sibling, &info->lhead);

    assert(info->ltail != &info->lhead);
    //这个必定有任务，这可是激活的任务诶
    tsched = container_of(list_head(&info->lhead), struct scheduler, s_sibling);
    sched->s_count = tsched->s_count; //重置步长
    preempt_enable();
}

//核心初始化的时候调用的, 所以这个函数没必要preempt disable
void init_scheduler() {
    struct se_info* info = THIS_CPU_PTR(se_info);
    INIT_LIST_HEAD(&info->lhead);
    info->ltail = &info->lhead;
    register_eevdf();
    register_idle();
    register_drv_sched();
}