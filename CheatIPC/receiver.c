// receiver.c -- 从 Q2 读并打印
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "ipc_common.h"

int main(void) {
    int q2 = msgget(Q_KEY, IPC_CREAT | 0666);
    if (q2 < 0) { perror("msgget Q2"); return 1; }

    struct msgbuf m;
    printf("receiver: waiting on Q2\n");
    while (1) {
        ssize_t r = msgrcv(q2, &m, sizeof(m.mtext), 0, 0);
        if (r < 0) {
            perror("msgrcv Q2");
            break;
        }
        m.mtext[sizeof(m.mtext)-1] = '\0';
        printf("receiver: got message: '%s'\n", m.mtext);
        fflush(stdout);
        if (strcmp(m.mtext, "good") == 0) {
            puts("B prints: good");
        } else {
            printf("[$] cheat success!\n");
        }
        sleep(10);
    }
    return 0;
}
