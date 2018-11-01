/*
 * Threads synced with Lamport's bakery algorithm
 *
 * Still not perfect
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef long long ll;


/* this must be calculated */
ll TOTAL = 0;

/* thread related data */
int THREADS;
pthread_t *THREAD_IDS;

struct do_work_arg {
	int thread_index;	/* this is not id, this is index to keep track num. of total THREADS */
	ll *thread_jobs;	/* thread_jobs in main, no need to copy value, pointer will do */
}; 

/* Lamport data */
int *ENTERING;
int *NUMBER;


int find_max(int *arr, int size)
{
	/* find maximum in Lamport NUMBER array */
	int max = arr[0];
	for (int i = 0; i < size; i++) {
		if (arr[i] > max) {
			max = arr[i];
		}
	}
	return max;
}


void lamport_lock(int i)
{
	/* i = thread index from THREADS */

	ENTERING[i] = 1;
	NUMBER[i] = 1 + find_max(NUMBER, THREADS);
	ENTERING[i] = 0;

	for (int j = 0; j < THREADS; j++) {
		/*
		 * wait until thread j receives its number
		 */
		while (ENTERING[j]);
		/*
		 * wait unitl all threads with smaller numbes or with the same number,
		 * but with higher priority,
		 * finish their work
		 */
		while ((NUMBER[j] != 0) && ((NUMBER[j] < NUMBER[i]) || ((NUMBER[j] == NUMBER[i]) && (j < i))));
	}
}


void lamport_unlock(int i)
{
	/* i = thread index from THREADS */

	NUMBER[i] = 0;
}


void *do_work(void *arg)
{
	/*
	 * do some long work with shared variable
	 *
	 * extract data from arg pointer
	 */
	struct do_work_arg *arg_struct = (struct do_work_arg *)arg;

	int thr_indx = arg_struct->thread_index;
	ll *jobs_ptr = arg_struct->thread_jobs;
	ll jobs = *jobs_ptr;

	/*
	 * do long work,
	 * keep track of thread index
	 * use Lamport's bakery algorithm
	 */
	for (ll i = 0; i < jobs; i++) {
		lamport_lock(thr_indx);
		/* start of critical section */
		TOTAL++;
		/* end of critical section */
		lamport_unlock(thr_indx);
	}

	pthread_exit(0);
}


int main(int argc, char *argv[])
{
	/*
	 * this program creates N threads,
	 * each increases shared int value M times
	 *
	 * ./a.out N M
	 */

	struct do_work_arg *arg_array;  /* save thread function arguments (save thread index for Lamport) */
	ll thread_jobs;			/* this is M from arguments */

	if (argc != 3) {
		perror("Wrong number of arguments\n");
		printf("Usage: %s <N> <M>\n", argv[0]);
		exit(-1);
	}

	/* load N and M values from arguments */
	THREADS = atoi(argv[1]);
	thread_jobs = atoll(argv[2]);

	/* allocate memory for thread_ids and Lamport arrays */
	THREAD_IDS = (pthread_t *)malloc(sizeof(pthread_t)*THREADS);
	ENTERING = (int *)calloc(THREADS, sizeof(int));
	NUMBER = (int *)calloc(THREADS, sizeof(int));
	arg_array = (struct do_work_arg *)malloc(sizeof(struct do_work_arg)*THREADS);

	/*
	 * start threads
	 *
	 * THREAD_IDS 	-> save thread id
	 * do_work	-> function to execute by thread
	 * arg_array	-> arguments for thread function (also save thread index (i))
	 */
	for (int i = 0; i < THREADS; i++) {
		arg_array[i].thread_index = i;
		arg_array[i].thread_jobs = &thread_jobs;  /* this is stored in main */

		if (pthread_create(THREAD_IDS+i, NULL, do_work, &arg_array[i]) != 0) {
			perror("Error creating new thread\n");
			exit(-1);
		}
	}

	/*
	 * join threads
	 *
	 * grab thread id from THREAD_IDS array
	 */
	for (int i = 0; i < THREADS; i++) {
		pthread_join(*(THREAD_IDS+i), NULL);
	}

	/* all threads finished */
	printf("Total=%lld\n", TOTAL);

	free(NUMBER);
	free(ENTERING);
	free(THREAD_IDS);

	return 0;
}

