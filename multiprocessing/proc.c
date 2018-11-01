#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>


int SEGMENT_ID;
int *SHARED_VAR;


void init_mem(void)
{
	/*
	 * allocate shared memory segment
	 *
	 * key  => shared memory segment of size equal to at least the value of size bytes
	 * size => sizeof(int)
	 * shmflag => 600 = owner read+write
	 */
	SEGMENT_ID = shmget(IPC_PRIVATE, sizeof(int), 0600);
	
	if (SEGMENT_ID == -1) {
		perror("Shared memory segment could not be created!\n");
		exit(1);
	}

	/*
	 * shared memory attach
	 *
	 * shmaddr => attaching address, if NULL system chooses unused address
	 */
	SHARED_VAR = (int *)shmat(SEGMENT_ID, NULL, 0);
}


void clear_mem(void)
{
	/* shared memory detach */
	(void)shmdt(SHARED_VAR);

	/*
	 * shared memory control
	 *
	 * int cmd => mark the segment to be destroyed
	 * struct shmid_ds *buf => NULL
	 */
	(void)shmctl(SEGMENT_ID, IPC_RMID, NULL);
}


void handle_interrupt(int sig)
{
	/* on sigterm clear shared memory before exit */
	printf("%d signal received\n", sig);
	clear_mem();
	exit(0);
}


void do_work(int n)
{
	/* do some long work with variable in shared memory */
	for (int i = 0; i < n; i++) {

		/* start of critical section */
		(*SHARED_VAR)++;
		/* end of critical section */

		if ((i & 1) == 0) {
			sleep(rand() % 2);
		}
	}
}


int main(int argc, char *argv[])
{
	/*
	 * this program forks N processes,
	 * each increases shared int value M times
	 *
	 * ./a.out N M
	 */
	int forks;
	int fork_jobs;
	pid_t child_pid;
	int child_exst;

	if (argc != 3) {
		perror("Wrong number of arguments\n");
		printf("Usage: %s <N> <M>\n", argv[0]);
		exit(-1);
	}

	/* load N and M values from arguments */
	forks = atoi(argv[1]);
	fork_jobs = atoi(argv[2]);

	/* set signal handler and shared memory */
	srand(getpid());
	init_mem();
	signal(SIGINT, handle_interrupt);

	/* set value to be calculated  */
	*SHARED_VAR = 0;

	/* fork */
	for (int i = 0; i < forks; i++) {
		child_pid = fork();

		if (child_pid < 0) {
			/* fork error */
			perror("Fork failed\n");
		} else if (child_pid == 0) {
			/* this is child */
			do_work(fork_jobs);
			exit(0);
		} else {
			/* this is parent */
			printf("Parent pid=%d\tChild pid=%d\n", getpid(), child_pid);
		}
	}

	for (int i = 0; i < forks; i++) {
		wait(&child_exst);
		/* use WEXITSTATUS to extract child process exit status */
	}

	/*
	 * End
	 *
	 * clear memory before exit
	 * otherwise, it stays in kernel even after program exits
	 */
	printf("Total=%d\n", *SHARED_VAR);
	clear_mem();

	return 0;
}

