# 内核函数符号导出

成功编译运行内核（见 [setup](./setup.md)）之后，部分实验需要导出特定的符号才能完成实验。本操作手册将介绍如何导出一个内核符号（函数或变量），以便在自定义模块中使用。

---

## 1. 背景介绍

Linux 内核中的函数或全局变量，默认只能在内核内部使用。  
若希望在内核模块（`*.ko`）中访问这些符号（例如调用某个内核函数），  
则必须在内核源码中通过 `EXPORT_SYMBOL()` 或 `EXPORT_SYMBOL_GPL()` 显式导出。

这称为 **符号导出（symbol export）**。

## 2. 导出符号

1. 找到目标函数的实现文件（例如 `kernel/sched/core.c`）。
2. 在函数定义之后添加导出宏：

    ```c
   void my_kernel_func(void)
   {
       pr_info("This is my kernel function.\n");
   }
   EXPORT_SYMBOL(my_kernel_func);
    ```

3. 如果希望该符号仅对 GPL 模块可见，则改用：

   ```c
   EXPORT_SYMBOL_GPL(my_kernel_func);
   ```

> `EXPORT_SYMBOL_GPL` 与 `EXPORT_SYMBOL` 的区别：
> 前者仅允许 GPL 兼容模块使用，后者则可被任何模块引用。

之后重新**编译内核**即可。

## 3. 模块中引用导出符号

当符号成功导出后，模块即可直接使用，无需额外声明：

```c
#include <linux/module.h>
#include <linux/init.h>

extern void my_kernel_func(void);

static int __init test_init(void)
{
    pr_info("Calling exported kernel function...\n");
    my_kernel_func();
    return 0;
}

static void __exit test_exit(void)
{
    pr_info("Test module unloaded.\n");
}

module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL");
```

编译模块后加载：

```bash
sudo insmod test.ko
```

## 4. 常见问题排查

| 错误提示                               | 原因                                     | 解决方法                                        |
| ---------------------------------- | -------------------------------------- | ------------------------------------------- |
| `undefined symbol: my_kernel_func` | 符号未导出或未重新编译内核                          | 检查是否添加 `EXPORT_SYMBOL()` 并重新编译安装内核          |
| `GPL-only symbol ...`              | 使用了 `EXPORT_SYMBOL_GPL()` 符号但模块未声明 GPL | 在模块中添加 `MODULE_LICENSE("GPL");`             |
| 没有找到 `/proc/kallsyms`              | 未启用 `CONFIG_KALLSYMS`                  | 在内核 `.config` 中启用 `CONFIG_KALLSYMS=y` 并重新编译 |