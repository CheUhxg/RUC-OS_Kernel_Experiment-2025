# ProcMirror

* 添加一个内核模块，在 `/proc` 下创建 `proc_mirror` 目录，用符号链接映射目标进程已打开的文件（`fd-<N>` -> 文件路径）。

## 环境准备

* （可选）编译指定版本内核，实验中采用 v6.6。

```bash
cd ~/Kernel
./compile v6.6 x86_64
```

* 拷贝仓库源码到宿主机的共享文件夹。

```bash
cd ~/Workdir/share
git clone https://github.com/CheUhxg/RUC-OS_Kernel_Experiment-2025
mv RUC-OS_Kernel_Experiment-2025 practice_kern
```

* 进入项目目录。

```bash
cd practice_kern/ProcMirror
```

* 确认 [Makefile](Makefile) 中路径是否正确。

```Makefile
# 目标内核的根目录（确认是否存在对应目录）
LINUX_KERNEL_PATH := /home/user/Kernel/v6.6/x86_64/
```

* 编译内核模块。

```bash
make
```

生成 `proc_mirror.ko` 文件。

## 实验运行

* 启动 qemu 运行编译好的内核。

```bash
./run v6.6 x86_64 focal
```

* 用户为 user，无密码。

```bash
kernel login: user
```

> 接下来的操作都在客户机中。

* 拷贝实验目录到客户机本地。

```bash
cp -r /tmp/share/practice_kern/ProcMirror .
cd ProcMirror
```

* 准备一个目标进程，让其打开 fd=3 并保持存活：

```bash
bash -c 'exec 3</etc/hostname; echo $$; sleep 300' &
PID=$!
```

* 加载内核模块，仅需指定 pid：

```bash
make load PID=$PID
```

* 查看结果：

```bash
ls -l /proc/proc_mirror
ls -l /proc/proc_mirror/fd-3 || true
```

* （可选）卸载内核模块：

```bash
make unload
```

* （可选）使用 `CTRL+A+X` 退出内核运行。

## 实验要求

* 完成[TODO](./proc_mirror.c)，实现如下效果：

```bash
user@kernel:~/ProcMirror$ ls -l /proc/proc_mirror
total 0
lrwxrwxrwx 1 root root 13 Nov  5 03:46 fd-3 -> /etc/hostname
```