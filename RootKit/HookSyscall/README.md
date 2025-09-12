# HookSyscall

* 添加一个内核模块，hook系统调用setuid，使得恶意进程通过该系统调用提权。

## 环境准备

* （可选）编译指定版本内核，实验中采用v6.6。

```bash
cd ~/Kernel
./compile v6.6 x86_64
```

> 注意：该实验请导出符号kallsyms_lookup_name。

* 拷贝仓库源码到宿主机的共享文件夹。

```bash
cd ~/Workdir/share
git clone https://github.com/CheUhxg/RUC-OS_Kernel_Experiment-2025
mv RUC-OS_Kernel_Experiment-2025 practice_kern
```

* 进入项目目录。

```bash
cd practice_kern/RootKit
```

* 确认[Makefile](Makefile)中路径是否正确。

```Makefile
# 目标内核的根目录（确认是否存在对应目录）
LINUX_KERNEL_PATH := /home/user/Kernel/v6.6/x86_64/
```

* 编译内核模块。

```bash
make
```

生成 `hook_syscall.ko` 文件。

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
cp -r /tmp/share/practice_kern/HookSyscall .
cd HookSyscall
```

* 编译运行malware。

```bash
make malware && make run
```

## 实验要求

* malware会循环检查自己的权限并调用setuid，加载HookSyscall模块后，malware调用新的setuid，使得权限为root权限。

``` bash
sudo insmod hook_syscall.ko
[   41.850746][ T7921] hook_syscall: loading out-of-tree module taints kernel.
[   41.851199][ T7921] hook_syscall: module verification failed: signature and/or required key missing l
[   41.851903][ T7921] HookSyscall Module Loaded
[   41.852187][ T7921] Found target process: malware (PID: 7914)
[   41.852541][ T7921] sys_call_table found at ffffffff83e00240
[   41.852861][ T7921] Original syscall handler at ffffffff81216730
[   41.853665][ T7921] Installed hook for syscall 105
[   43.569028][ T7914] sys_mycall called
[   43.569910][ T7914] Elevating privileges for process 'malware' (PID: 7914)
<malware>: ruid=1000 euid=1000
[   48.572940][ T7914] sys_mycall called
[   48.573834][ T7914] Elevating privileges for process 'malware' (PID: 7914)
<malware>: ruid=0 euid=0
```