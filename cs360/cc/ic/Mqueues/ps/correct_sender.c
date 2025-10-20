#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/wait.h>

// SENDER
struct Data {
    char   key[32];
    int value;
};

int main(int argc, char *argv[])
{
    if (argc < 4) {
        printf("Usage: %s <mqueue file> <num forks> <receiver program>\n", argv[0]);
        return 1;
    }
    // printf("SENDER: Hello, World!\n");
	int num_forks;
	sscanf(argv[2], "%d", &num_forks);
	// gives PIDs found
	pid_t *pids;
	 
	pids = malloc(num_forks * sizeof(*pids));


	mqd_t mq;
	struct Data data;
	struct mq_attr mqa;
	
	int i; // for our loops
	
	memset(&mqa, 0, sizeof(mqa));
	mqa.mq_msgsize = sizeof(struct Data);
	mqa.mq_maxmsg = 10; // linux default - limit

	mq = mq_open(argv[1], O_CREAT | O_EXCL | O_WRONLY, 0600, mqa);

	for (i = 0; i < num_forks; i++) {
		pids[i] = fork();
		if (pids[i] == 0) {
			// give command line argument name of the program
			execl(argv[3], argv[3], argv[1], NULL);
			perror("execl");
			return 1;
		}
	}

	while (2 == scanf("%31s %d", data.key, &data.value)) {
		mq_send(mq, (char*)&data, sizeof(data), 0);
	}

	for (i = 0; i < num_forks; i+=1){
		strcpy(data.key, "QUIT");
		mq_send(mq, (char*)&data, sizeof(data), 0);
	}
	
    int status;
	// wait for children after all messages have been sent
	for (i = 0; i < num_forks; i+=1) {
		waitpid(pids[i], &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			fprintf(stderr, "Child (pid %d) ran, but it exited improperly.\n", i);
		}
	}

	mq_close(mq);
	// unlink the mq - deletes off the file system
	mq_unlink(argv[1]);
	free(pids);
    return 0;
}
