/*
 * Name: Isaac Rafanan Abella
 * Student ID: 000641840
 * Email: iabella@vols.utk.edu
 *
 * Collaboration: None
*/

#include <stdio.h>
#include <math.h>

#define MAX_LENGTH 32
#define BUFF_SIZE 256

struct Point {
	int x;
	int y;
};
double distance (const struct Point *p1, const struct Point *p2);

int main (int argc, char *argv[]) {
  // take in 4 command line arguments

	double initial_blast, attenuation;
   	int x, y;	   

	// standardized list of people with no assumptions comes in as <x> <y> <string>
	// string is max of 32 chars so no need to guess a buffer size. 
	// insert an error check with argc to argv
	
	if (argc != 5) {
		fprintf(stderr, "Usage: Not enough command line arguments for the program to run.\n");
		return 1;
	}

	// error check line by line

	if (sscanf(argv[1], "%lf", &initial_blast) != 1) {
		fprintf(stderr, "Error: Invalid Blast Strength.\n");
		return 1;
	}	
	
	if (sscanf(argv[2], "%lf", &attenuation) != 1) {
		fprintf(stderr, "Error: Invalid Attenuation Factor.\n");
		return 1;
	}

	if (sscanf(argv[3], "%d", &x) != 1) {
		fprintf(stderr, "Error: Invalid X Coordinate.\n");
		return 1;
	}

	if (sscanf(argv[4], "%d", &y) != 1) {
		fprintf(stderr, "Error: Invalid Y Coordinate.\n");
		return 1;
	}

	// set the initial blast point. This is used against every other attenuation dose calculation
	struct Point blast = {x, y}; 

	// now process in the standard input file line by line using scanf
	
	int file_x, file_y;
	// length of char buff + 1 accounts for null terminator
	char name_buff[MAX_LENGTH + 1];

	while (scanf("%d %d %32s", &file_x, &file_y, name_buff) == 3) {
		// create an object for the people being calculated on. 

		struct Point person = {file_x, file_y};
		double distance_calculated = distance(&blast, &person);

		double dosage_received = initial_blast * pow (attenuation, distance_calculated);

		// formatted output according to lab writeup
		printf("%-8s: %8.3f Sv\n", name_buff, dosage_received);
	}
	return 0;
}

double distance (const struct Point *p1, const struct Point *p2) {
	double dist_calc = sqrt(pow(p2->x - p1->x, 2) + pow(p2->y - p1->y, 2));
	return dist_calc;
}
