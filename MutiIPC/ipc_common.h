#ifndef IPC_COMMON_H
#define IPC_COMMON_H

#include <sys/types.h>

#define SHM_KEY 0x1234
#define SEM_KEY 0x5678
#define MSG_KEY 0x2468

#define MAX_SAMPLES 64
#define MAX_SAMPLE_SIZE 256

/* 消息队列任务结构 */
struct fuzz_task {
    int sample_idx;   /* 要使用的共享内存样本索引 */
    long mtype;       /* 消息类型（必须为 long） */
};

/* 消息封装（用于 msgsnd/msgrcv） */
struct fuzz_msg {
    long mtype;
    struct fuzz_task task;
};

#endif /* IPC_COMMON_H */
