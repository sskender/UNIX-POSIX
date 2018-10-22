#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>


unsigned long num = 1000000001;
unsigned long prime = 1000000001;


/* signal flags */
int flag_wait = 0;
int flag_print = 0;
int flag_end = 0;


int is_prime(unsigned long n)
{
	unsigned long i;
	unsigned long max;

	if ((n & 1) == 0) {
		return 0;
	}

	max = sqrt(n);
	for (i = 3; i <= max; i += 2) {
		if (n % i == 0) {
			return 0;
		}
	}

	return 1;
}


void print_prime(void)
{
	printf("Last prime: %lu\n", prime);
}


/* signal handlers */
void handle_alarm()
{
	flag_print = 1;
}


void handle_pause()
{
	flag_wait = !flag_wait;
}


void handle_end()
{
	flag_end = 1;
}


int main(void)
{
	struct itimerval t;

	signal(SIGALRM, handle_alarm);
	signal(SIGINT, handle_pause);
	signal(SIGTERM, handle_end);

	t.it_value.tv_sec = 0;
	t.it_value.tv_usec = 500000;
	t.it_interval.tv_sec = 0;
	t.it_interval.tv_usec = 500000;
	setitimer(ITIMER_REAL, &t, NULL);

	while (1) {
		/* exit */
		if (flag_end) {
			print_prime();
			exit(0);
		}
		/* print last prime */
		if (flag_print) {
			print_prime();
			flag_print = 0;
		}
		/* calc prime */
		if (is_prime(num)) {
			prime = num;
		}
		/* proceed to next num */
		num++;
		/* pause */
		while (flag_wait) {
			pause();
		}
	}

	return 0;
}
