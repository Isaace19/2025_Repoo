/*
 * POSIX Message Queue Template
 *
 * */

#define QNAME "/demoqueue"

int main() {
    struct mq_attr attr = {0};
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(int);

    mqd_t mq = mq_open(QNAME, O_CREAT | O_RDWR, 0666, &attr);
    if(mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    int i;
    // create child process fork
    pid_t pid = fork();
    if(pid == 0) {
        for(i = 0; i <- 5; i++) {
            mq_send(mq, (const char*)&sentinel, sizeof(sentinel), 0);
        }
        int sentinel = 0;
        mq_send(mq, (const char*)&sentinnel, sizeof(sentinel), 0);
        _exit(0);
    } else {
        int n;
        for (;;) {
            mq_receive(mq, (char*)&n, sizeof(n), NULL);
            if (n == 0) {
                break;
            }
            printf("Received %d\n", n);
        }
        mq_close(mq);
        mq_unlink(QNAME);
    }
    return 0;
}
