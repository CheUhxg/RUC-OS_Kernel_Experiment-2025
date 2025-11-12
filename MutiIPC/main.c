#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include "ipc_common.h"

/* 外部接口（来自 msgqueue.c / shm_sem.c） */
extern int msgqueue_init(key_t);
extern int msgqueue_send(struct fuzz_task *);
extern int msgqueue_cleanup();

extern int shmsem_init(key_t, key_t);
extern int shm_write_sample(int, const char*, int);
extern int shm_get_count();
extern int shmsem_cleanup();

#define WORKER_COUNT 2

int pipes_fd[WORKER_COUNT][2];
pid_t workers[WORKER_COUNT];

/* 标记是否已收到崩溃 */
volatile sig_atomic_t got_crash = 0;

void cleanup_all() {
    msgqueue_cleanup();
    shmsem_cleanup();
}

/* 当我们检测到崩溃（主进程收到包含 "CRASHED" 的报告）时，
   会设置 got_crash 并在外层循环中结束并杀掉 workers。 */

int main(int argc, char **argv) {
    printf("Master starting (persistent fuzz until crash)\n");

    if (shmsem_init(SHM_KEY, SEM_KEY) < 0) exit(1);
    if (msgqueue_init(MSG_KEY) < 0) exit(1);

    /* 准备样本池（教学示例） */
    const char *samples[] = {
        "hello\n",
        "foo\n",
        "CRASH\n", /* 会触发 target_vuln abort() */
        "abcd\n"
    };
    int sample_count = sizeof(samples)/sizeof(samples[0]);
    for (int i = 0; i < sample_count; ++i) {
        shm_write_sample(i, samples[i], strlen(samples[i]));
    }
    printf("Master wrote %d samples\n", sample_count);

    /* 创建 pipes 并 fork worker（用 exec 启动 worker 可保持原样） */
    for (int i = 0; i < WORKER_COUNT; ++i) {
        if (pipe(pipes_fd[i]) < 0) { perror("pipe"); cleanup_all(); exit(1); }

        pid_t pid = fork();
        if (pid < 0) { perror("fork"); cleanup_all(); exit(1); }
        if (pid == 0) {
            /* child: 关闭其他 fds，保留自己的写端并 exec worker */
            char fdarg[32];
            snprintf(fdarg, sizeof(fdarg), "%d", pipes_fd[i][1]);
            for (int j = 0; j < WORKER_COUNT; ++j) {
                close(pipes_fd[j][0]);
                if (j != i) close(pipes_fd[j][1]);
            }
            execl("./worker", "./worker", fdarg, NULL);
            perror("execl worker");
            _exit(1);
        } else {
            /* parent: 保存 pid，关闭写端（worker 写） */
            workers[i] = pid;
            close(pipes_fd[i][1]);
        }
    }

    /* wait a bit for workers to init */
    sleep(1);

    /* 下面我们做一个持续派发循环，直到 got_crash 被触发。
       派发逻辑：随机或轮询选择样本索引并发送到消息队列，然后广播 SIGUSR1 给所有 worker。
    */
    srand((unsigned)time(NULL) ^ getpid());
    int sample_idx = 0;
    printf("Master: starting persistent dispatch loop\n");

    /* 主循环：在未检测到崩溃时继续派发、读取 pipes（非阻塞检查） */
    fd_set rfds;
    int maxfd = 0;
    for (int i = 0; i < WORKER_COUNT; ++i) if (pipes_fd[i][0] > maxfd) maxfd = pipes_fd[i][0];

    while (!got_crash) {
        /* 选择样本：随机或轮询 */
        sample_idx = (rand() % sample_count);
        struct fuzz_task t;
        t.sample_idx = sample_idx;
        t.mtype = 1;
        if (msgqueue_send(&t) < 0) {
            fprintf(stderr, "Master: failed to send task %d\n", sample_idx);
        } else {
            /* 通知所有 worker 开始（广播 SIGUSR1） */
            for (int w = 0; w < WORKER_COUNT; ++w) kill(workers[w], SIGUSR1);
        }

        /* 小延迟，避免过度占用 CPU；可根据目标调整 */
        usleep(100000); /* 0.1s */

        /* 轮询读取 pipes：使用 select 但设置短超时，及时发现崩溃报告 */
        FD_ZERO(&rfds);
        for (int i = 0; i < WORKER_COUNT; ++i) FD_SET(pipes_fd[i][0], &rfds);
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 200000; /* 200ms 等待报告 */
        int sel = select(maxfd + 1, &rfds, NULL, NULL, &tv);
        if (sel > 0) {
            for (int i = 0; i < WORKER_COUNT; ++i) {
                if (FD_ISSET(pipes_fd[i][0], &rfds)) {
                    char buf[512];
                    ssize_t r = read(pipes_fd[i][0], buf, sizeof(buf) - 1);
                    if (r > 0) {
                        buf[r] = '\0';
                        printf("Master: report from worker[%d] pid=%d: %s", i, workers[i], buf);
                        /* 如果包含 CRASHED（worker 在崩溃时发回），则认为是崩溃并退出 */
                        if (strstr(buf, "CRASHED") != NULL || strstr(buf, "CRASH") != NULL) {
                            got_crash = 1;
                            break;
                        }
                    }
                }
            }
        }
        /* 如果 select 返回 0（超时），继续下一轮派发；当 got_crash 被设置会跳出循环 */
    }

    /* 我们检测到崩溃，停止并清理：向 workers 发 SIGTERM，然后回收并输出 */
    printf("Master: crash detected, shutting down workers\n");
    for (int i = 0; i < WORKER_COUNT; ++i) {
        kill(workers[i], SIGTERM);
        waitpid(workers[i], NULL, 0);
        close(pipes_fd[i][0]);
    }

    cleanup_all();
    printf("Master exiting (crash-driven stop)\n");
    return 0;
}
