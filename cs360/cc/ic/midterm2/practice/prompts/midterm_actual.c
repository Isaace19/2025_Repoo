
typedef struct {
  double x;
  double y;
  double z;
} Data;

ftruncate(shm, sizeof(double));
double *shared_total =
    mmap(NULL, sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
if (shared_total == MAP_FAILED {
  perror("mmap");
  exit(1);
}

// set the shared_total memory to 0
*shared_total = 0;

// the send command
stuct Data message = {-1, -1, -1};
int i;
pid_t pids = fork();
for(i = 0; i < num_workers; i++) {
  int n;
  Data message;
  if (pids == 0) {
    execl(worker, worker, mq, sem, shm, NULL);
    mq_send(mq, (const char *)&message, sizeof(message), 0);
  }
  int sentinel = 0;
  mq_send(mq, (const char *)&sentinel, sizeof(sentinel), 0);

  double sum = shared_total + compute;
  *shared_total += sum;

  mq_close(mq);
  sem_close(sem);
  close(shm);
  munmap(shared_total, sizeof(int));

  exit(0);

  // terminate out and close/unlink all files inside of this child process
}

waitpid(pids, 0, NULL);

printf("%lf-3\n", *shared_total);
