#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>

#define VS_MAGIC 0xA4
#define VS_INC1 _IO(VS_MAGIC, 1)
#define VS_INC2 _IO(VS_MAGIC, 2)
#define VS_GET  _IOR(VS_MAGIC, 3, int)

static int fd;
static int loops = 100000;
static int inc2_threads = 2; // number doing inc2
static int total_threads = 4;

static void* run_inc(void* arg)
{
    int use_inc2 = (int)(long) arg;
    unsigned long cmd = use_inc2 ? VS_INC2 : VS_INC1;
    for (int i = 0; i < loops; ++i) {
        if (ioctl(fd, cmd) < 0) {
            perror("ioctl inc");
            break;
        }
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    if (argc >= 2) total_threads = atoi(argv[1]);
    if (argc >= 3) inc2_threads  = atoi(argv[2]);
    if (argc >= 4) loops         = atoi(argv[3]);
    if (inc2_threads > total_threads) inc2_threads = total_threads;
    if (total_threads <= 0 || loops <= 0) {
        fprintf(stderr, "Usage: %s [total_threads] [inc2_threads] [loops]\n", argv[0]);
        return 1;
    }

    fd = open("/dev/visit_shared", O_RDWR);
    if (fd < 0) {
        perror("open /dev/visit_shared");
        return 1;
    }

    pthread_t* tids = malloc(sizeof(pthread_t) * total_threads);
    if (!tids) return 1;

    int inc1_threads = total_threads - inc2_threads;
    for (int i = 0; i < inc2_threads; ++i)
        pthread_create(&tids[i], NULL, run_inc, (void*)1);
    for (int i = 0; i < inc1_threads; ++i)
        pthread_create(&tids[inc2_threads + i], NULL, run_inc, (void*)0);

    for (int i = 0; i < total_threads; ++i)
        pthread_join(tids[i], NULL);

    int val = 0;
    if (ioctl(fd, VS_GET, &val) < 0) {
        perror("ioctl get");
        free(tids);
        close(fd);
        return 1;
    }

    long expected = (long)loops * inc1_threads * 1L + (long)loops * inc2_threads * 2L;
    long lost = expected - val;
    double ratio = expected ? (100.0 * lost / expected) : 0.0;
    printf("Threads=%d (inc1=%d inc2=%d) Loops=%d\n", total_threads, inc1_threads, inc2_threads, loops);
    printf("Expected=%ld Observed=%d Lost=%ld Ratio=%.2f%%\n", expected, val, lost, ratio);

    free(tids);
    close(fd);
    return 0;
}