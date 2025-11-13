#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/msg.h>
#include <linux/ipc.h>
#include <linux/nsproxy.h>
#include <linux/moduleparam.h>
#include <linux/ipc_namespace.h>

static int qid = 0;
module_param(qid, int, 0644);
MODULE_PARM_DESC(qid, "System V message queue id to inspect");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("xxx");

struct msg_queue {
	struct kern_ipc_perm q_perm;
	time64_t q_stime;		/* last msgsnd time */
	time64_t q_rtime;		/* last msgrcv time */
	time64_t q_ctime;		/* last change time */
	unsigned long q_cbytes;		/* current number of bytes on queue */
	unsigned long q_qnum;		/* number of messages in queue */
	unsigned long q_qbytes;		/* max number of bytes on queue */
	struct pid *q_lspid;		/* pid of last msgsnd */
	struct pid *q_lrpid;		/* last receive pid */

	struct list_head q_messages;
	struct list_head q_receivers;
	struct list_head q_senders;
} __randomize_layout;

#define msg_ids(ns)	((ns)->ids[1])

extern struct kern_ipc_perm *ipc_obtain_object_check(struct ipc_ids *ids, int id);

static int __init cheat_ipc_init(void)
{
    struct ipc_namespace *ns;
    struct msg_queue *msq;
    struct msg_msg *msg, *t;
    struct kern_ipc_perm *ipcp;
    struct task_struct *task;
    char *str;
    struct task_struct *sender_task = NULL;

    // 遍历系统中所有进程
    for_each_process(task) {
        if (strcmp(task->comm, "sender") == 0) {
            pr_info("cheat_ipc: found task! pid=%d, comm=%s\n",
                    task->pid, task->comm);
            sender_task = task;
            break;
        }
    }

    if (!sender_task) {
        pr_info("cheat_ipc: process 'sender' not found\n");
        return -ESRCH;
    }
    // TODO: obtain target message queue(task -> ns -> ipcp -> msq)
    
    // TODO: iterate messages and modify payload from 'good' to 'bad'
    return 0;
}

static void __exit cheat_ipc_exit(void)
{
    pr_info("cheat_ipc: exit\n");
}

module_init(cheat_ipc_init);
module_exit(cheat_ipc_exit);
