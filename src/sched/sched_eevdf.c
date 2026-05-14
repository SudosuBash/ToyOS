#include <kernel/sched/sched.h>
#include <kernel/sched/sched_eevdf.h>
#include <kernel/config.h>
#include <kernel/base/math.h>
#include <kernel/timer/timer.h>
#include <kernel/put.h>

#if defined(CONFIG_EEVDF)

DEFINE_PERCPU_VAR(eevdf_sched, struct sched_eevdf);

#define eevdf_of(rq) container_of(rq, struct sched_eevdf, scheduler)
#define TASK_WEIGH(task) ((task->nice_level) + 30)

static uint64_t weigh[41] = {
    429496729,390451572,357913941,330382099,
    306783378,286331153,268435456,252645135,
    238609294,226050910,214748364,204522252,
    195225786,186737708,178956970,171798691,
    165191049,159072862,153391689,148102320,
    143165576,138547332,134217728,130150524,
    126322567,122713351,119304647,116080197,
    113025455,110127366,107374182,104755299,
    102261126,99882960,97612893,95443717,
    93368854,91382282,89478485,87652393,84215045
};

static void __eevdf_update_min_vd(struct rb_node* node, void* data);

static inline uint64_t __task_get_weigh(uint64_t value, struct task_struct* task) {
    uint64_t target = (weigh[task->nice_level + 20] * value);
    uint64_t res = target >> 32;
    return res;
}

static inline void __erase_task_queue(struct task_struct* task, struct rb_root* root) {
    struct rb_node *node = rb_augment_erase_begin(&task->rb_node);
    rb_erase(&task->rb_node, root);
    if(node != NULL) 
        rb_augment_erase_end(node, __eevdf_update_min_vd, NULL);
}

static inline void __update_vdeadtime(struct task_struct *task) {
    task->vdeadtime = task->vruntime + __task_get_weigh(task->request_time, task);
}

static inline void __update_vcputime(struct sched_eevdf *eevdf, uint64_t current) {
    if(eevdf->e_flag & EEVDF_FLAG_TASK_SELECTED) {
        uint64_t delta = current - eevdf->last_timestamp;
        if(eevdf->eevdf_sum_weigh != 0)
            eevdf->vcputime += div_32bit(delta, eevdf->eevdf_sum_weigh);
    }
    eevdf->last_timestamp = current;
}

//更新 vruntime, vdeadtime
static inline void __update_sched_vruntime(struct task_struct* task, uint64_t current) {
    uint64_t delta = current - task->last_runtime;
    task->vruntime += __task_get_weigh(delta, task);
    __update_vdeadtime(task);
}

static void __insert_into_queue(struct task_struct* task, struct rb_root* root) {
    struct rb_node* node = root->rb_node;
    struct task_struct* parent;
    rb_init_node(&task->rb_node);
    if(node == NULL) {
        rb_link_node(&task->rb_node, NULL, &root->rb_node);
        task->min_vdeadtime = task->vdeadtime;
        return;
    }

    while(node) {
        parent = container_of(node, struct task_struct, rb_node);
        if(signed_bigger(parent->vruntime , task->vruntime))
            node = node->rb_left;
        else 
            node = node->rb_right;
    }

    if(signed_bigger(parent->vruntime, task->vruntime))
        rb_link_node(&task->rb_node, &parent->rb_node, &parent->rb_node.rb_left);
    else
        rb_link_node(&task->rb_node, &parent->rb_node, &parent->rb_node.rb_right);
    rb_augment_insert(&task->rb_node, __eevdf_update_min_vd, NULL);
}

