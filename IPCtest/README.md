# IPCtest

* 课堂演示：测试常见进程间通信（IPC）方式的简单性能对比。

包括：
- 匿名管道：`pipe_anonymous`
- 有名管道：`pipe_named_reader` + `pipe_named_writer`
- 共享内存（配合信号量同步）：`shared_memory`
- POSIX 消息队列：`msg_queue`
- POSIX 信号量：`semaphore_sync`
- UNIX 域套接字（SOCK_STREAM）：`socket_unix_stream`
- UNIX 域套接字（SOCK_DGRAM）：`socket_unix_dgram`

运行全部测试：

```sh
make test
```

默认每个用例会重复运行多次（见 `Makefile` 中的 `TEST_RUNS`）。