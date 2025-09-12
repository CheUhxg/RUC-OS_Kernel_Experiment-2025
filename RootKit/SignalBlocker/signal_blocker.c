// signal_blocker_task.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/string.h>
#include <linux/signal.h>

#define TARGET_PROC_NAME "malware"   // 要忽略信号的进程
#define BLOCKED_SIG SIGINT            // 要忽略的信号

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("任务：让指定进程忽略特定信号");

// 保存原来的信号处理器
static struct k_sigaction old_action;

static int __init signal_blocker_init(void)
{
    struct task_struct *task;

    pr_info("signal_blocker_task: 模块加载\n");

    // 遍历所有进程
    for_each_process(task) {
        if (/* TODO: 判断 task 是否是 TARGET_PROC_NAME */) {

            pr_info("signal_blocker_task: 找到目标进程, PID=%d\n", task->pid);

            rcu_read_lock();
            task_lock(task);

            // TODO: 保存原来的 handler 到 old_action

            // TODO: 设置 task 的 BLOCKED_SIG 信号处理为忽略(SIG_IGN)
            // 注意要清空 sa_mask 并设置 sa_flags = 0

            task_unlock(task);
            rcu_read_unlock();
        }
    }

    return 0;
}

static void __exit signal_blocker_exit(void)
{
    struct task_struct *task;

    pr_info("signal_blocker_task: 模块卸载\n");

    for_each_process(task) {
        if (/* TODO: 判断 task 是否是 TARGET_PROC_NAME */) {

            rcu_read_lock();
            task_lock(task);

            // TODO: 恢复 task 的 BLOCKED_SIG 信号处理为 old_action

            task_unlock(task);
            rcu_read_unlock();
        }
    }
}

module_init(signal_blocker_init);
module_exit(signal_blocker_exit);
