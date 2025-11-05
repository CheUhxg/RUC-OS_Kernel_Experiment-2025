// SPDX-License-Identifier: GPL-2.0
/*
 * StatPlus (assignment skeleton)
 * Goal: On module load, print information for a given PID's file descriptor (default: fd=3).
 *
 * Keep the printing block unchanged. Implement the TODOs to make it work.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/pid.h>
#include <linux/sched/signal.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/limits.h>
#include <linux/dcache.h>
#include <linux/path.h>
#include <linux/uidgid.h>
#include <linux/kdev_t.h>
#include <linux/time64.h>

static int pid = -1;
module_param(pid, int, 0644);
MODULE_PARM_DESC(pid, "Target PID");

static int fd = 3; /* default: 3 */
module_param(fd, int, 0644);
MODULE_PARM_DESC(fd, "Target FD (default 3)");

static int __init stat_plus_init(void)
{
    struct task_struct *task; /* Process resolution is provided (not graded). */
    struct files_struct *files __maybe_unused;
    struct file *file = NULL;

    if (pid <= 0) {
        pr_err("StatPlus: please set pid (>0)\n");
        return -EINVAL;
    }
    if (fd < 0) {
        pr_err("StatPlus: fd must be >= 0\n");
        return -EINVAL;
    }

    /* Process lookup is provided (not part of grading). */
    task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task) {
        pr_err("StatPlus: pid %d not found\n", pid);
        return -ESRCH;
    }

    /* TODO: From task->files, locate the file corresponding to the given fd
     * under the correct synchronization. Hold a safe reference to the file
     * while printing its information, and release it afterwards.
     */

    /* TODO (optional): Use an RCU-friendly approach for walking the fd table
     * and briefly justify the synchronization choice.
     */

    /* Drop the task reference here to avoid leaks in the skeleton.
     * When you implement the TODOs above, adjust the placement accordingly.
     */
    put_task_struct(task);

    if (!file) {
        pr_err("StatPlus: pid %d has no open file at fd=%d\n", pid, fd);
        return -EBADF;
    }

    /* Gather and print information. NOTE: keep the block below unchanged. */
    {
        char path_buf[PATH_MAX];
        char *path = (char *)"(todo)";
        struct inode *inode = NULL;
        umode_t mode = 0;
        const char *type = "(todo)";
        kuid_t kuid;
        kgid_t kgid;
        unsigned int uid = 0;
        unsigned int gid = 0;
        loff_t size = 0;
        loff_t pos = 0;
        struct timespec64 at = (struct timespec64){0};
        struct timespec64 mt = (struct timespec64){0};
        struct timespec64 ct = (struct timespec64){0};
        dev_t sdev = 0;

    /* TODO: Fill all fields from file/inode under proper sync; get path via d_path (handle IS_ERR). */

        pr_info("StatPlus: PID=%d FD=%d\n", pid, fd);
        pr_info("  path: %s\n", path);
        pr_info("  type: %s\n", type);
        pr_info("  mode: %#o (perm=%#o)\n", mode, mode & 07777);
        pr_info("  uid: %u  gid: %u\n", uid, gid);
        pr_info("  size: %lld  pos: %lld\n", (long long)size, (long long)pos);
        pr_info("  inode: %lu  nlink: %lu\n",
                (unsigned long)inode->i_ino, (unsigned long)inode->i_nlink);
        pr_info("  flags: %#x  fmode: %#x\n", file->f_flags, file->f_mode);
        pr_info("  superblock: %s  sb_dev: %u:%u\n",
                inode->i_sb ? inode->i_sb->s_id : "?", MAJOR(sdev), MINOR(sdev));
        pr_info("  atime: %lld.%09lu\n  mtime: %lld.%09lu\n  ctime: %lld.%09lu\n",
                (long long)at.tv_sec, (unsigned long)at.tv_nsec,
                (long long)mt.tv_sec, (unsigned long)mt.tv_nsec,
                (long long)ct.tv_sec, (unsigned long)ct.tv_nsec);
    }

    /* Balance get_file() once the TODOs are implemented. */
    fput(file);
    return 0;
}

static void __exit stat_plus_exit(void)
{
    pr_info("StatPlus: unloaded\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("StatPlus");
MODULE_DESCRIPTION("Print specified PID's fd (default 3) info - stat plus");
MODULE_VERSION("1.0");

module_init(stat_plus_init);
module_exit(stat_plus_exit);
