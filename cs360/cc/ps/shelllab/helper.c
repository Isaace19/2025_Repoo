#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>

#define MAX_ARGS  128
#define MAX_JOBS  128

struct job {
    pid_t pid;
    char  cmdline[256];
    int   active;
};

static struct job jobs[MAX_JOBS];
static int num_jobs = 0;

void prompt(void) {
    char *user = getenv("USER");
    if (!user) user = "unknown";

    char *home = getenv("HOME");

    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd");
        strcpy(cwd, "?");
    }

    char display[PATH_MAX];
    if (home && strncmp(cwd, home, strlen(home)) == 0) {
        snprintf(display, sizeof(display), "~%s", cwd + strlen(home));
    } else {
        snprintf(display, sizeof(display), "%s", cwd);
    }

    char buf[PATH_MAX + 256];
    snprintf(buf, sizeof(buf), "%s@cosc360:%s> ", user, display);
    fputs(buf, stdout);
    fflush(stdout);
}


void add_job(pid_t pid, const char *cmdline) {
    if (num_jobs >= MAX_JOBS) return;
    jobs[num_jobs].pid    = pid;
    jobs[num_jobs].active = 1;
    strncpy(jobs[num_jobs].cmdline, cmdline,
            sizeof(jobs[num_jobs].cmdline) - 1);
    jobs[num_jobs].cmdline[sizeof(jobs[num_jobs].cmdline) - 1] = '\0';
    num_jobs++;
}

void reap_zombies(void) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < num_jobs; i++) {
            if (jobs[i].active && jobs[i].pid == pid) {
                jobs[i].active = 0;
                break;
            }
        }
    }
}

void builtin_jobs(void) {
    // Count active jobs
    int count = 0;
    for (int i = 0; i < num_jobs; i++) {
        if (jobs[i].active) count++;
    }

    if (count == 1) {
        printf("1 job.\n");
    } else {
        printf("%d jobs.\n", count);
    }

    for (int i = 0; i < num_jobs; i++) {
        if (jobs[i].active) {
            printf("   %-6d - %s\n", jobs[i].pid, jobs[i].cmdline);
        }
    }
}

char *expand_variables(const char *in) {
    size_t len = strlen(in);
    // Allocate a buffer that is safely larger than input.
    // Env expansions can grow; this is a simple over-approximation.
    size_t out_cap = len * 4 + 1;
    char *out = malloc(out_cap);
    if (!out) {
        perror("malloc");
        exit(1);
    }

    size_t i = 0; // index into in
    size_t o = 0; // index into out

    while (in[i] != '\0') {
        if (in[i] == '$') {
            i++;
            if (in[i] == '\0') {
                // Just a trailing '$' → keep it
                if (o + 1 >= out_cap) {
                    out_cap *= 2;
                    out = realloc(out, out_cap);
                    if (!out) { perror("realloc"); exit(1); }
                }
                out[o++] = '$';
                break;
            }

            // Next char must start a variable name: alpha or '_'
            if (!isalpha((unsigned char)in[i]) && in[i] != '_') {
                // Not a valid var start, output '$' literally
                if (o + 1 >= out_cap) {
                    out_cap *= 2;
                    out = realloc(out, out_cap);
                    if (!out) { perror("realloc"); exit(1); }
                }
                out[o++] = '$';
                continue; // re-process current in[i] in next loop iteration
            }

            // Parse variable name
            size_t start = i;
            while (isalpha((unsigned char)in[i]) || in[i] == '_') {
                i++;
            }
            size_t vlen = i - start;
            char name[256];
            if (vlen >= sizeof(name)) vlen = sizeof(name) - 1;
            memcpy(name, &in[start], vlen);
            name[vlen] = '\0';

            char *val = getenv(name);
            if (!val) {
                // Undefined: expansion is empty string
                continue;
            }

            size_t vval = strlen(val);
            if (o + vval >= out_cap) {
                while (o + vval >= out_cap) out_cap *= 2;
                out = realloc(out, out_cap);
                if (!out) { perror("realloc"); exit(1); }
            }
            memcpy(&out[o], val, vval);
            o += vval;
        } else {
            // Normal character
            if (o + 1 >= out_cap) {
                out_cap *= 2;
                out = realloc(out, out_cap);
                if (!out) { perror("realloc"); exit(1); }
            }
            out[o++] = in[i++];
        }
    }

    out[o] = '\0';
    return out;
}

