#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/ioctl.h>
#include <linux/hashtable.h>

/* ---------------- 学生结构与链表/哈希 ---------------- */

struct student {
    int id;
    char name[16];
    struct list_head list;
    struct hlist_node hnode_grade;
    struct hlist_node hnode_college;
};

LIST_HEAD(student_list);

#define GRADE_BITS   8
#define COLLEGE_BITS 8
#define GRADE_BUCKETS   (1 << GRADE_BITS)
#define COLLEGE_BUCKETS (1 << COLLEGE_BITS)

static struct hlist_head grade_table[GRADE_BUCKETS];
static struct hlist_head college_table[COLLEGE_BUCKETS];

static inline int get_grade(int id) { return id / 1000000; }
static inline int get_college(int id) { return (id / 1000) % 1000; }
static inline unsigned int grade_hashfn(int grade) { return grade & (GRADE_BUCKETS - 1); }
static inline unsigned int college_hashfn(int college) { return college & (COLLEGE_BUCKETS - 1); }

static void init_buckets(void)
{
    /* TODO: initialize all hash buckets */
}

static struct student *create_student(int id, const char *name)
{
    struct student *stu;
    /* TODO: allocate and initialize a student struct */
    return stu;
}

static void add_student(struct student *stu)
{
    /* TODO: add student to list and both hash tables */
}

static void del_student(struct student *stu)
{
    /* TODO: remove student from list and hash tables, free memory */
}

static struct student *find_by_id(int id)
{
    /* TODO: search student by id in list */
    return NULL;
}

int assemb_strcmp(const char *dst, const char *src) {
    int res;
    /* TODO: implement strcmp in assembly */
    return res;
}

static struct student *find_by_name(const char *name)
{
    /* TODO: search student by name using assemb_strcmp */
    return NULL;
}

/* ---------------- IOCTL 定义 ---------------- */

#define STUDENT_MAGIC   'S'

#define STUDENT_ADD        _IOW(STUDENT_MAGIC, 1, struct student_ioctl)
#define STUDENT_DEL        _IOW(STUDENT_MAGIC, 2, int)   // 按学号删除
#define STUDENT_QUERY_NAME _IOWR(STUDENT_MAGIC, 3, struct student_ioctl)
#define STUDENT_QUERY_GRADE  _IOWR(STUDENT_MAGIC, 4, int) // 输入年级
#define STUDENT_QUERY_COLLEGE _IOWR(STUDENT_MAGIC, 5, int) // 输入学院编码

struct student_ioctl {
    int id;
    char name[16];
};

/* ---------------- 字符设备实现 ---------------- */

static dev_t devno;
static struct cdev student_cdev;
static struct class *student_class;

static long student_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct student_ioctl udata;
    struct student *stu;

    switch (cmd) {
    case STUDENT_ADD:
        if (copy_from_user(&udata, (void __user *)arg, sizeof(udata)))
            return -EFAULT;
        stu = create_student(udata.id, udata.name);
        if (!stu) return -ENOMEM;
        add_student(stu);
        printk("Added student %d %s\n", udata.id, udata.name);
        break;

    case STUDENT_DEL:
        {
            int id;
            if (copy_from_user(&id, (void __user *)arg, sizeof(int)))
                return -EFAULT;
            stu = find_by_id(id);
            if (!stu) return -ENOENT;
            del_student(stu);
            printk("Deleted student %d\n", id);
        }
        break;

    case STUDENT_QUERY_NAME:
        if (copy_from_user(&udata, (void __user *)arg, sizeof(udata)))
            return -EFAULT;
        stu = find_by_name(udata.name);
        if (!stu) return -ENOENT;
        udata.id = stu->id;
        if (copy_to_user((void __user *)arg, &udata, sizeof(udata)))
            return -EFAULT;
        printk("Query name=%s -> id=%d\n", stu->name, stu->id);
        break;

    case STUDENT_QUERY_GRADE:
        {
            int grade, bucket;
            if (copy_from_user(&grade, (void __user *)arg, sizeof(int)))
                return -EFAULT;
            bucket = grade_hashfn(grade);
            hlist_for_each_entry(stu, &grade_table[bucket], hnode_grade) {
                if (get_grade(stu->id) == grade)
                    printk("Grade %d: %d %s\n", grade, stu->id, stu->name);
            }
        }
        break;

    case STUDENT_QUERY_COLLEGE:
        {
            int college, bucket;
            if (copy_from_user(&college, (void __user *)arg, sizeof(int)))
                return -EFAULT;
            bucket = college_hashfn(college);
            hlist_for_each_entry(stu, &college_table[bucket], hnode_college) {
                if (get_college(stu->id) == college)
                    printk("College %d: %d %s\n", college, stu->id, stu->name);
            }
        }
        break;

    default:
        return -ENOTTY;
    }
    return 0;
}

static struct file_operations student_fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = student_ioctl,
};

/* ---------------- 模块入口/出口 ---------------- */

static int __init student_init(void)
{
    int ret;
    init_buckets();

    ret = alloc_chrdev_region(&devno, 0, 1, "student");
    if (ret < 0) return ret;

    cdev_init(&student_cdev, &student_fops);
    ret = cdev_add(&student_cdev, devno, 1);
    if (ret < 0) goto err_cdev;

    student_class = class_create("student");
    if (IS_ERR(student_class)) {
        ret = PTR_ERR(student_class);
        goto err_class;
    }
    device_create(student_class, NULL, devno, NULL, "student");

    printk("student module loaded\n");
    return 0;

err_class:
    cdev_del(&student_cdev);
err_cdev:
    unregister_chrdev_region(devno, 1);
    return ret;
}

static void __exit student_exit(void)
{
    struct student *stu, *tmp;
    list_for_each_entry_safe(stu, tmp, &student_list, list)
        del_student(stu);

    device_destroy(student_class, devno);
    class_destroy(student_class);
    cdev_del(&student_cdev);
    unregister_chrdev_region(devno, 1);

    printk("student module unloaded\n");
}

module_init(student_init);
module_exit(student_exit);

MODULE_LICENSE("GPL");
