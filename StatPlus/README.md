# StatPlus

* 添加一个内核模块，打印指定进程的 fd（默认 3）对应文件的详细信息（路径、类型、权限、owner、size、inode、时间戳等）。

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
cd practice_kern/StatPlus
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

生成 `stat_plus.ko` 文件。

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
cp -r /tmp/share/practice_kern/StatPlus .
cd StatPlus
```

* 确保目标进程存在且打开了 fd=3（示例：用 bash 打开 /etc/hostname 到 fd=3 并保持后台运行）。

```bash
bash -c 'exec 3</etc/hostname; echo $$; sleep 300' &
# 记录上一行打印的 PID，例如 12345
```

* 加载内核模块（默认 fd=3）。

```bash
make load PID=12345
```

## 实验要求

* 完成[TODO](./stat_plus.c)，使其能打印如下文件信息：

```bash
user@kernel:~/StatPlus$ make load PID=8049
sudo insmod stat_plus.ko pid=8049 fd=3
[ 2246.264867][ T8054] StatPlus: PID=8049 FD=3
[ 2246.265086][ T8054]   path: /etc/hostname
[ 2246.265280][ T8054]   type: regular
[ 2246.265454][ T8054]   mode: 0100644 (perm=0644)
[ 2246.265681][ T8054]   uid: 0  gid: 0
[ 2246.265860][ T8054]   size: 7  pos: 0
[ 2246.266051][ T8054]   inode: 22522  nlink: 1
[ 2246.266538][ T8054]   flags: 0x8000  fmode: 0x494a801d
[ 2246.266799][ T8054]   superblock: sda  sb_dev: 8:0
[ 2246.267033][ T8054]   atime: 1698928826.855588001
[ 2246.267033][ T8054]   mtime: 1698930341.222199259
[ 2246.267033][ T8054]   ctime: 1698930342.662255903
```