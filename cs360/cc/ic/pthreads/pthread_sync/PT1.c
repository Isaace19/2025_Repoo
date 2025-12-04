#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <pthread.h>

// basic linked list structure

struct List {
	int x;
	int y;
	int z;
	struct List *next;
};

// need to review mutex and race conditions. 

struct Data {
	int result;
	struct List *work_list;
	pthread_mutex_t lock;
};

void *woker(void *arg) {
	struct Data *data = (struct Data *)arg;
	struct List *t;

	pthread_mutex_lock(&data->lock);
	t = data->work_list;
	data->work_list = t->next;
	data->result += t-> x *t->y + t->z;
	pthread_mutex_unlock(&data->lock);
	free(t);

	return NULL;
}


