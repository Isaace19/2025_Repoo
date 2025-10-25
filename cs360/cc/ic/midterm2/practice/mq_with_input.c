#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <mqueue.h>

#define QNAME "/dynqueue"

void worker(mqd_t q) {
    int n;
    while (1) {
        mq_receive(q, (char*)&n, sizeof(n), NULL);
        if (n == 0) break;      // sentinel
        printf("PID %d got %d\n", getpid(), n);
    }
    mq_close(q);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num_workers>\n", argv[0]);
        return 1;
    }
    int num_workers = atoi(argv[1]);
    if (num_workers <= 0 || num_workers > 50) {
        fprintf(stderr, "Invalid worker count\n");
        return 1;
    }

    struct mq_attr attr = {0};
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(int);

    mqd_t q = mq_open(QNAME, O_CREAT | O_RDWR, 0666, &attr);
    if (q == (mqd_t)-1) { perror("mq_open"); exit(1); }

    pid_t *pids = malloc(sizeof(pid_t) * num_workers);

    // Spawn workers
    for (int i = 0; i < num_workers; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            worker(q);
            _exit(0);
        }
    }

    // Parent generates work
    for (int i = 1; i <= 20; i++)
        mq_send(q, (const char*)&i, sizeof(i), 0);

    int sentinel = 0;
    for (int i = 0; i < num_workers; i++)
        mq_send(q, (const char*)&sentinel, sizeof(sentinel), 0);

    // Wait for all workers
    for (int i = 0; i < num_workers; i++)
        waitpid(pids[i], NULL, 0);

    free(pids);
    mq_close(q);
    mq_unlink(QNAME);
    return 0;
}