int parseline(char *line,
              char **argv,
              char **infile,
              char **outfile,
              int *append,
              int *background)
{
    *infile     = NULL;
    *outfile    = NULL;
    *append     = 0;
    *background = 0;

    // Strip trailing newline
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }

    int argc = 0;
    char *tok = strtok(line, " \t");

    while (tok != NULL && argc < MAX_ARGS - 1) {

        // Handle input redirection: "<file" or "< file"
        if (tok[0] == '<') {
            const char *name = NULL;
            if (tok[1] != '\0') {
                name = tok + 1;   // "<file"
            } else {
                // "< file"
                tok = strtok(NULL, " \t");
                if (!tok) {
                    fprintf(stderr, "Missing filename after '<'\n");
                    return -1;
                }
                name = tok;
            }
            *infile = expand_variables(name);
            tok = strtok(NULL, " \t");
            continue;
        }

        // Handle append redirection: ">>file" or ">> file"
        if (tok[0] == '>' && tok[1] == '>') {
            const char *name = NULL;
            *append = 1;
            if (tok[2] != '\0') {
                name = tok + 2;   // ">>file"
            } else {
                tok = strtok(NULL, " \t");
                if (!tok) {
                    fprintf(stderr, "Missing filename after '>>'\n");
                    return -1;
                }
                name = tok;
            }
            *outfile = expand_variables(name);
            tok = strtok(NULL, " \t");
            continue;
        }

        // Handle truncate redirection: ">file" or "> file"
        if (tok[0] == '>' && tok[1] != '>') {
            const char *name = NULL;
            *append = 0;
            if (tok[1] != '\0') {
                name = tok + 1;   // ">file"
            } else {
                tok = strtok(NULL, " \t");
                if (!tok) {
                    fprintf(stderr, "Missing filename after '>'\n");
                    return -1;
                }
                name = tok;
            }
            *outfile = expand_variables(name);
            tok = strtok(NULL, " \t");
            continue;
        }

        // Background &
        if (strcmp(tok, "&") == 0) {
            *background = 1;
            tok = strtok(NULL, " \t");
            continue;
        }

        // Normal argument: expand variables in token
        char *expanded = expand_variables(tok);
        argv[argc++] = expanded;

        tok = strtok(NULL, " \t");
    }

    argv[argc] = NULL;
    return argc;
}

/* ---------- Built-ins ---------- */

void do_cd(char **argv) {
    char *target = NULL;

    if (!argv[1] || strcmp(argv[1], "~") == 0) {
        target = getenv("HOME");
        if (!target) {
            fprintf(stderr, "cd: HOME not set\n");
            return;
        }
    } else {
        target = argv[1];
    }

    if (chdir(target) < 0) {
        perror(target);
    }
}

/* ---------- Execution ---------- */

void run_command(char **argv,
                 char *infile,
                 char *outfile,
                 int append,
                 int background,
                 const char *raw_cmdline)
{
    if (argv[0] == NULL) return;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        // Child: apply redirection then exec

        if (infile) {
            int fd = open(infile, O_RDONLY);
            if (fd < 0) {
                perror(infile);
                _exit(1);
            }
            if (dup2(fd, STDIN_FILENO) < 0) {
                perror("dup2");
                _exit(1);
            }
            close(fd);
        }

        if (outfile) {
            int flags = O_WRONLY | O_CREAT;
            if (append) {
                flags |= O_APPEND;
            } else {
                flags |= O_TRUNC;
            }

            int fd = open(outfile, flags, 0644);
            if (fd < 0) {
                perror(outfile);
                _exit(1);
            }
            if (dup2(fd, STDOUT_FILENO) < 0) {
                perror("dup2");
                _exit(1);
            }
            close(fd);
        }

        execvp(argv[0], argv);
        perror(argv[0]);   // print error based on executable name
        _exit(1);
    } else {
        // Parent
        if (background) {
            add_job(pid, raw_cmdline);
        } else {
            int status;
            if (waitpid(pid, &status, 0) < 0) {
                perror("waitpid");
            }
        }
    }
}


int main(void) {
    char line[4096];
    char *argv[MAX_ARGS];
    char *infile = NULL;
    char *outfile = NULL;
    int append = 0;
    int background = 0;

    for (;;) {
        reap_zombies();
        prompt();

        if (!fgets(line, sizeof(line), stdin)) {
            putchar('\n');
            break;  // EOF
        }

        // Save original command line for jobs list
        char raw_cmdline[256];
        strncpy(raw_cmdline, line, sizeof(raw_cmdline) - 1);
        raw_cmdline[sizeof(raw_cmdline) - 1] = '\0';
        size_t rlen = strlen(raw_cmdline);
        if (rlen > 0 && raw_cmdline[rlen - 1] == '\n') {
            raw_cmdline[rlen - 1] = '\0';
        }

        int argc = parseline(line, argv, &infile, &outfile, &append, &background);
        if (argc < 0) {
            // parse error
            continue;
        }
        if (argc == 0) {
            // empty line
            continue;
        }

        // Internal commands handled in parent (no fork)
        if (strcmp(argv[0], "exit") == 0) {
            // free argv entries and infile/outfile before exit
            for (int i = 0; i < argc; i++) free(argv[i]);
            free(infile);
            free(outfile);
            break;  // return from main
        } else if (strcmp(argv[0], "cd") == 0) {
            do_cd(argv);
        } else if (strcmp(argv[0], "jobs") == 0) {
            builtin_jobs();
        } else {
            // External command via fork/exec
            run_command(argv, infile, outfile, append, background, raw_cmdline);
        }

        // Clean up allocated argument strings and redirection paths
        for (int i = 0; i < argc; i++) {
            free(argv[i]);
        }
        if (infile) {
            free(infile);
            infile = NULL;
        }
        if (outfile) {
            free(outfile);
            outfile = NULL;
        }
    }

    return 0;
}
