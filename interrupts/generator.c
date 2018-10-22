#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>


int SIGNALS[4] = {SIGUSR1, SIGUSR2, SIGPROF, SIGCHLD};
int pid = 0;


void handle_interrupt(int sig)
{
	kill(pid, SIGKILL);
	exit(0);
}


int main(int argc, char *argv[])
{
	int rsec, rsig;
	pid = atoi(argv[1]);
	
	signal(SIGINT, handle_interrupt);
	srand((unsigned)time(NULL));
	
	while (1) {
		rsec = rand() % (5-3+1) + 3;
		rsig = rand() % 4;
		printf("Sending %d signal to process with pid %d after %d seconds ...\n", SIGNALS[rsig], pid, rsec);
		sleep(rsec);
		kill(pid, SIGNALS[rsig]);
	}

	return 0;
}
