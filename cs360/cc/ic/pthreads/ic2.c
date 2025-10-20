#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <pthread.h>

static void *worker(void *arg);

struct List {
	int value;
	struct List *next;
};

struct Data {
	pthread_cond_t cond;
	pthread_mutex_t lock;
	int die;
};

int main() {	
	struct Data data;
	int i;
	pthreads = tids[2];
	pthread_cond_init(&data.cond, NULL);
	pthread_mutex_init(&data.lock, NULL);

	data.die = 0;

	work = malloc(sizeof(struct List));
	work_value = 10;
	work->next = NULL;
	data.work = work;

	/*
	 * wanting address of value use & 
	 * wanting value of address use *
	 * */

	for (i = 0; i < 2; i++) {
		pthread_create(tids + i, NULL, worker, &data);
	}

	// whenever creating pthreads, you need to join them
	for (i = 0; i < 2; i++) {
		pthread_join(tids[i], NULL);
	}

	pthread_mutex_destroy(&data.cond);
	pthread_mutex_destroy(&data.lock);

}

static void *worker(void *arg) {
	struct Data *data = (struct Data *)arg;
	for (;;) {
		while (!data->die){
			pthread_mutex_lock(&data->lock);
			pthread_mutex_wait(&data->cond, &data->lock);
		}

		/*
		 * locked-> wait ->unlocked -> waiting for sig. -> locked
		 *
		 * */

		/*
		 * practice these threads in pc.c in making pthreads
		 * and mutex expressions
		 *
		 * */

		if(data->die) {
			// put into waiting, and unlocks the mutex
			pthread_mutex_unlock(&data->lock);
			break;
		}
		// once break out of this the thread is locked. 
		// This is called deadlock -> it is no longer possible to unlock this again
		// uncapturable signal, program will not terminate.

		// signals out of order
		pthread_mutex_unlock(&data->lock);
	}
}
