#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/pid.h>

static int pid = -1;
module_param(pid, int, 0644);

static struct task_struct *target;
static struct timer_list vruntime_timer;
static unsigned long end_jiffies;

static void print_vruntime(struct timer_list *t)
{
    if (time_after(jiffies, end_jiffies))
        return;
    if (target)
        printk(KERN_INFO "[vruntime] pid=%d comm=%s vruntime=%llu\n",
               target->pid, target->comm,
               (unsigned long long)target->se.vruntime);
    mod_timer(&vruntime_timer, jiffies + HZ);
}

static int __init vruntime_init(void)
{
    struct pid *pid_struct;
    if (pid < 0)
        return -EINVAL;
    pid_struct = find_get_pid(pid);
    if (!pid_struct)
        return -ESRCH;
    target = pid_task(pid_struct, PIDTYPE_PID);
    put_pid(pid_struct);
    if (!target)
        return -ESRCH;
    end_jiffies = jiffies + 10*HZ;
    timer_setup(&vruntime_timer, print_vruntime, 0);
    mod_timer(&vruntime_timer, jiffies + HZ);
    printk(KERN_INFO "tracking pid=%d comm=%s\n", target->pid, target->comm);
    return 0;
}

static void __exit vruntime_exit(void)
{
    del_timer_sync(&vruntime_timer);
    printk(KERN_INFO "vruntime module unloaded\n");
}

module_init(vruntime_init);
module_exit(vruntime_exit);
MODULE_LICENSE("GPL");
