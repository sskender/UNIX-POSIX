#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>


#define LEVEL 6             /* priority level array length */
#define SIG_COUNT 5         /* total number of signals, also array length */

/*
 * NOTE:
 *  This is random work simulation,
 *  doesn't do anything actually.
 */
#define WORKERS 5           /* number of work to do */
#define WAIT_TIME 1         /* worker's job time (in seconds) */


int CURR_PRIOR = 0;         /* 0 prior is main function, signal interrupts are 1..LEVEL priors */
int PRIORITY[LEVEL] = {0};  /* keeps track of interrupted job (job with lower priority) */
int WAIT[LEVEL] = {0};      /* keeps track of total interrupt signals */

/* arbitrarily signals */
int SIGNALS[SIG_COUNT] = {SIGUSR1, SIGUSR2, SIGPROF, SIGCHLD, SIGINT};


int MAIN_COUNTER = 1;       /* see in main function */


/* <print functions> */

void print_main(int n)
{
	printf("%3d", n);
	for (int i = 0; i < SIG_COUNT; i++) {
		if (i == 0) {
			printf("\t");
		}
		printf(" - ");
	}
	printf("\n");
}


void print_status(int sig_index, char c)
{
	for (int i = 1; i < SIG_COUNT+1; i++) {
		if (i == 1) {
			printf("\t");
		}
		if (i == sig_index) {
			printf(" %c ", c);
		}
		else {
			printf(" - ");
		}
	}
	printf("\n");
}


void print_signal_interrupt(int sig_index)
{
	print_status(sig_index, 'X');
}


void print_job_start(int sig_index)
{
	print_status(sig_index, 'P');
}


void print_job_progress(int sig_index, char c)
{
	print_status(sig_index, c);
}


void print_job_end(int sig_index)
{
	print_status(sig_index, 'K');
}

/* </print functions> */


/* <workers> */

/*
 * NOTE:
 *  This is random work simulation,
 *  doesn't do anything actually.
 */
void do_work(int sig_index)
{
	print_job_start(sig_index);

	for (int i = 0; i < WORKERS; i++) {
		print_job_progress(sig_index, '1'+i);
		sleep(WAIT_TIME);
	}

	print_job_end(sig_index);
}

/* </workers> */


/* <interrupt handlers> */

void allow_interrupts(void)
{
	for (int i = 0; i < SIG_COUNT; i++) {
		sigrelse(SIGNALS[i]);
	}
}


void deny_interrupts(void)
{
	for (int i = 0; i < SIG_COUNT; i++) {
		sighold(SIGNALS[i]);
	}
}


void handle_interrupts(int sig)
{
	int sig_index = -1;
	int new_prior;
	
	/* deny new signal interrupts while processing the current one */
	deny_interrupts();

	/* identify new signal interrupt index */
	switch (sig) {
	case SIGUSR1:
		sig_index = 1;
		print_signal_interrupt(sig_index);
		break;
	case SIGUSR2:
		sig_index = 2;
		print_signal_interrupt(sig_index);
		break;
	case SIGPROF:
		sig_index = 3;
		print_signal_interrupt(sig_index);
		break;
	case SIGCHLD:
		sig_index = 4;
		print_signal_interrupt(sig_index);
		break;
	case SIGINT:
		sig_index = 5;
		print_signal_interrupt(sig_index);
		break;
	default:
		break;
	}

	/* save new signal interrupt index to wait list */
	WAIT[sig_index]++;


	/* find signal with the highest priority */
	do {
		new_prior = 0;  /* reset to main function priority */

		for (int i = LEVEL-1; i > CURR_PRIOR; i--) {
			if (WAIT[i]) {
				new_prior = i;
				break;
			}
		}

		if (new_prior > CURR_PRIOR) {
			WAIT[new_prior]--;                      /* remove from wait list */
			PRIORITY[new_prior] = CURR_PRIOR;       /* save lower priority job, continue that later */
			CURR_PRIOR = new_prior;                 /* change current priority to higher signal priority */

			allow_interrupts();                     /* allow interrupts while job is running */
			do_work(CURR_PRIOR);
			deny_interrupts();

			CURR_PRIOR = PRIORITY[new_prior];       /* continue job with lower priority */
		}

	} while (new_prior > 0);

	/* allow new signal interrupts, processing has finished */
	allow_interrupts();
}

/* </interrupt handlers> */


int main(void)
{
	/* assign signal handler function to each signal */
	for (int i = 0; i < SIG_COUNT; i++) {
		signal(SIGNALS[i], handle_interrupts);
	}

	printf("Main started with PID=%d\n", getpid());

	while (1) {
		if (!CURR_PRIOR) {
			print_main(MAIN_COUNTER++);
			sleep(WAIT_TIME);
		}
	}
	
	printf("Main stopped\n");

	return 0;
}
