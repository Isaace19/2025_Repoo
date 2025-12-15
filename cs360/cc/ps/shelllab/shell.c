// writing testing code for a simple shell program
// Author: Isaac Abella

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_ARGS 128

struct job {
    pid_t pid;
    char command[256];
};

static struct job jobs[128];
int num_job = 0;

void prompt(void) {
    char *user = getenv("USER");
    if (!user) {
        user = "unknown";
    }

    char *home = getenv("HOME");

    char curr_dir[PATH_MAX];
    if (!getcwd(curr_dir, sizeof(curr_dir))) {
        perror("getcwd");
        strcpy(curr_dir, "?");
    }

    char display[PATH_MAX];
    if (home && strncmp(curr_dir, home, strlen(home)) == 0) {
        // replace the leading $HOME with the tilde
        snprintf(display, sizeof(display), "~%s", curr_dir + strlen(home));
    } else {
        snprintf(display, sizeof(display), "%s", curr_dir);
    }

    char combined[PATH_MAX + 256];
    // Note the space after '>'
    snprintf(combined, sizeof(combined), "%s@cosc360:%s> ", user, display);
    printf("%s", combined);
    fflush(stdout);
}

int parseline(char *line,
              char **argv,
              char **infile,
              char **outfile,
              int *append,
              int *background)
{
    *infile = NULL;
    *outfile = NULL;
    *append = 0;
    *background = 0;

    // strip newline, tokenize etc.

    while (tok != NULL && argc < MAX_ARGS - 1) {

        // 1. <file
        if (tok[0] == '<') {
            *infile = tok + 1;
            tok = strtok(NULL, " \t");
            continue;
        }

        if (tok[0] == '>' && tok[1] == '>') {
            *append = 1;
            *outfile = tok + 2;
            tok = strtok(NULL, " \t");
            continue;
        }

        if (tok[0] == '>') {
            *append = 0;
            *outfile = tok + 1;
            tok = strtok(NULL, " \t");
            continue;
        }

        if (strcmp(tok, "&") == 0) {
            *background = 1;
            tok = strtok(NULL, " \t");
            continue;
        }

        argv[argc++] = tok;
        tok = strtok(NULL, " \t");
    }

    argv[argc] = NULL;
    return argc;
}


int main(int argc, char *argv[]) {
    char line[4096];
    char *args[MAX_ARGS];
    char *infile, *outfile;
    int background;

    for (;;) {
        prompt();

        if (!fgets(line, sizeof(line), stdin)) {
            putchar('\n');
            break;  // EOF
        }

        int argcount = parseline(line, args, &infile, &outfile, &background);
        if (argcount < 0) {
            // parse error
            continue;
        }

        if (argcount == 0) {
            // empty line
            continue;
        }

        if (strcmp(args[0], "exit") == 0) {
            break;
        }

   }

    return 0;
}

