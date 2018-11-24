/*
 * Carousel modeled with two types of processes.
 *
 * Parent process represents carousel which can "spin" only when it is full,
 * other processes (children) are visitors who stand in line and try to get on,
 * in case there is an ongoing ride, they have to wait for the next cycle.
 *
 * This is achieved by a counting semaphore.
 *
 * Counting semaphore counts down for each visitor who enters (children processes),
 * and resets to default state (total free seats on carousel) after the ride.
 * (parent process)
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>

#define CAROUSEL_SEATS 5
#define TIME_RIDE 10

const char *semname = "Carousel";
sem_t *sem;


/*
 * Initialize and close counting semaphore
 */
void init_sem(void)
{
        sem = sem_open(semname, O_CREAT | O_EXCL, 0600, CAROUSEL_SEATS);
        if (sem == SEM_FAILED) {
                perror("Semaphore could not be created! Check /dev/shm/ for more info.\n");
                exit(1);
        }
        /* mark the semaphore to be destroyed once all processes cease using it */
        sem_unlink(semname);
}


void clear_sem(void)
{
        sem_close(sem);
}


/*
 * Handle interrupt signal
 */
void handle_interrupt()
{
        clear_sem();
        exit(0);
}


void carousel(void)
{
        /*
         * This runs in parent process,
         * constantly checking if the carousel if full and ride can be started.
         *
         * Ride takes TIME_RIDE number of seconds,
         * and resets semaphore afterwards.
         * (calls sem_post n times where n is number of seats
         */
        int i, sval;

        while (1) {
                sem_getvalue(sem, &sval);
                if (sval == 0) {
                        printf("[!] carousel if full, ride starting ...\n");
                        sleep(TIME_RIDE);
                        printf("[!] carousel ride ended\n");
                        for (i = 0; i < CAROUSEL_SEATS; i++) {
                                sem_post(sem);
                        }
                }
        }
}


void visitor(void)
{
        /*
         * Wait if semaphore value is zero (ride is in progress),
         * otherwise decrement the value (visitor has taken a seat)
         */
        printf("[?] %d spawned and is trying to take a seat\n", getpid());
        sem_wait(sem);
        printf("[+] %d has taken a seat\n", getpid());
}


int main(int argc, char **args)
{
        int i;
        pid_t pid;

        if (argc < 2) {
                printf("Usage: %s <number of visitors>\n", args[0]);
                exit(0);
        }

        signal(SIGINT, handle_interrupt);
        init_sem();

        for (i = 0; i < atoi(args[1]); i++) {
                pid = fork();

                if (pid < 0) {
                        /* fork error */
                        perror("Fork failed!\n");
                        break;
                } else if (pid == 0) {
                        /* child */
                        visitor();
                        exit(0);
                } else {
                        /* parent */
                }
        }

        carousel();

        clear_sem();

        return 0;
}

