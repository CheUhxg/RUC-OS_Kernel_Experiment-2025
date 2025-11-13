// sender.c -- 不停往 Q1 发送 "print good"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "ipc_common.h"

int main(void) {
    int qid = msgget(Q_KEY, IPC_CREAT | 0666);
    if (qid < 0) { perror("msgget Q1"); return 1; }

    struct msgbuf m;
    m.mtype = MSGTYPE;
    strncpy(m.mtext, "good", MAX_TEXT-1);
    m.mtext[MAX_TEXT-1] = '\0';

    printf("sender: qid=%d\n", qid);
    while (1) {
        if (msgsnd(qid, &m, strlen(m.mtext)+1, 0) < 0) {
            perror("msgsnd");
            break;
        }
        sleep(2);
    }
    return 0;
}
