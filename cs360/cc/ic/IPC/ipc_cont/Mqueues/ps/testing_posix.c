#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mqueue.h"  // shim’s header

int main(void) {
    const char *qname = "/testqueue";
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 256;
    attr.mq_curmsgs = 0;

    mqd_t mq = mq_open(qname, O_CREAT | O_RDWR, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    char *msg = "Hello, world";
    if (mq_send(mq, msg, strlen(msg) + 1, 0) == -1) {
        perror("mq_send");
        mq_close(mq);
        mq_unlink(qname);
        exit(1);
    }

    char buffer[256];
    if (mq_receive(mq, buffer, sizeof(buffer), NULL) == -1) {
        perror("mq_receive");
        mq_close(mq);
        mq_unlink(qname);
        exit(1);
    }

    printf("Received: %s\n", buffer);

    mq_close(mq);
    mq_unlink(qname);
    return 0;
}
