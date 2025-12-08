#include <stdio.h>
#include <string.h>

struct Config {
    char name[32];
    int id;
    float score;
};

int main() {
    struct Config cfg;
    char line[256];

    printf("Enter name, id, score: ");
    if (!fgets(line, sizeof(line), stdin)) {
        perror("fgets");
        return 1;
    }

    // Remove trailing newline (if present)
    line[strcspn(line, "\n")] = '\0';

    // Parse the line
    if (sscanf(line, "%31s %d %f", cfg.name, &cfg.id, &cfg.score) != 3) {
        fprintf(stderr, "Invalid input format!\n");
        return 1;
    }

    printf("Parsed: name='%s', id=%d, score=%.1f\n", cfg.name, cfg.id, cfg.score);
    return 0;
}