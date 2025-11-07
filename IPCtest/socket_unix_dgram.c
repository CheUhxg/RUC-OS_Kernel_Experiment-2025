#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_DGRAM, 0, sv) < 0) {
        perror("socketpair");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // 子进程：接收数据
        close(sv[1]);
        char buffer[1024];
        struct timeval start, end;
        gettimeofday(&start, NULL);
        for (int i = 0; i < 1024; i++) {
            ssize_t r = recv(sv[0], buffer, sizeof(buffer), 0);
            if (r <= 0) break;
        }
        gettimeofday(&end, NULL);
        long seconds = end.tv_sec - start.tv_sec;
        long micros = end.tv_usec - start.tv_usec;
        if (micros < 0) { micros += 1000000; seconds--; }
        double elapsed = seconds * 1000.0 + micros / 1000.0;
        printf("[UNIX域SOCK_DGRAM] 耗时: %.3f 毫秒\n", elapsed);
        close(sv[0]);
    } else if (pid > 0) {
        // 父进程：发送数据
        close(sv[0]);
        char data[1024];
        memset(data, 0, sizeof(data));
        for (int i = 0; i < 1024; i++) {
            ssize_t w = send(sv[1], data, sizeof(data), 0);
            if (w <= 0) break;
        }
        close(sv[1]);
        wait(NULL);
    } else {
        perror("fork");
        return 1;
    }
    return 0;
}
