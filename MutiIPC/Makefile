CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O0
LDFLAGS =
SRCS = main.c worker.c msgqueue.c shm_sem.c target_vuln.c
OBJS = $(SRCS:.c=.o)
TARGETS = main worker target_vuln

all: main worker target_vuln

main: main.c msgqueue.c shm_sem.c
	$(CC) $(CFLAGS) -o main main.c msgqueue.c shm_sem.c $(LDFLAGS)

worker: worker.c msgqueue.c shm_sem.c
	$(CC) $(CFLAGS) -o worker worker.c msgqueue.c shm_sem.c $(LDFLAGS)

target_vuln: target_vuln.c
	$(CC) $(CFLAGS) -o target_vuln target_vuln.c

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean