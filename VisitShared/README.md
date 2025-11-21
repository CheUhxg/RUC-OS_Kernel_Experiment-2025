# VisitShared

* 修改当前内核模块，为共享变量加锁，消除竞态条件。

## 实验步骤

* 拷贝编译好的内核模块。

```bash
cp -r /tmp/share/practice_kern/VisitShared/ .
cd VisitShared/
```

* 加载内核模块。

```bash
make load
```

* 编译执行测试程序。

```bash
make test
sudo ./test
```

## 实验效果

* 修改内核模块之前呈现效果：

```bash
user@kernel:~/VisitShared$ sudo ./test
Threads=4 (inc1=2 inc2=2) Loops=100000
Expected=600000 Observed=110236 Lost=489764 Ratio=81.63%
```

* 消除竞态条件后：

```bash
user@kernel:~/VisitShared$ sudo ./test
Threads=4 (inc1=2 inc2=2) Loops=100000
Expected=600000 Observed=600000 Lost=0 Ratio=0.00%
```