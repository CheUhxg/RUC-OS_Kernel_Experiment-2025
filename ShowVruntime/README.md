# ShowVruntime

* 展示10s指定进程的vruntime。

## 使用方法

* 编译内核模块和测试程序。

```bash
# Host
make

# Guest
make workload
```

* 在客户机中运行workload，详情参照[workload.c](./workload.c)。
    * -c：CPU密集型
    * -i：IO密集型
    * -y：调度密集型

```bash
# 派生一个CPU密集型的子进程
./workload -c 1
# 加载内核模块查看vruntime
make load
```

> 其他不同密集型同理。

## 展示效果

```bash
root@kernel:~/ShowVruntime# make load
[ 174.353185][ C0] [vruntime] pid=7942 comm=cpu_task vruntime=8194820852 
[ 175.393172][ C0] [vruntime] pid=7942 comm=cpu_task vruntime=82987527773 
[ 176.433160][ C0] [vruntime] pid=7942 comm=cpu_task vruntime=84026320509 
[ 177.473147][ C0] [vruntime] pid=7942 comm=cpu_task vruntime=85065363823 
[ 178.513136][ C0] [vruntime] pid=7942 comm=cpu_task vruntime=86104148786 
[ 179.553122][ C0] [vruntime] pid=7942 comm=cpu_task vruntime=87143090910 
[ 180.593110][ C0] [vruntime] pid=7942 comm=cpu_task vruntime=88181934446 
[ 181.633099][ C0] [vruntime] pid=7942 comm=cpu_task vruntime=89221261889 
[ 182.673087][ C0] [vruntime] pid=7942 comm=cpu_task vruntime=90259911937
```