#ifndef IPC_COMMON_H
#define IPC_COMMON_H

#include <sys/types.h>

#define Q_KEY 0x1111

#define MSGTYPE 1
#define MAX_TEXT 256

struct msgbuf {
    long mtype;
    char mtext[MAX_TEXT];
};

#endif