static void __eevdf_update_min_vd(struct rb_node* node, void* data) {
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

static struct task_struct* eevdf_sched_next_task(struct scheduler* sched) {
    struct sched_eevdf* eevdf;
    struct rb_node *current;
    struct task_struct *new_task, *left, *last_fit = NULL;

    eevdf = eevdf_of(sched);
    if(RB_EMPTY_ROOT(&eevdf->run_task_queue)) {
        return NULL;
    }

    uint64_t current_time = get_current_tstamp();
    __update_vcputime(eevdf, current_time);
    
    current = eevdf->run_task_queue.rb_node;
    while(current) {
        new_task = container_of(current,struct task_struct, rb_node);
        if(signed_bigger(new_task->vruntime, eevdf->vcputime)) 
            current = current->rb_left;
        else
            break;
    }
    if(current != NULL) {
        last_fit = new_task;
        while(current) {
            new_task = container_of(current, struct task_struct, rb_node);
            if(signed_bigger(new_task->vruntime, eevdf->vcputime)) {
                current = current->rb_left;
                continue;
            } else {
                if(signed_bigger(new_task->min_vdeadtime, last_fit->min_vdeadtime))
                //new_task 的最短截止时间比 last_fit 大的话, 那last_fit就是最小的
                //直接 break 就好, 用 last_fit, 不用找了
                    break;
                last_fit = new_task;
            }

            if(new_task->vdeadtime == new_task->min_vdeadtime)
                break;

            if(current->rb_left != NULL) 
                left = container_of(current->rb_left,struct task_struct, rb_node);
            
            if(current->rb_left && left->min_vdeadtime == new_task->min_vdeadtime)
                current = current->rb_left;
            else
                current = current->rb_right;
        }
    
    }

    if(last_fit != NULL) {
        eevdf->e_flag |= EEVDF_FLAG_TASK_SELECTED;
        __erase_task_queue(last_fit,&eevdf->run_task_queue);
        eevdf->eevdf_sum_weigh -= TASK_WEIGH(last_fit); //减少权重
        last_fit->last_runtime = current_time; 
        //更新 last_runtime 的那一刻, 程序开始运行
        return last_fit;
    }
    return NULL;
}
//从 running 切换到 sleep
static void eevdf_task_r_switch(struct task_struct* task) {
    struct scheduler *sched = task->scheduler;
    struct sched_eevdf *eevdf = eevdf_of(sched);
    uint64_t current = get_current_tstamp();

    __update_vcputime(eevdf, current); //vcputime 更新的原因是, 权重变少了
    // * 顺序不能换, 因为此时此刻要计算 lag, 所以 delta 就是 vcputime - vruntime
    task->vruntime = eevdf->vcputime - task->vruntime; 
    //此时 vruntime 代表 lag
    __erase_task_queue(task, &eevdf->run_task_queue); 
    eevdf->eevdf_sum_weigh -= TASK_WEIGH(task); //减少权重

    __insert_into_queue(task, &eevdf->sleep_task_queue);
    //清除任务, 此时此刻任务不再红黑树上了
}

static void eevdf_task_switch_to_r(struct task_struct* task) {
    
}

static void eevdf_smp_enqueue(struct scheduler* sched, struct task_struct* task) {

}

//这个是 schedule 推入的task_enqueue
//此时此刻应该更新 vruntime, vdeadtime
//因为是 schedule 推入的, 所以加入的时间是任务真实运行时间
static void eevdf_task_sched_enqueue(struct scheduler* sched, struct task_struct* task) {
    struct sched_eevdf *eevdf = eevdf_of(sched);

    uint64_t current = get_current_tstamp();
    eevdf->eevdf_sum_weigh += TASK_WEIGH(task);
    __update_vcputime(eevdf, current); //结束一轮执行, 更新 vcputime
    __update_sched_vruntime(task, current); //结束一轮执行, 更新 vruntime

    __insert_into_queue(task, &eevdf->run_task_queue); //插入队列
    eevdf->e_flag &= ~EEVDF_FLAG_TASK_SELECTED;
}

static void eevdf_sched_init(struct scheduler* scheduler) {
    struct sched_eevdf* eevdf = eevdf_of(scheduler);
    eevdf->vcputime = 0;
    eevdf->run_task_queue.rb_node = NULL;
    eevdf->sleep_task_queue.rb_node = NULL;
}

static void eevdf_task_fork_enqueue(struct scheduler* sched, struct task_struct* task) {  
    struct sched_eevdf* eevdf = eevdf_of(sched);

    uint64_t current = get_current_tstamp();
    eevdf->eevdf_sum_weigh += TASK_WEIGH(task);
    /**
     * 这儿其实做了一点性能优化
     * 原来的逻辑:
     * 1. 加上旧任务的权重
     * 2. 更新 vcputime 的记账和调度前的逻辑
     * 3. 减去旧任务的权重
     * 4. 加上新任务的权重
     * 但是新任务的权重=旧任务的权重(fork), 3 4 抵消
     * 所以 3 4 不用做
    */
    __update_vcputime(eevdf, current); //更新虚拟时间

    task->vruntime = eevdf->vcputime;
    __update_vdeadtime(task);
    __insert_into_queue(task, &eevdf->run_task_queue);
    // __update_sched_vruntime(task, current); //更新sched vruntime
}

static struct sched_class sc = {
    .task_sched_next_task = eevdf_sched_next_task,
    .sched_init = eevdf_sched_init,
    .task_sched_enqueue = eevdf_task_sched_enqueue,
    .task_smp_enqueue = eevdf_smp_enqueue,
    .task_fork_enqueue = eevdf_task_fork_enqueue,
    .task_r_to_bs = eevdf_task_r_switch,
    .task_r_to_ss = eevdf_task_r_switch,
    .task_bs_to_r = eevdf_task_switch_to_r,
    .task_ss_to_r = eevdf_task_switch_to_r
};

void set_eevdf_sched(struct task_struct* task) {
    struct sched_eevdf* eevdf = THIS_CPU_PTR(eevdf_sched);
    task->scheduler = &eevdf->scheduler;
    task->nice_level = 0;
    task->request_time = 100000; //暂时先这么定, 还需要初始化 pic
}

void register_eevdf() {
    struct sched_eevdf* eevdf = THIS_CPU_PTR(eevdf_sched);
    eevdf->last_timestamp = get_current_tstamp();
    register_scheduler(&eevdf->scheduler, sc, SCHED_PRIO_HIIGH);
}
#endif