#include <stdio.h>

int main(int argc, char *argv[]) {
	if (argv < 4) {
		print("Usage: %s <output> <error> <command> [args]\n", argv[0]);
		return 1;
	}

	printf("Hello World\n");
	return 0;
}
