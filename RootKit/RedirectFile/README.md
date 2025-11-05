# RedirectFile

* 添加一个内核模块，将指定进程打开的文件重定向到另一个文件（如`/etc/passwd`）。

## 实验运行

* 先在宿主机编译好内核模块。

```bash
make
```

* 之后进入客户机，检查当前是否存在hacker用户：

```bash
id hacker
```

此时应输出`id: ‘hacker’: no such user`。

* 编译执行malware

```bash
make run
```

* 加载内核模块篡改malware的fd3，并向篡改后的文件中添加用户信息。

```bash
root@kernel:~/RedirectFile# make load
sudo insmod redirect_file.ko
[   38.116530][ T7915] redirect_file: loading out-of-tree module taints kernel.
[   38.116958][ T7915] redirect_file: module verification failed: signature and/or required key missl
[   38.117650][ T7915] hide_file模块正在加载...
[   38.117958][ T7915] 找到目标进程 "malware" (pid: 7909)
[   38.118307][ T7915] 找到目标文件: /home/user/secret_file (fd: 3)
[   38.118711][ T7915] 已将 fd 3 替换为 /etc/passwd
[   38.119010][ T7915] 文件替换成功。
root@kernel:~/RedirectFile# ll /proc/7909/fd/3
lrwx------ 1 root root 64 Apr 21 04:44 /proc/7909/fd/3 -> /etc/passwd
```

## 实验要求

* 请完善[redirect_file.c](./redirect_file.c)的TODO，使得加载内核模块后hacker用户被添加。

```bash
user@kernel:~/RedirectFile$ id hacker
id: ‘hacker’: no such user
user@kernel:~/RedirectFile$ make run
cc -o malware malware.c
./malware &
user@kernel:~/RedirectFile$ Malware process started. PID: 7914
Use: cat /proc/7914/maps to check fd 3.
malware process started. Opening file: /home/user/secret_file
user@kernel:~/RedirectFile$ make load
sudo insmod redirect_file.ko
[  359.185576][ T7918] redirect_file: loading out-of-tree module taints kernel.
[  359.185993][ T7918] redirect_file: module verification failed: signature and/or l
[  359.186698][ T7918] hide_file模块正在加载...
[  359.186999][ T7918] 找到目标进程 "malware" (pid: 7914)
[  359.187341][ T7918] 找到目标文件: /home/user/secret_file (fd: 3)
[  359.187738][ T7918] 已将 fd 3 替换为 /etc/passwd
[  359.188038][ T7918] 文件替换成功。
[#] Start writing
user@kernel:~/RedirectFile$ id hacker
uid=1001(hacker) gid=1001(fuse) groups=1001(fuse)
```