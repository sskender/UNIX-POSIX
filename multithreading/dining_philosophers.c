/**
 * Dinning philosophers problem, but slightly altered.
 *
 * N philosophers, where each one can have different number of hands.
 * To eat, they need to have chopsticks in all hands.
 *
 * There is M chopsticks in the middle of the table.
 *
 * Configuration file:
 * In the first line M number of chopsticks and N number of philosophers,
 * in next N lines number of hands for each philosopher.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* how many times philosopher needs to get chopsticks in order to finish the meal */
#define CYCLE_LENGTH 3


/* sync mechanism */
pthread_mutex_t MUTEX;
pthread_cond_t CONDITION;


/* variable to protect: number of chopsticks available */
int CHOPSTICKS;

/* philosopher structure */
struct philosopher {
        int ID;
        int num_of_hands;
};

typedef struct philosopher Philosopher;

/* all philosophers are stored in this array */
Philosopher *PHILOSOPHERS;
int PHILOSOPHERS_NUM;


void take_chopsticks(int n);
void release_chopsticks(int n);
void *philosophers_life(void *arg);


int main(int argc, char **args)
{
        FILE *filep;
        int i = 0;
        pthread_t *ID_t;

        if (argc != 2) {
                printf("Usage: %s <file>\n", args[0]);
                exit(0);
        }

        if ((filep = fopen(args[1], "r")) == NULL) {
                perror("Can't open file!\n");
                exit(1);
        }

        if (fscanf(filep, "%d %d", &CHOPSTICKS, &PHILOSOPHERS_NUM) != 2) {
                perror("Invalid number of chopsticks and philosophers in configuration file!\n");
                exit(1);
        }

        PHILOSOPHERS = (Philosopher *)malloc(sizeof(Philosopher)*PHILOSOPHERS_NUM);
        if (PHILOSOPHERS == NULL) {
                perror("Out of memory!\n");
                exit(1);
        }

        while (i < PHILOSOPHERS_NUM) {
                PHILOSOPHERS[i].ID = i+1;
                if (fscanf(filep, "%d", &(PHILOSOPHERS[i].num_of_hands)) != 1) {
                        perror("Invalid number of hands in file!\n");
                        exit(1);
                }
                i++;
        }

        fclose(filep);


        ID_t = (pthread_t *)malloc(sizeof(pthread_t)*PHILOSOPHERS_NUM);
        for (i = 0; i < PHILOSOPHERS_NUM; i++) {
                if (pthread_create(ID_t+i, NULL, philosophers_life, PHILOSOPHERS+i) != 0) {
                        perror("Error creating new thread!\n");
                        exit(1);
                }
        }

        for (i = 0; i < PHILOSOPHERS_NUM; i++) {
                pthread_join(*(ID_t+i), NULL);
        }


        free(ID_t);
        free(PHILOSOPHERS);

        return 0;
}


/**
 * Take chopsticks from the middle of the table.
 *
 * Pass the number of hands.
 */
void take_chopsticks(int n)
{
        pthread_mutex_lock(&MUTEX);

        while (CHOPSTICKS < n) {
                pthread_cond_wait(&CONDITION, &MUTEX);
        }

        CHOPSTICKS = CHOPSTICKS - n;

        pthread_mutex_unlock(&MUTEX);
}


/**
 * Release chopsticks, put them back in the middle of the table.
 *
 * Pass the number of hands.
 */
void release_chopsticks(int n)
{
        pthread_mutex_lock(&MUTEX);

        CHOPSTICKS = CHOPSTICKS + n;
        pthread_cond_broadcast(&CONDITION);

        pthread_mutex_unlock(&MUTEX);
}


/**
 * Philosopher's life consists of eating and thinking.
 *
 * Pass Philosopher structure to extract ID and number of hands.
 * (in order to know how many chopsticks are required)
 *
 * NOTE:
 * This is a thread
 */
void *philosophers_life(void *arg)
{
        int cycle = 1;
        Philosopher philosopher = *((Philosopher *)arg);

        while (cycle <= CYCLE_LENGTH) {
                take_chopsticks(philosopher.num_of_hands);
                printf("[%d][+] Philosopher %d took %d chopsticks, eating ...\n", cycle, philosopher.ID, philosopher.num_of_hands);
                sleep(3);

                release_chopsticks(philosopher.num_of_hands);
                printf("[%d][-] Philosopher %d left %d chopsticks, thinking ...\n", cycle, philosopher.ID, philosopher.num_of_hands);

                cycle++;
        }

        pthread_exit(0);
}

