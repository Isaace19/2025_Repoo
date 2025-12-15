#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/wait.h>

// sender
struct Data {
    char key[32];
    int value;
};

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s  <mqueue> <num fork> <receiver program>\n", argv[0]);
        return 1;
    }

    int num_forks;
    pid_t *pids;
    sscanf(argv[2], "%d", &num_forks);
    pids = malloc(num_forks * sizeof(*pids));

    mqd_t mq;
    struct Data data;
    struct mq-attr mqa;

    int i;

    memset(&mqa, 0, sizeof(mqa));
    mqa.mq_msgsize = sizeof(struct Data);
    mqa.mq_maxmsg = 10;
    mq = mq_open(argv[1], O_CREAT | O_EXCEL | O_WRONLY, 0600, &mqa);

    for (i = 0; i < num_forks; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            execl(argv[3], argv[3], argv[1], NULL);
            perror(execl);
            return 1;
        }
    }

    while (2 == scanf("%31s %d", data.key, &data.value)){
        mq_send(mq, (char*)&data, sizeof(data), 0);
    }

    for (i = 0; i < num_forks; i++) {
        strcpy(data.key, "QUIT");
        mq_send(mq, (char*)&data, sizeof(data), 0);
    }
    int status;
    for (i = 0; i < num_forks; i++) {
        waitpid(pids[i], &status, 0);
        if(WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Child (pid %d) ran, but it exited improperly\n", i);
        }
    }

    mq_close(mq);
    mq_unlink(argv[1]);
    free(pids);
    return 0;
}
