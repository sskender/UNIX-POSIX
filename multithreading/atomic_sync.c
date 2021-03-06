/*
 * Threads synced with functions for Memory Model Aware Atomic Operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef long long ll;


/* atomic test and set flag */
char sync_flag = 0;

/* this must be calculated */
ll TOTAL = 0;

/* thread function arg */
struct thread_arg {
	ll jobs;
};


void lock(void)
{
	while (__atomic_test_and_set(&sync_flag, __ATOMIC_SEQ_CST) != 0);
}


void unlock(void)
{
	sync_flag = 0;
}


void *do_work(void *arg)
{
	/*
	 * do some long work with shared variable
	 *
	 * extract data from arg pointer
	 */
	struct thread_arg *t_arg = (struct thread_arg *)arg;
	ll jobs = t_arg->jobs;

	for (ll i = 0; i < jobs; i++) {
		lock();
		/* start of critical section */
		TOTAL++;
		/* end of critical section */
		unlock();
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
	int t_num;
	pthread_t *t_ids;
	struct thread_arg t_arg;

	if (argc != 3) {
		perror("Wrong number of arguments\n");
		printf("Usage: %s <N> <M>\n", argv[0]);
		exit(-1);
	}

	/* load N and M values from arguments */
	t_num = atoi(argv[1]);
	t_arg.jobs = atoll(argv[2]);

	/* allocate memory for thread ids */
	t_ids = (pthread_t *)malloc(sizeof(pthread_t)*t_num);

	/* start threads */
	for (int i = 0; i < t_num; i++) {
		if (pthread_create(t_ids+i, NULL, do_work, &t_arg) != 0) {
			perror("Error creating new thread\n");
			exit(-1);
		}
	}

	/*
	 * join threads
	 *
	 * grab thread id from thread ids array
	 */
	for (int i = 0; i < t_num; i++) {
		pthread_join(*(t_ids+i), NULL);
	}

	/* all threads finished */
	printf("Total=%lld\n", TOTAL);

	free(t_ids);

	return 0;
}

