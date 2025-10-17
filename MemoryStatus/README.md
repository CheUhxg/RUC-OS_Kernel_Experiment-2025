# MemoryStatus

* 添加一个内核模块，遍历page打印物理内存信息，指定进程后遍历并打印vma和页表信息。

## 操作指南

* 拷贝编译好的内核模块原目录到客户机中：

```bash
cp -r /tmp/share/practice_kern/MemoryStatus/ .
cd MemoryStatus
```

* 编译并运行target用户态程序（target pid会输出）：

```bash
make target && ./target &
```

* 指定pid后运行内核模块：

```bash
make load PID=<target pid>
```

## 实验要求

* 根据[export](../export.md)导出符号[__pte_offset_map](https://elixir.bootlin.com/linux/v6.6/source/mm/pgtable-generic.c#L280)。
* 根据[mem_stat.c](./mem_stat.c)中的TODO完善代码，并按照如上流程执行后可以得到：

```bash
user@kernel:~/MemoryStatus$ make load PID=7928
sudo insmod mem_stat.ko pid=7928
[  199.109584][ T7937] mem_stat: loading out-of-tree module taints kernel.
[  199.109898][ T7937] mem_stat: module verification failed: signature and/or required key missing - taintingl
[  199.110459][ T7937] [memory_status] ===== Page Frame Traversal =====
[  199.136519][ T7937] [memory_status] ===== Page Type Summary =====
[  199.136858][ T7937] [memory_status] total_pfn         = 1310719
[  199.137140][ T7937] [memory_status] valid_pfn         = 1048575
[  199.137420][ T7937] [memory_status] free_pages        = 888
[  199.137681][ T7937] [memory_status] anon_pages        = 12596
[  199.137952][ T7937] [memory_status] file_pages        = 8873
[  199.138220][ T7937] [memory_status] slab_pages        = 14483
[  199.138491][ T7937] [memory_status] dirty_pages       = 937
[  199.138753][ T7937] [memory_status] writeback_pages   = 0
[  199.139007][ T7937] [memory_status] lru_pages         = 93311
[  199.139279][ T7937] [memory_status] PAGE_SIZE         = 4096 bytes
[  199.139574][ T7937] [memory_status] ===== Target process: bash (pid=7928) =====
[  199.139919][ T7937] [memory_status] ===== Traverse VMA (Maple Tree) =====
[  199.140241][ T7937] VMA: 0x555dcf831000 - 0x555dcf85e000, flags=0x8000071, anon=0, file=bash
[  199.140621][ T7937] VMA: 0x555dcf85e000 - 0x555dcf90f000, flags=0x8000075, anon=0, file=bash
[  199.141012][ T7937] VMA: 0x555dcf90f000 - 0x555dcf946000, flags=0x8000071, anon=0, file=bash
[  199.141406][ T7937] VMA: 0x555dcf946000 - 0x555dcf94a000, flags=0x8100071, anon=0, file=bash
[  199.141797][ T7937] VMA: 0x555dcf94a000 - 0x555dcf953000, flags=0x8100073, anon=0, file=bash
[  199.142195][ T7937] VMA: 0x555dcf953000 - 0x555dcf95d000, flags=0x8100073, anon=1, file=(null)
[  199.162442][ T7937] VMA: 0x555dd09f9000 - 0x555dd0a82000, flags=0x8100073, anon=1, file=(null)
[  199.162927][ T7937] VMA: 0x7f8e1a803000 - 0x7f8e1a806000, flags=0x8000071, anon=0, file=libnss_files-2.31.o
[  199.163421][ T7937] VMA: 0x7f8e1a806000 - 0x7f8e1a80d000, flags=0x8000075, anon=0, file=libnss_files-2.31.o
[  199.163913][ T7937] VMA: 0x7f8e1a80d000 - 0x7f8e1a80f000, flags=0x8000071, anon=0, file=libnss_files-2.31.o
[  199.164407][ T7937] VMA: 0x7f8e1a80f000 - 0x7f8e1a810000, flags=0x8100071, anon=0, file=libnss_files-2.31.o
[  199.164900][ T7937] VMA: 0x7f8e1a810000 - 0x7f8e1a811000, flags=0x8100073, anon=0, file=libnss_files-2.31.o
[  199.165397][ T7937] VMA: 0x7f8e1a811000 - 0x7f8e1a817000, flags=0x8100073, anon=1, file=(null)
[  199.165832][ T7937] VMA: 0x7f8e1a81d000 - 0x7f8e1a84f000, flags=0x8000071, anon=0, file=LC_CTYPE
[  199.166273][ T7937] VMA: 0x7f8e1a84f000 - 0x7f8e1a850000, flags=0x8000071, anon=0, file=LC_NUMERIC
[  199.166722][ T7937] VMA: 0x7f8e1a850000 - 0x7f8e1a851000, flags=0x8000071, anon=0, file=LC_TIME
[  199.167161][ T7937] VMA: 0x7f8e1a851000 - 0x7f8e1a9c4000, flags=0x8000071, anon=0, file=LC_COLLATE
[  199.167610][ T7937] VMA: 0x7f8e1a9c4000 - 0x7f8e1a9c5000, flags=0x8000071, anon=0, file=LC_MONETARY
[  199.168065][ T7937] VMA: 0x7f8e1a9c5000 - 0x7f8e1a9cc000, flags=0x80000d1, anon=0, file=gconv-modules.cache
[  199.168551][ T7937] VMA: 0x7f8e1a9cc000 - 0x7f8e1a9cf000, flags=0x8100073, anon=1, file=(null)
[  199.168983][ T7937] VMA: 0x7f8e1a9cf000 - 0x7f8e1a9f1000, flags=0x8000071, anon=0, file=libc-2.31.so
[  199.169442][ T7937] VMA: 0x7f8e1a9f1000 - 0x7f8e1ab69000, flags=0x8000075, anon=0, file=libc-2.31.so
[  199.169898][ T7937] VMA: 0x7f8e1ab69000 - 0x7f8e1abb7000, flags=0x8000071, anon=0, file=libc-2.31.so
[  199.170356][ T7937] VMA: 0x7f8e1abb7000 - 0x7f8e1abbb000, flags=0x8100071, anon=0, file=libc-2.31.so
[  199.170812][ T7937] VMA: 0x7f8e1abbb000 - 0x7f8e1abbd000, flags=0x8100073, anon=0, file=libc-2.31.so
[  199.171273][ T7937] VMA: 0x7f8e1abbd000 - 0x7f8e1abc1000, flags=0x8100073, anon=1, file=(null)
[  199.171706][ T7937] VMA: 0x7f8e1abc1000 - 0x7f8e1abc2000, flags=0x8000071, anon=0, file=libdl-2.31.so
[  199.172169][ T7937] VMA: 0x7f8e1abc2000 - 0x7f8e1abc4000, flags=0x8000075, anon=0, file=libdl-2.31.so
[  199.192446][ T7937] VMA: 0x7f8e1abc4000 - 0x7f8e1abc5000, flags=0x8000071, anon=0, file=libdl-2.31.so
[  199.192951][ T7937] VMA: 0x7f8e1abc5000 - 0x7f8e1abc6000, flags=0x8100071, anon=0, file=libdl-2.31.so
[  199.193413][ T7937] VMA: 0x7f8e1abc6000 - 0x7f8e1abc7000, flags=0x8100073, anon=0, file=libdl-2.31.so
[  199.193880][ T7937] VMA: 0x7f8e1abc7000 - 0x7f8e1abd5000, flags=0x8000071, anon=0, file=libtinfo.so.6.2
[  199.194352][ T7937] VMA: 0x7f8e1abd5000 - 0x7f8e1abe4000, flags=0x8000075, anon=0, file=libtinfo.so.6.2
[  199.194821][ T7937] VMA: 0x7f8e1abe4000 - 0x7f8e1abf2000, flags=0x8000071, anon=0, file=libtinfo.so.6.2
[  199.195291][ T7937] VMA: 0x7f8e1abf2000 - 0x7f8e1abf6000, flags=0x8100071, anon=0, file=libtinfo.so.6.2
[  199.195760][ T7937] VMA: 0x7f8e1abf6000 - 0x7f8e1abf7000, flags=0x8100073, anon=0, file=libtinfo.so.6.2
[  199.196230][ T7937] VMA: 0x7f8e1abf7000 - 0x7f8e1abf9000, flags=0x8100073, anon=1, file=(null)
[  199.196662][ T7937] VMA: 0x7f8e1abf9000 - 0x7f8e1abfa000, flags=0x8000071, anon=0, file=SYS_LC_MESSAGES
[  199.197135][ T7937] VMA: 0x7f8e1abfa000 - 0x7f8e1abfb000, flags=0x8000071, anon=0, file=LC_PAPER
[  199.197576][ T7937] VMA: 0x7f8e1abfb000 - 0x7f8e1abfc000, flags=0x8000071, anon=0, file=LC_NAME
[  199.198012][ T7937] VMA: 0x7f8e1abfc000 - 0x7f8e1abfd000, flags=0x8000071, anon=0, file=LC_ADDRESS
[  199.198463][ T7937] VMA: 0x7f8e1abfd000 - 0x7f8e1abfe000, flags=0x8000071, anon=0, file=LC_TELEPHONE
[  199.198920][ T7937] VMA: 0x7f8e1abfe000 - 0x7f8e1abff000, flags=0x8000071, anon=0, file=LC_MEASUREMENT
[  199.199387][ T7937] VMA: 0x7f8e1abff000 - 0x7f8e1ac00000, flags=0x8000071, anon=0, file=ld-2.31.so
[  199.199836][ T7937] VMA: 0x7f8e1ac00000 - 0x7f8e1ac23000, flags=0x8000075, anon=0, file=ld-2.31.so
[  199.200286][ T7937] VMA: 0x7f8e1ac23000 - 0x7f8e1ac2b000, flags=0x8000071, anon=0, file=ld-2.31.so
[  199.200734][ T7937] VMA: 0x7f8e1ac2b000 - 0x7f8e1ac2c000, flags=0x8000071, anon=0, file=LC_IDENTIFICATION
[  199.201214][ T7937] VMA: 0x7f8e1ac2c000 - 0x7f8e1ac2d000, flags=0x8100071, anon=0, file=ld-2.31.so
[  199.201663][ T7937] VMA: 0x7f8e1ac2d000 - 0x7f8e1ac2e000, flags=0x8100073, anon=0, file=ld-2.31.so
[  199.202112][ T7937] VMA: 0x7f8e1ac2e000 - 0x7f8e1ac2f000, flags=0x8100073, anon=1, file=(null)
[  199.222437][ T7937] VMA: 0x7fff6c5a0000 - 0x7fff6c5c1000, flags=0x100173, anon=1, file=(null)
[  199.222913][ T7937] VMA: 0x7fff6c5fa000 - 0x7fff6c5fe000, flags=0xc044411, anon=0, file=(null)
[  199.223344][ T7937] VMA: 0x7fff6c5fe000 - 0x7fff6c600000, flags=0x8040075, anon=0, file=(null)
[  199.223771][ T7937] [memory_status] ===== Page Table Walk (partial) =====
...
[  225.438494][ T7937] VA 0x555dcf85d000 -> PFN 0x0 (PA 0x0)
[  225.438848][ T7937] VA 0x555dcf85e000 -> PFN 0x2296d (PA 0x2296d000)
[  225.439261][ T7937] VA 0x555dcf85f000 -> PFN 0x2296e (PA 0x2296e000)
[  225.439674][ T7937] VA 0x555dcf860000 -> PFN 0x2296f (PA 0x2296f000)
[  225.440089][ T7937] VA 0x555dcf861000 -> PFN 0x22970 (PA 0x22970000)
[  225.440502][ T7937] VA 0x555dcf862000 -> PFN 0x22971 (PA 0x22971000)
[  225.440916][ T7937] VA 0x555dcf863000 -> PFN 0x22972 (PA 0x22972000)
[  225.461142][ T7937] VA 0x555dcf864000 -> PFN 0x22973 (PA 0x22973000)
[  225.461584][ T7937] VA 0x555dcf865000 -> PFN 0x22974 (PA 0x22974000)
[  225.462000][ T7937] VA 0x555dcf866000 -> PFN 0x22975 (PA 0x22975000)
[  238.653332][ T7937] [memory_status] Mapped pages: 471
```