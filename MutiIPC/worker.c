/*
 Worker:
 - argv[1] 为写入 master 的 pipe fd（整数）
 - 连接到 msgqueue、shared memory、sem
 - 处理循环：从消息队列取任务 -> 等待 SIGUSR1 启动 -> 读取共享内存样本 -> fork 子进程执行 target_vuln（通过 stdin 传入样本） -> 检测崩溃并把结果写回 master pipe
*/

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "ipc_common.h"

extern int msgqueue_init(key_t);
extern int msgqueue_recv(struct fuzz_task *, long);
extern int shmsem_init(key_t, key_t);
extern int shm_read_sample(int, char*, int*);
extern int shm_get_count();

volatile sig_atomic_t go_flag = 0;
void sigusr1_handler(int s) { (void)s; go_flag = 1; }

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "worker: missing pipe fd arg\n"); exit(1);
    }
    int master_fd = atoi(argv[1]);

    signal(SIGUSR1, sigusr1_handler);

    if (shmsem_init(SHM_KEY, SEM_KEY) < 0) exit(1);
    if (msgqueue_init(MSG_KEY) < 0) exit(1);

    char sample_buf[MAX_SAMPLE_SIZE];
    int sample_len;

    while (1) {
        struct fuzz_task t;
        if (msgqueue_recv(&t, 0) < 0) {
            /* error or no message */
            sleep(1);
            continue;
        }
        /* 等待 SIGUSR1 */
        while (!go_flag) {
            usleep(10000);
        }
        go_flag = 0;

        if (shm_read_sample(t.sample_idx, sample_buf, &sample_len) < 0) {
            dprintf(master_fd, "worker[%d]: failed read sample %d\n", getpid(), t.sample_idx);
            continue;
        }

        /* 创建一个子进程来执行 target_vuln，并把 sample 通过 pipe 传入其 stdin */
        int inpipe[2];
        if (pipe(inpipe) < 0) {
            perror("pipe");
            continue;
        }
        pid_t cpid = fork();
        if (cpid < 0) { perror("fork"); close(inpipe[0]); close(inpipe[1]); continue; }
        if (cpid == 0) {
            /* child: 读取 stdin，从 inpipe 读取；关闭写端 */
            dup2(inpipe[0], STDIN_FILENO);
            close(inpipe[0]);
            close(inpipe[1]);
            /* exec target */
            execl("./target_vuln", "./target_vuln", NULL);
            perror("execl target_vuln");
            _exit(1);
        } else {
            /* parent: 关闭 read end，写入 sample，然后等待 child 结束 */
            close(inpipe[0]);
            ssize_t w = write(inpipe[1], sample_buf, sample_len);
            (void)w;
            close(inpipe[1]);

            int status;
            waitpid(cpid, &status, 0);
            if (WIFSIGNALED(status)) {
                int sig = WTERMSIG(status);
                dprintf(master_fd, "PID %d: CRASHED on sample %d (signal %d)\n", cpid, t.sample_idx, sig);
            } else {
                if (WIFEXITED(status)) {
                    int code = WEXITSTATUS(status);
                    dprintf(master_fd, "PID %d: exit %d on sample %d\n", cpid, code, t.sample_idx);
                } else {
                    dprintf(master_fd, "PID %d: unknown result on sample %d\n", cpid, t.sample_idx);
                }
            }
        }
    }

    return 0;
}
