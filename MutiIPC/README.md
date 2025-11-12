# MutiIPC

* 一个最小化 Fuzz 练习框架，演示并练习多种 IPC 的协作。
* Master 负责初始化 IPC、填充样本、派发任务并收集报告；Worker 接收任务、从共享内存读取样本、执行 `target_vuln`，遇到崩溃将通过管道上报 Master。Master 将持续派发样本，直到检测到崩溃为止。

## 目录结构

```
MutiIPC/
├── Makefile
├── run_demo.sh
├── README.md           # 本文件
├── ipc_common.h        # 公共常量与数据结构（包含 SHM_KEY/SEM_KEY/MSG_KEY）
├── main.c              # Master（已实现）
├── worker.c            # Worker（已实现）
├── target_vuln.c       # 被 fuzz 的示例目标（已实现）
├── msgqueue.c          # TODO：实现消息队列接口（已清空/骨架）
├── shm_sem.c           # TODO：实现共享内存 + 信号量接口（已清空/骨架）
```

## 实验步骤

```bash
# 运行 demo（会编译并启动 main）
./run_demo.sh
```

正常观察到的流程：Master 写入样本池（包含触发崩溃的样本 `"CRASH\n"`），fork 出若干 worker，持续派发任务并监听 worker 管道；当 worker 报告 `CRASHED` 后 Master 停止并清理 IPC。

示例输出（预期）：

```
Master starting (persistent fuzz until crash)
Master wrote 4 samples
Master: starting persistent dispatch loop
Master: report from worker[0] pid=5017: PID 5019: CRASHED on sample 2 (signal 6)
Master: crash detected, shutting down workers
Master exiting (crash-driven stop)
```

## 实验要求

请实现以下两个文件中的函数，使整个框架能运行并在 `target_vuln` 遇到 `"CRASH"` 时报告崩溃并停止：

1. [msgqueue.c](./msgqueue.c)：消息队列实现，System V。需要实现并导出：
    * `int msgqueue_init(key_t key);`：创建/获取消息队列，返回消息队列 id，失败返回 -1。
    * `int msgqueue_send(struct fuzz_task *t);`：向消息队列发送任务消息（封装为 `struct fuzz_msg`），返回 0 成功，-1 失败。
    * `int msgqueue_recv(struct fuzz_task *t, long mtype);`：从消息队列接收任务（根据 `mtype`），填充 `t`，成功返回 0，失败返回 -1。
    * `int msgqueue_cleanup();`：删除消息队列资源（`IPC_RMID`），返回 0/ -1。
2. `shm_sem.c`：共享内存 + SysV 信号量。需要实现并导出：
    * `int shmsem_init(key_t shmkey, key_t semkey);`：创建/附加共享内存并创建/初始化信号量（互斥，初值 1）。返回 0 成功，-1 失败。
    * `int shm_write_sample(int idx, const char *buf, int len);`：在共享池的 `idx` 位置写入样本（受信号量保护），返回 0 成功，-1 失败。
    * `int shm_read_sample(int idx, char *outbuf, int *outlen);`：从共享池读取样本到 `outbuf` 并设置 `*outlen`，受信号量保护。
    * `int shm_get_count();`：返回当前样本数量（池中已写入样本数）。
    * `int shmsem_cleanup();`：分离并删除共享内存与信号量资源（`shmdt` / `shmctl IPC_RMID` / `semctl IPC_RMID`）。
