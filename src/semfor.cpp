#include <semfor.h>

void init(sem_t& sem, int val)
{
    sem_init(&sem, 0, val);
}

void wait(sem_t& sem)
{
    sem_wait(&sem);
}

void signal(sem_t& sem)
{
    sem_post(&sem);
}

void destroy(sem_t& sem)
{
    sem_close(&sem);
}


