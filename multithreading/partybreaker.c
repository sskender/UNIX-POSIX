#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

pthread_mutex_t MUTEX;
pthread_cond_t CONDITION;

int STUDENTS_AT_PARTY = 0;
int PARTYBREAKER_IN_ROOM = 0;

int TOTAL_STUDENTS_THREADS = 15;
int STUDENTS_I[15];

void *student(void *arg)
{
        int entry = 1;
        float time_to_sleep = (float)(((rand() % (500 - 100)) + 100) / 100);

        int k = *((int *)arg);
        float party_time, rest_time;

        printf("[+] Student created%d\n", k);
        sleep(time_to_sleep);

        while (entry <= 3)
        {
                pthread_mutex_lock(&MUTEX);
                while (PARTYBREAKER_IN_ROOM)
                {
                        pthread_cond_wait(&CONDITION, &MUTEX);
                }
                STUDENTS_AT_PARTY++;
                pthread_cond_broadcast(&CONDITION);
                pthread_mutex_unlock(&MUTEX);

                printf("[+] Student %d entered a party\n", k);

                // party time
                party_time = (float)(((rand() % (2000 - 1000)) + 1000) / 100);
                sleep(party_time);

                // leave party
                pthread_mutex_lock(&MUTEX);
                STUDENTS_AT_PARTY--;
                pthread_cond_broadcast(&CONDITION);
                pthread_mutex_unlock(&MUTEX);

                printf("[-] Student %d left party\n", k);

                // rest time
                rest_time = (float)(((rand() % (2000 - 1000)) + 1000) / 100);
                sleep(rest_time);

                entry++;
        }

        free(arg);
        pthread_exit(0);
}

void *partibrejker(void *arg)
{
        float time_to_sleep;

        while (1)
        {
                // wait to enter a party
                time_to_sleep = (float)(((rand() % (1000 - 100)) + 100) / 100);
                sleep(time_to_sleep);

                // enter party
                pthread_mutex_lock(&MUTEX);
                while (STUDENTS_AT_PARTY < 3)
                {
                        pthread_cond_wait(&CONDITION, &MUTEX);
                }
                PARTYBREAKER_IN_ROOM = 1;
                pthread_cond_broadcast(&CONDITION);
                pthread_mutex_unlock(&MUTEX);

                printf("[!] Partybreaker entered a party\n");

                // everyone left, leave party now
                pthread_mutex_lock(&MUTEX);
                while (STUDENTS_AT_PARTY > 0)
                {
                        pthread_cond_wait(&CONDITION, &MUTEX);
                }
                PARTYBREAKER_IN_ROOM = 0;
                pthread_mutex_unlock(&MUTEX);

                printf("[x] Partybreaker left party\n");
        }
}

int main(void)
{
        int i;

        pthread_t *students_t;
        pthread_t *partybreaker_t;

        srand((unsigned)time(NULL));

        // create partybreaker thread
        pthread_create(partybreaker_t, NULL, partibrejker, NULL);

        // create students threads
        students_t = (pthread_t *)malloc(sizeof(pthread_t) * TOTAL_STUDENTS_THREADS);
        for (i = 0; i < TOTAL_STUDENTS_THREADS; i++)
        {
                STUDENTS_I[i] = i + 1;
                pthread_create(students_t + i, NULL, student, STUDENTS_I + i);
        }

        // join
        pthread_join(*partybreaker_t, NULL);
        for (i = 0; i < TOTAL_STUDENTS_THREADS; i++)
        {
                pthread_join(*(students_t + i), NULL);
        }

        free(students_t);
        free(partybreaker_t);

        return 0;
}
