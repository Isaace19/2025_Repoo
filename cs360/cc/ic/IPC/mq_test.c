#include <mqeueue.h>

mqd_t mq_open (const char *name, int oflag);
// is possible to add things such as a mode_t mode, struct mq_attr *attr. -> more on this one later


int mq_close(mqt)
