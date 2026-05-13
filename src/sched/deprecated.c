/**
 * 数据规模小于 10 的情况, 链表比红黑树快得多
*/
// #include <kernel/sched/scheduler.h>
// #include <kernel/cpu/smp.h>
// #include <kernel/irq/timer.h>
// #include <kernel/base/linklist.h>
// #include <kernel/fault/fault.h>
// #include <kernel/config.h>

// struct se_info {
//     struct rb_root root;
//     struct rb_node* rb_min;
// };

// DEFINE_PERCPU_VAR(se_info, struct se_info);

// extern struct sched_class rr_se;


// struct scheduler* pick_scheduler() {
//     struct se_info* info = THIS_CPU_PTR(se_info);
//     struct scheduler *sched, *target;
//     struct rb_node* next;
    
//     assert(!RB_EMPTY_ROOT(&info->root));
//     preempt_disable();
// pick_scheduler_start:
//     next = rb_next(info->rb_min);
//     sched = container_of(info->rb_min, struct scheduler, s_sibling);

//     if(next == NULL) //万一只有一个调度器, 那直接炸了, 防止这种情况.
//         return sched;

//     target = container_of(next, struct scheduler, s_sibling);
//     sched->s_count += sched->s_level;

//     if((long)(target->s_count - sched->s_count) > 0) { //原本就够小的话, 没必要加了
//         if(sched->s_class.sched_has_task(target))
//             return sched;
//         else {
//             target = container_of(info->root.rb_node, struct scheduler, s_sibling);
//             sched->s_count = target->s_count + CONFIG_SCHED_COUNT_MAGIC_VALUE;
//             //推红黑树右侧
//         }
//         //linux的cfs有一个很精妙的调度算法
//         //能够将任务推到红黑树较右边的地方
//         //但是以后再借鉴了
//     }

//     info->rb_min = next;
//     rb_erase(&sched->s_sibling, &info->root);

//     struct rb_node *node = info->root.rb_node;
//     while(node) {
//         target = container_of(node, struct scheduler, s_sibling);
//         if(target->s_count > sched->s_count)
//             node = node->rb_left;
//         else
//             node = node->rb_right;
//     }
//     if(target->s_count > sched->s_count) 
//         rb_link_node(&sched->s_sibling, &target->s_sibling, &target->s_sibling.rb_left);
//     else
//         rb_link_node(&sched->s_sibling, &target->s_sibling, &target->s_sibling.rb_right);
    
//     rb_insert_color(&sched->s_sibling, &info->root);
//     if(!sched->s_class.sched_has_task(target))
//         goto pick_scheduler_start;
//     preempt_enable();
//     return sched;
// }

// void register_scheduler(struct scheduler* sched, struct sched_class sc, uint16_t level) {
//     struct se_info* info = THIS_CPU_PTR(se_info);
//     struct scheduler* tsched;
//     sched->s_level = level;

//     rb_init_node(&sched->s_sibling);
//     sched->s_class = sc;

//     if(info->rb_min != NULL) {
//         tsched = container_of(info->rb_min, struct scheduler, s_sibling);
//         rb_link_node(&sched->s_sibling, info->rb_min, &info->rb_min->rb_left);
//         sched->s_count = tsched->s_count;
//     } else {
//         rb_link_node(&sched->s_sibling, NULL, &info->root.rb_node);
//     }
//     rb_insert_color(&sched->s_sibling, &info->root);
//     info->rb_min = &sched->s_sibling;

//     sched->s_class.sched_init(sched); // 触发 sched_init 事件初始化调度器
// }

// void init_scheduler() {
//     struct se_info* info = THIS_CPU_PTR(se_info);
//     se_info.root.rb_node = NULL;
//     info->rb_min = NULL;
// }
