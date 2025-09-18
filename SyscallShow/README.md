# SyscallShow

* 添加一个内核模块，加载模块时打印内核中所有系统调用。

## 实验运行

* 启动qemu运行编译好的内核。

```bash
./run v6.6 x86_64 focal
```

* 用户为user，无密码。

```bash
kernel login: user
```

> 接下来的操作都在客户机中。

* 拷贝实验目录到客户机本地。

```bash
cp -r /tmp/share/practice_kern/SyscallShow .
cd SyscallShow
```

* 加载内核模块syscall_show。

```bash
make load
```

## 实验要求

1. 打印系统调用的地址。
2. 打印系统调用的符号名称。

``` bash
user@kernel:~/SyscallShow$ make load
sudo insmod syscall_show.ko
[  187.017119][ T7901] syscall_show: loading out-of-tree module taints kernel.
[  187.017896][ T7901] syscall_show: module verification failed: signature and/or required key missing - tainl
[  187.019078][ T7901] print_syscalls: module init
[  187.019575][ T7901] print_syscalls: sys_call_table at ffffffff83e00240
[  187.019962][ T7901] syscall[  0] = ffffffff814b3590 -> __x64_sys_read+0x0/0x20
[  187.020311][ T7901] syscall[  1] = ffffffff814b36b0 -> __x64_sys_write+0x0/0x20
[  187.020669][ T7901] syscall[  2] = ffffffff814b03f0 -> __x64_sys_open+0x0/0x90
[  187.021024][ T7901] syscall[  3] = ffffffff814b0b30 -> __x64_sys_close+0x0/0x80
[  187.021382][ T7901] syscall[  4] = ffffffff814baf30 -> __x64_sys_newstat+0x0/0x20
[  187.021745][ T7901] syscall[  5] = ffffffff814bafd0 -> __x64_sys_newfstat+0x0/0x10
[  187.022108][ T7901] syscall[  6] = ffffffff814baf60 -> __x64_sys_newlstat+0x0/0x20
[  187.022471][ T7901] syscall[  7] = ffffffff814cfd90 -> __x64_sys_poll+0x0/0x130
[  187.022826][ T7901] syscall[  8] = ffffffff814b2370 -> __x64_sys_lseek+0x0/0x20
[  187.023175][ T7901] syscall[  9] = ffffffff810d1170 -> __x64_sys_mmap+0x0/0x40
...
```