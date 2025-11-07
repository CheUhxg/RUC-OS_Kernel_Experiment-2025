#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
        perror("socketpair");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // 子进程：读数据
        close(sv[1]);
        char buffer[1024];
        struct timeval start, end;
        gettimeofday(&start, NULL);
        for (int i = 0; i < 1024; i++) {
            ssize_t n = 0, r;
            while (n < (ssize_t)sizeof(buffer)) {
                r = read(sv[0], buffer + n, sizeof(buffer) - n);
                if (r <= 0) break;
                n += r;
            }
        }
        gettimeofday(&end, NULL);
        // 计算时间差
        long seconds = end.tv_sec - start.tv_sec;
        long micros = end.tv_usec - start.tv_usec;
        if (micros < 0) { micros += 1000000; seconds--; }
        double elapsed = seconds * 1000.0 + micros / 1000.0;
        printf("[UNIX域SOCK_STREAM] 耗时: %.3f 毫秒\n", elapsed);
        close(sv[0]);
    } else if (pid > 0) {
        // 父进程：写数据
        close(sv[0]);
        char data[1024];
        memset(data, 0, sizeof(data));
        for (int i = 0; i < 1024; i++) {
            ssize_t n = 0, w;
            while (n < (ssize_t)sizeof(data)) {
                w = write(sv[1], data + n, sizeof(data) - n);
                if (w <= 0) break;
                n += w;
            }
        }
        close(sv[1]);
        wait(NULL);
    } else {
        perror("fork");
        return 1;
    }
    return 0;
}
