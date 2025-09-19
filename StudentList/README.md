# StudentList

* 实现一个内核模块，管理学生链表，并支持用户进程增删查。

## 环境准备

* （可选）编译指定版本内核，实验中采用v6.6。

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
cd practice_kern/S tu den t Li s t
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

生成 `student_list.ko` 文件。

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
cp -r /tmp/share/practice_kern/StudentList .
cd StudentList
```

* 加载内核模块student_list。

```bash
make load
```

* 编译运行（make user && make run）用户态程序，该程序会通过ioctl操作内核中的用户链表。

```c
/* 添加多条学生数据 */
struct {
    int id;
    char name[16];
} dataset[] = {
    {2023103111, "Alice"},
    {2022201456, "Bob"},
    {2023103122, "Carol"},
    {2022202457, "David"},
    {2024103113, "Eve"},
    {2023201789, "Frank"},
    {2023103555, "Grace"},
    {2023201333, "Heidi"},
    {2022201999, "Ivan"},
    {2024103666, "Judy"},
};

int n = sizeof(dataset) / sizeof(dataset[0]);
for (int i = 0; i < n; i++) {
    s.id = dataset[i].id;
    strncpy(s.name, dataset[i].name, sizeof(s.name));
    s.name[sizeof(s.name)-1] = '\0';
    if (ioctl(fd, STUDENT_ADD, &s) < 0) {
        perror("ioctl ADD");
    } else {
        printf("Added: %d %s\n", s.id, s.name);
    }
}

/* 按名字查询 */
strcpy(s.name, "Alice");
if (ioctl(fd, STUDENT_QUERY_NAME, &s) == 0) {
    printf("Query by name=Alice -> id=%d\n", s.id);
} else {
    perror("ioctl QUERY_NAME");
}

strcpy(s.name, "Judy");
if (ioctl(fd, STUDENT_QUERY_NAME, &s) == 0) {
    printf("Query by name=Judy -> id=%d\n", s.id);
} else {
    perror("ioctl QUERY_NAME");
}

/* 按年级查询 */
int grade = 2023;
printf("\nQuery grade %d (see kernel log for results)\n", grade);
ioctl(fd, STUDENT_QUERY_GRADE, &grade);

grade = 2022;
printf("\nQuery grade %d (see kernel log for results)\n", grade);
ioctl(fd, STUDENT_QUERY_GRADE, &grade);

/* 按学院查询 */
int college = 103;
printf("\nQuery college %d (see kernel log for results)\n", college);
ioctl(fd, STUDENT_QUERY_COLLEGE, &college);

college = 201;
printf("\nQuery college %d (see kernel log for results)\n", college);
ioctl(fd, STUDENT_QUERY_COLLEGE, &college);
```

* （可选）卸载内核模块student_list。

```bash
make unload
```

* （可选）使用`CTRL+A+X`退出内核运行。

## 实验要求

* 请实现内核模块的TODO，使得进入客户机后编译运行（make user && make run）用户态程序，能够得到正确输出。

``` bash
user@kernel:~/StudentList$ make load
sudo insmod student_list.ko
[ 6101.015527][ T8104] student module loaded
user@kernel:~/StudentList$ make user
gcc student_ioctl.c -o student_ioctl
user@kernel:~/StudentList$ make run
sudo ./student_ioctl
[ 6103.453687][ T8113] Added student 2023103111 Alice
Added: 2023103111 Alice
[ 6103.454156][ T8113] Added student 2022201456 Bob
Added: 2022201456 Bob
[ 6103.454552][ T8113] Added student 2023103122 Carol
Added: 2023103122 Carol
[ 6103.454950][ T8113] Added student 2022202457 David
Added: 2022202457 David
[ 6103.455358][ T8113] Added student 2024103113 Eve
Added: 2024103113 Eve
[ 6103.455748][ T8113] Added student 2023201789 Frank
Added: 2023201789 Frank
[ 6103.456155][ T8113] Added student 2023103555 Grace
Added: 2023103555 Grace
[ 6103.456564][ T8113] Added student 2023201333 Heidi
Added: 2023201333 Heidi
[ 6103.456972][ T8113] Added student 2022201999 Ivan
Added: 2022201999 Ivan
[ 6103.457368][ T8113] Added student 2024103666 Judy
Added: 2024103666 Judy
[ 6103.457762][ T8113] Query name=Alice -> id=2023103111
Query by name=Alice -> id=2023103111
[ 6103.458246][ T8113] Query name=Judy -> id=2024103666
Query by name=Judy -> id=2024103666

Query grade 2023 (see kernel log for results)
[ 6103.459012][ T8113] Grade 2023: 2023201333 Heidi
[ 6103.459259][ T8113] Grade 2023: 2023103555 Grace
[ 6103.459500][ T8113] Grade 2023: 2023201789 Frank
[ 6103.459740][ T8113] Grade 2023: 2023103122 Carol
[ 6103.460290][ T8113] Grade 2023: 2023103111 Alice

Query grade 2022 (see kernel log for results)
[ 6103.460809][ T8113] Grade 2022: 2022201999 Ivan
[ 6103.461040][ T8113] Grade 2022: 2022202457 David
[ 6103.461275][ T8113] Grade 2022: 2022201456 Bob

Query college 103 (see kernel log for results)
[ 6103.461785][ T8113] College 103: 2024103666 Judy
[ 6103.462020][ T8113] College 103: 2023103555 Grace
[ 6103.462260][ T8113] College 103: 2024103113 Eve
[ 6103.462490][ T8113] College 103: 2023103122 Carol
[ 6103.462728][ T8113] College 103: 2023103111 Alice

Query college 201 (see kernel log for results)
[ 6103.463249][ T8113] College 201: 2022201999 Ivan
[ 6103.463487][ T8113] College 201: 2023201333 Heidi
[ 6103.463726][ T8113] College 201: 2023201789 Frank
[ 6103.463965][ T8113] College 201: 2022201456 Bob
```