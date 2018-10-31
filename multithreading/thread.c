/*
 * Threads are not synced
 * program is $h1t,
 *
 * DO NOT USE IT
 *
 * It is only an example 
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


long long TOTAL = 0;


void *do_work(void *jobs)
{
	/* do some long work with shared variable */
	for (int i = 0; i < *((int *)jobs); i++) {
		TOTAL++;
	}
	pthread_exit(0);
}


void create_threads(pthread_t *thread_ids, int *threads, int *thread_jobs)
{
	for (int i = 0; i < *threads; i++) {
		if (pthread_create(thread_ids+i, NULL, do_work, thread_jobs) != 0) {
			perror("Error creating new thread\n");
			exit(1);
		}
	}
}


void join_threads(pthread_t *thread_ids, int *threads)
{
	for (int i = 0; i < *threads; i++) {
		pthread_join(*(thread_ids+i), NULL);
	}
}


int main(int argc, char *argv[])
{
	/*
	 * this program creates N threads,
	 * each increases shared int value M times
	 *
	 * ./a.out N M
	 */
	int threads;
	int thread_jobs;
	pthread_t *thread_ids;

	if (argc != 3) {
		perror("Wrong number of arguments\n");
		printf("Usage: %s <N> <M>\n", argv[0]);
		exit(-1);
	}

	threads = atoi(argv[1]);
	thread_jobs = atoi(argv[2]);
	thread_ids = (pthread_t *)malloc(sizeof(pthread_t)*threads);

	create_threads(thread_ids, &threads, &thread_jobs);
	join_threads(thread_ids, &threads);

	printf("Total=%lld\n", TOTAL);
	free(thread_ids);

	return 0;
}

