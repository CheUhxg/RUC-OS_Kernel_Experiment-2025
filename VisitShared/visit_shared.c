// VisitShared: vulnerable race condition demo (no synchronization)
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/ioctl.h>

#define VS_DEV_NAME "visit_shared"
#define VS_CLASS_NAME "visit_shared_cls"

#define VS_MAGIC 0xA4
#define VS_INC1 _IO(VS_MAGIC, 1)
#define VS_INC2 _IO(VS_MAGIC, 2)
#define VS_GET  _IOR(VS_MAGIC, 3, int)

static dev_t vs_devno;
static struct cdev vs_cdev;
static struct class *vs_class;
static int shared_val = 0; // intentionally non-atomic

static long vs_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int tmp;
    switch (cmd) {
    case VS_INC1:
        tmp = shared_val;
        schedule();
        shared_val = tmp + 1;
        return 0;
    case VS_INC2:
        tmp = shared_val;
        schedule();
        shared_val = tmp + 2;
        return 0;
    case VS_GET:
        if (copy_to_user((int __user *)arg, &shared_val, sizeof(shared_val)))
            return -EFAULT;
        return 0;
    default:
        return -ENOTTY;
    }
}

static int vs_open(struct inode *inode, struct file *file) { return 0; }
static int vs_release(struct inode *inode, struct file *file) { return 0; }

static const struct file_operations vs_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = vs_ioctl,
    .open = vs_open,
    .release = vs_release,
};

static int __init vs_init(void)
{
    int ret = alloc_chrdev_region(&vs_devno, 0, 1, VS_DEV_NAME);
    if (ret) return ret;
    cdev_init(&vs_cdev, &vs_fops);
    vs_cdev.owner = THIS_MODULE;
    ret = cdev_add(&vs_cdev, vs_devno, 1);
    if (ret) goto err_unregister;
    vs_class = class_create(VS_CLASS_NAME);
    if (IS_ERR(vs_class)) {
        ret = PTR_ERR(vs_class);
        goto err_cdev;
    }
    device_create(vs_class, NULL, vs_devno, NULL, VS_DEV_NAME);
    pr_info("visit_shared: loaded without locking\n");
    shared_val = 0;
    return 0;
err_cdev:
    cdev_del(&vs_cdev);
err_unregister:
    unregister_chrdev_region(vs_devno, 1);
    return ret;
}

static void __exit vs_exit(void)
{
    device_destroy(vs_class, vs_devno);
    class_destroy(vs_class);
    cdev_del(&vs_cdev);
    unregister_chrdev_region(vs_devno, 1);
    pr_info("visit_shared: unloaded\n");
}

module_init(vs_init);
module_exit(vs_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("VisitShared race condition demo (inc1/inc2/get) without synchronization");