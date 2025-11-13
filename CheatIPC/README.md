# CheatIPC

* 添加一个内核模块，篡改消息队列中的消息内容使得程序行为改变。

## 操作指南

* 拷贝编译好的内核模块原目录到客户机中：

```bash
cp -r /tmp/share/practice_kern/CheatIPC/ .
cd CheatIPC
```

* 编译并运行用户态程序sender和receiver：

```bash
make user
```

* 执行用户态程序：
    * sender：每隔2秒给receiver发送一个消息，消息内容是`good`；
    * receiver：每隔10秒获取一个消息，如果消息内容不是`good`，则说明消息被篡改。

```bash
make run
```

此时会输出一个qid，后续会使用到。

* 指定qid后运行内核模块：

```bash
make load QID=<target qid>
```

* 之后执行check来验证实验是否通过：
    * <font color=green>PASS</font>：通过。
    * <font color=red>FAIL</font>：未通过。

```bash
make check
```

## 实验要求

* 根据[export](../export.md)导出符号[ipc_obtain_object_check](https://elixir.bootlin.com/linux/v6.6/source/ipc/util.c#L650)。
* 根据[cheat_ipc.c](./cheat_ipc.c)中的TODO完善代码，并按照如上流程执行后可以得到：

```bash
user@kernel:~/CheatIPC$ make user
gcc -Wall -Wextra -std=c99 -O2 -o sender sender.c
gcc -Wall -Wextra -std=c99 -O2 -o receiver receiver.c
user@kernel:~/CheatIPC$ make run
./receiver > ./rcv &
./sender &
sender: qid=0
user@kernel:~/CheatIPC$ make load QID=0
sudo insmod cheat_ipc.ko qid=0
[   35.854352][ T7923] cheat_ipc: loading out-of-tree module taints kernel.
[   35.854680][ T7923] cheat_ipc: module verification failed: signature and/or required key missing - tl
[   35.855265][ T7923] cheat_ipc: found task! pid=7914, comm=sender
[   35.855556][ T7923] cheat_ipc: good -> bad
[   35.855758][ T7923] cheat_ipc: good -> bad
[   35.855958][ T7923] cheat_ipc: good -> bad
[   35.856157][ T7923] cheat_ipc: good -> bad
user@kernel:~/CheatIPC$ make check  # 此时receiver还未接收消息
FAIL
user@kernel:~/CheatIPC$ make check  # receiver接收到消息，实验通过
PASS
```