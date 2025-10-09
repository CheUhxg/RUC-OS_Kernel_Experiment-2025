/* workload.c
 * usage examples:
 *   ./workload -c 2 -i 2 -m 1 -y 1 -t 60
 *   ./workload -c 4        # 4 cpu-bound children, default duration 60s
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/prctl.h>
#include <errno.h>

static volatile sig_atomic_t stop = 0;
static int run_seconds = 60;
static char *io_dir = "/tmp";

static void handle_sigterm(int s){ (void)s; stop = 1; }

static void set_name(const char *name){
    prctl(PR_SET_NAME, name, 0,0,0);
}

/* cpu-bound: tight busy loop doing some arithmetic */
static void cpu_work(void){
    set_name("cpu_task");
    unsigned long a = 1;
    while(!stop){
        /* some non-trivial ops to avoid being optimized out */
        a ^= a << 13;
        a ^= a >> 7;
        a ^= a << 17;
        if ((a & 0xfff) == 0) sched_yield();
    }
    _exit(0);
}

/* io-bound: periodic small writes + fsync to force IO wait */
static void io_work(void){
    char fname[256];
    snprintf(fname, sizeof(fname), "%s/io_%d.dat", io_dir, getpid());
    set_name("io_task");
    int fd = open(fname, O_CREAT | O_WRONLY | O_TRUNC, 0600);
    if (fd < 0) _exit(1);
    const size_t B = 4096;
    void *buf = malloc(B);
    memset(buf, 'A' + (getpid()%26), B);
    struct timespec ts = {0, 50 * 1000 * 1000}; /* 50ms */
    while(!stop){
        if (write(fd, buf, B) != (ssize_t)B) break;
        if (fsync(fd) != 0) break;
        nanosleep(&ts, NULL); /* yield to I/O & sleep a bit */
    }
    close(fd);
    _exit(0);
}

/* yielding task: frequently calls sched_yield() or sleeps */
static void yield_work(void){
    set_name("yield_task");
    struct timespec ts = {0, 200 * 1000}; /* 0.2ms */
    while(!stop){
        sched_yield();
        nanosleep(&ts, NULL);
    }
    _exit(0);
}

static pid_t spawn_child(char type){
    pid_t pid = fork();
    if (pid < 0) return -1;
    if (pid == 0){
        signal(SIGTERM, handle_sigterm);
        signal(SIGINT, handle_sigterm);
        switch(type){
            case 'C': cpu_work(); break;
            case 'I': io_work(); break;
            case 'Y': yield_work(); break;
            default: _exit(1);
        }
    }
    return pid;
}

int main(int argc, char **argv){
    int opt;
    int nc=0, ni=0, nm=0, ny=0;
    while((opt = getopt(argc, argv, "c:i:m:y:t:d:")) != -1){
        switch(opt){
            case 'c': nc = atoi(optarg); break;
            case 'i': ni = atoi(optarg); break;
            case 'y': ny = atoi(optarg); break;
            case 't': run_seconds = atoi(optarg); break;
            case 'd': io_dir = optarg; break;
            default:
                fprintf(stderr,"Usage: %s [-c cpu] [-i io] [-m mixed] [-y yield] [-t seconds] [-d io_dir]\n", argv[0]);
                exit(1);
        }
    }

    signal(SIGTERM, handle_sigterm);
    signal(SIGINT, handle_sigterm);

    int total = nc + ni + nm + ny;
    if (total == 0){ fprintf(stderr,"no children requested\n"); return 1; }

    pid_t *pids = calloc(total, sizeof(pid_t));
    int idx = 0;
    for (int i=0;i<nc;i++) { pids[idx] = spawn_child('C'); printf("spawn cpu pid=%d\n", pids[idx]); idx++; }
    for (int i=0;i<ni;i++) { pids[idx] = spawn_child('I'); printf("spawn io  pid=%d\n", pids[idx]); idx++; }
    for (int i=0;i<ny;i++) { pids[idx] = spawn_child('Y'); printf("spawn yld pid=%d\n", pids[idx]); idx++; }

    /* wait run_seconds or until signal */
    for (int s=0; s<run_seconds && !stop; s++) sleep(1);
    stop = 1;
    /* ask children to exit */
    for (int i=0;i<total;i++) if (pids[i] > 0) kill(pids[i], SIGTERM);
    /* wait them */
    for (int i=0;i<total;i++) if (pids[i] > 0) waitpid(pids[i], NULL, 0);

    free(pids);
    printf("all children exited\n");
    return 0;
}
