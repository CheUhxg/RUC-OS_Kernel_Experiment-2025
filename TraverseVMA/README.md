# TraverseVMA

* 添加一个内核模块，翻转指定进程VMA的写位。

## 实验过程

* 编译好内核模块后进入客户机，拷贝当前目录到客户机中。

```bash
cp -r /tmp/share/practice_kern/TraverseVMA/ .
cd TraverseVMA
```

* 编译target用户态程序并运行。

```bash
make target && ./target &
```

* 查看当前用户态程序的maps。

```bash
cat /proc/<target-pid>/maps
```

* 加载内核模块翻转写位。

```bash
make load PID=<target-pid>
```

* 再次查看maps，比较前后差异。

## 实验要求

* 完善TODO的翻转VMA写位过程，使得查看maps时对应的写位被翻转：

```bash
user@kernel:~/TraverseVMA$ ./target &
[1] 7898
user@kernel:~/TraverseVMA$ cat /proc/7898/maps
559de2044000-559de2045000 r--p 00000000 08:00 28521                      /home/user/TraverseVMA/target
559de2045000-559de2046000 r-xp 00001000 08:00 28521                      /home/user/TraverseVMA/target
559de2046000-559de2047000 r--p 00002000 08:00 28521                      /home/user/TraverseVMA/target
559de2047000-559de2048000 r--p 00002000 08:00 28521                      /home/user/TraverseVMA/target
559de2048000-559de2049000 rw-p 00003000 08:00 28521                      /home/user/TraverseVMA/target
7f56a20e8000-7f56a210a000 r--p 00000000 08:00 4415                       /usr/lib/x86_64-linux-gnu/libc-2.31.so
7f56a210a000-7f56a2282000 r-xp 00022000 08:00 4415                       /usr/lib/x86_64-linux-gnu/libc-2.31.so
7f56a2282000-7f56a22d0000 r--p 0019a000 08:00 4415                       /usr/lib/x86_64-linux-gnu/libc-2.31.so
7f56a22d0000-7f56a22d4000 r--p 001e7000 08:00 4415                       /usr/lib/x86_64-linux-gnu/libc-2.31.so
7f56a22d4000-7f56a22d6000 rw-p 001eb000 08:00 4415                       /usr/lib/x86_64-linux-gnu/libc-2.31.so
7f56a22d6000-7f56a22dc000 rw-p 00000000 00:00 0 
7f56a22e2000-7f56a22e3000 r--p 00000000 08:00 4411                       /usr/lib/x86_64-linux-gnu/ld-2.31.so
7f56a22e3000-7f56a2306000 r-xp 00001000 08:00 4411                       /usr/lib/x86_64-linux-gnu/ld-2.31.so
7f56a2306000-7f56a230e000 r--p 00024000 08:00 4411                       /usr/lib/x86_64-linux-gnu/ld-2.31.so
7f56a230f000-7f56a2310000 r--p 0002c000 08:00 4411                       /usr/lib/x86_64-linux-gnu/ld-2.31.so
7f56a2310000-7f56a2311000 rw-p 0002d000 08:00 4411                       /usr/lib/x86_64-linux-gnu/ld-2.31.so
7f56a2311000-7f56a2312000 rw-p 00000000 00:00 0 
7ffe9e32b000-7ffe9e34c000 rw-p 00000000 00:00 0                          [stack]
7ffe9e3b3000-7ffe9e3b7000 r--p 00000000 00:00 0                          [vvar]
7ffe9e3b7000-7ffe9e3b9000 r-xp 00000000 00:00 0                          [vdso]
ffffffffff600000-ffffffffff601000 --xp 00000000 00:00 0                  [vsyscall]
user@kernel:~/TraverseVMA$ make load PID=7898
sudo insmod traverse_vma.ko pid=7898
[   47.652363][ T7907] traverse_vma: loading out-of-tree module taints kernel.
[   47.652711][ T7907] traverse_vma: module verification failed: signature and/or required key missing - tainting kernel
[   47.653315][ T7907] Process 7898 (target):
[   47.653521][ T7907]     flags before=0x8000071, after=0x8000073, diff=0x2
[   47.653847][ T7907]     flags before=0x8000075, after=0x8000077, diff=0x2
[   47.654171][ T7907]     flags before=0x8000071, after=0x8000073, diff=0x2
[   47.654498][ T7907]     flags before=0x8100071, after=0x8100073, diff=0x2
[   47.654821][ T7907]     flags before=0x8100073, after=0x8100071, diff=0x2
[   47.655151][ T7907]     flags before=0x8000071, after=0x8000073, diff=0x2
[   47.655478][ T7907]     flags before=0x8000075, after=0x8000077, diff=0x2
[   47.655809][ T7907]     flags before=0x8000071, after=0x8000073, diff=0x2
[   47.656134][ T7907]     flags before=0x8100071, after=0x8100073, diff=0x2
[   47.656465][ T7907]     flags before=0x8100073, after=0x8100071, diff=0x2
[   47.656794][ T7907]     flags before=0x8100073, after=0x8100071, diff=0x2
[   47.657117][ T7907]     flags before=0x8000071, after=0x8000073, diff=0x2
[   47.657444][ T7907]     flags before=0x8000075, after=0x8000077, diff=0x2
[   47.657774][ T7907]     flags before=0x8000071, after=0x8000073, diff=0x2
[   47.658094][ T7907]     flags before=0x8100071, after=0x8100073, diff=0x2
[   47.658419][ T7907]     flags before=0x8100073, after=0x8100071, diff=0x2
[   47.658749][ T7907]     flags before=0x8100073, after=0x8100071, diff=0x2
[   47.659073][ T7907]     flags before=0x100173, after=0x100171, diff=0x2
user@kernel:~/TraverseVMA$ cat /proc/7898/maps
559de2044000-559de2045000 rw-p 00000000 08:00 28521                      /home/user/TraverseVMA/target
559de2045000-559de2046000 rwxp 00001000 08:00 28521                      /home/user/TraverseVMA/target
559de2046000-559de2047000 rw-p 00002000 08:00 28521                      /home/user/TraverseVMA/target
559de2047000-559de2048000 rw-p 00002000 08:00 28521                      /home/user/TraverseVMA/target
559de2048000-559de2049000 r--p 00003000 08:00 28521                      /home/user/TraverseVMA/target
7f56a20e8000-7f56a210a000 rw-p 00000000 08:00 4415                       /usr/lib/x86_64-linux-gnu/libc-2.31.so
7f56a210a000-7f56a2282000 rwxp 00022000 08:00 4415                       /usr/lib/x86_64-linux-gnu/libc-2.31.so
7f56a2282000-7f56a22d0000 rw-p 0019a000 08:00 4415                       /usr/lib/x86_64-linux-gnu/libc-2.31.so
7f56a22d0000-7f56a22d4000 rw-p 001e7000 08:00 4415                       /usr/lib/x86_64-linux-gnu/libc-2.31.so
7f56a22d4000-7f56a22d6000 r--p 001eb000 08:00 4415                       /usr/lib/x86_64-linux-gnu/libc-2.31.so
7f56a22d6000-7f56a22dc000 r--p 00000000 00:00 0 
7f56a22e2000-7f56a22e3000 rw-p 00000000 08:00 4411                       /usr/lib/x86_64-linux-gnu/ld-2.31.so
7f56a22e3000-7f56a2306000 rwxp 00001000 08:00 4411                       /usr/lib/x86_64-linux-gnu/ld-2.31.so
7f56a2306000-7f56a230e000 rw-p 00024000 08:00 4411                       /usr/lib/x86_64-linux-gnu/ld-2.31.so
7f56a230f000-7f56a2310000 rw-p 0002c000 08:00 4411                       /usr/lib/x86_64-linux-gnu/ld-2.31.so
7f56a2310000-7f56a2311000 r--p 0002d000 08:00 4411                       /usr/lib/x86_64-linux-gnu/ld-2.31.so
7f56a2311000-7f56a2312000 r--p 00000000 00:00 0 
7ffe9e32b000-7ffe9e34c000 r--p 00000000 00:00 0                          [stack]
7ffe9e3b3000-7ffe9e3b7000 r--p 00000000 00:00 0                          [vvar]
7ffe9e3b7000-7ffe9e3b9000 r-xp 00000000 00:00 0                          [vdso]
ffffffffff600000-ffffffffff601000 --xp 00000000 00:00 0                  [vsyscall]
```