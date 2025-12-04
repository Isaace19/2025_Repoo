/*
 * Basics of signals and daemons for server programming
 *
 * Infinite for loop that keeps checking for accepts and connects
 * otherwise, we set it to lock, if client then nonlocking
 *
 * handle as many clients as possible until interrupt signal
 *
 *
 * */

#include <signal.h>
// Date: 11/07/2025 - TODO-> Fix this code to work on interrupt signals


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void sighandler(int sig);
// close serve socket, unlink socket, cleanup
void exithandler(void);

int sock
char sockname[256];


// * exam = the optimzier might be turned on so we might need to use volatile signals
// pthreads & DSO are for the exam
volatile bool quit;
int refcount = 3;
int main() {
    signal(SIGINT, sighandler);
    signal(SIGSEGV, sighandler);
    atexit(exithandler);
    atexit(exithandler);
    atexit(exithandler);
    for (;;){

    }

    while(!quit) {
        // fprintf(stderr, "Hellow World\n");
        quit = true;
    }
    fprintf(stderr, "Close & Quit\n");

}

void exithandler(void){
    //refcount -= 1;
    //fprintf(stderr, )
    close(sock);
    unlink(sockname);
}


void sighandler(int sig) {
    if (SIGSEGV == sig) {
        fprintf(stderr, "----------------");
        fprintf(stderr, "-- SEG FAULT --");
        fprintf(stderr, "----------------");
        _Exit(EXIT_FAILURE);
        //exit(EXIT_SUCCESS); // both of lowercase exits have cleanup utils
        //Exit(EXIT_FAILURE) // uppercase skips cleanup utils

        // we would use these to cleanup our sockets
    }   else if (SIGINT == sig){
        // exit(EXIT_SUCCESS);
        quit = true;
        fprintf()
    }





}


