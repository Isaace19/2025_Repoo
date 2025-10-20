#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int comparator(const void *lp, const void *rp) {
	 int left = *(const int *)lp;
    int right = *(const int *)rp;

    if(left < right) return 1;
    if(left > right) return -1;
    return 0;

}

const char *ext = ".txt";

int main(int argc, char* argv[]){
	
	printf("Enter filename: ");
	char input[256];
	char filename[512];
	if(scanf("%255s", input) != 1){
		printf("Bad read in of file\n");
		return -1;
	}
	FILE *fl;

	size_t len = strlen(input);
	if (len >=4 && strcmp(input + len -4, ext) == 0){
		snprintf(filename, sizeof(filename), "%s", input);
	}else{
		snprintf(filename, sizeof(filename), "%s%s", input, ext);
	}
	printf("DEBUG: trying to open '%s'\n", filename);


	int fd = open(filename, O_RDONLY);
	if (fd == -1){
		perror(filename);
	}

	fl = fdopen(fd, "r");
	if(!fl){
		perror("Bad file access\n");
		return -1;
	}


	int index = 0;
	double arr[256];
	double temp;

	while(fscanf(fl, "%lf", &temp) == 1){
		arr[index] = temp;
		index++;

		if(index == 0){
			printf("Error, nothing in the index\n");
			return -1;
		}
	}

	// apply qsort onto the array grabbed in descending order
	qsort(arr, index, sizeof(double), comparator);

	for(int i = 0; i < index; i++) {
		printf("%.2f\n", arr[i]);
	}



	fclose(fl);
	return 0;
}

