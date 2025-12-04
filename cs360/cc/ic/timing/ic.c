/* Coding Notes on Timers from 11/14/2025 CS360 Lecture */

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

/* New timers need -lrt such as timer_create
 *
 * include <time.h> for nanosleep has two params (time to sleep, ????) -> find out what second value is
 * timers and sleeps are located in 4 different headers 
 *
 *
 * */





static void sighandler(int sig);

int cont = 0;
int main(int argc, char* argv[]) {
    // interval timer first, then one shot timer - can have microsecond precision     
    struct itimerval it = {{1, 0}, {2,0}};
    //alarm(2);
   
    // from struct itimerval we have a periodic interval and a time to count down from
    signal(SIGALRM, sighandler);

    setitimer(ITIMER_REAL, &it, NULL);
    while (cont < 3) {
        // capture signal, cont to 3, then we are done
    }
    printf("Hello World\n");

    //for (int i = 0; i < 10000000 && cont; i++1) {
    //    printf("Loop\n");
    //}
}

static void sighandler(int sig) {
   if (sig == SIGALRM) {
        cont += 1;
        printf("Cont is now %d\n", cont);
    } 
}
