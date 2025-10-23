/*
Marshaller Implementation for IPC System
Based roughly on the hfactor lab and the zybook programming stuff.
*/

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

// TODO -> understand sentinal values for the marshaller when receiving from the
// message queue

int main(int argc, char *argv[]) {
  int jog;
  int num;
  int i;
  int shm;
  // create the semaphore
  sem_t *sem;

  // error check the command line argument.
  if (argc < 6) {
    printf("Usage: %s <sem file> <shm file> <num> <jog> <worker>\n", argv[0]);
    return 1;
  }
  if (1 != sscanf(argv[3], "%d", &num) || num <= 0 || num > 100000) {
    printf("Invalid number for num '%s'\n", argv[3]);
    return 1;
  }
  if (1 != sscanf(argv[4], "%d", &jog) || jog <= 0 || jog > 100000) {
    printf("Invalid number for jog '%s'\n", argv[4]);
    return 1;
  }

  /*
    create the semaphore and shared memory object
    Set their flags to the proper command line arguments being read in, so
    semaphore would take in argv[1] & shared memory would take in argv[2]

    both objects are set to permission of 0600 (read and write for owner only)
    error check both objects after creation: exit if any failures with perror
  */

  sem = sem_open(argv[1], O_CREAT | O_EXCL | O_RDWR, 0600, 1);
  if (sem == NULL) {
    perror("sem_open");
    return 1;
  }

  shm = shm_open(argv[2], O_CREAT | O_RDWR | O_EXCL, 0600);
  if (shm == -1) {
    perror("shm_open");
    return 1;
  }

  /*
    Set the size of the shared memory object to 4 bytes using ftruncate
    then write the integer i = 0 to the shared memory object to initialize it.
    Shared memory uses the regular read/write functions after being created, but
    still rquires unlinking at the end.
  */
  ftruncate(shm, 4);
  i = 0;
  write(shm, &i, sizeof(i));
  // ensure that children are opened properly after scanning them

  /*
  Heavy weight threads are created by forking off the processes in a loop
  equal to the number of processes specified by the command line argument.
  The reason that pid_t *pids is created via malloc is because the number of
  processes is not known at compile time, so it's allocated dynamically.
  */
  pid_t *pids = malloc(sizeof(pid_t) * num);
  // create via fork
  for (i = 0; i < num; i++) {
    pids[i] = fork();
    if (pids[i] == 0) {
      // when pid is 0 then we know it is a child process, then we go through
      // the jog and values
      close(shm); // close share memory -> memory management purposes
      char value[16];
      char sjog[16];
      /*
      Every parameter to exec must be a string, so we take the integer
      and convert it to a string via sprintf
      */
      // convert the num and jog values to strings to print out later
      sprintf(value, "%d", num);
      sprintf(sjog, "%d", jog);
      // while in the child process we then run execl to execute the worker
      // process
      execl(argv[5], argv[5], argv[1], argv[2], value, sjog, NULL);
      // run the execl command and feed in the string value and sjog
      perror("execl");
      return 1;
    }
  }
  // wait for the children process to finish after work is given from command
  // line arguments children quits when work is done via wait pid_t

  for (i = 0; i < num; i++) {
    waitpid(pids[i], NULL, 0);
  }
  /*
  close out the shared memory and semaphore after all children processes
  are done. The unlinking is used in order to remove the semaphore and
  shared memory object from the system.
  */

  close(shm);
  // shm_unlink(argv[2]);
  sem_close(sem);
  // sem_unlink(argv[1]);
  free(pids);
  return 0;
}
