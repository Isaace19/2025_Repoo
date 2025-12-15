#pragma once

#include <stdint.h>
#include <mqueue.h>
#include <semaphore.h>

#define QUEUE_NAME "/factor"
#define SEMAPHORE_NAME "/factor"
#define SHM_NAME "/factor"

int hfactor_process(uint64_t target);
int hfactor_init(mqd_t *q, sem_t **sem, int *shm);
void hfactor_deinit(mqd_t q, sem_t *sem, int shm);
int hfactor_report(const char *reporter, int shm);
int hfactor_calculate(const char *reporter, uint64_t target, uint64_t num_workers);


