#ifndef _SEMFOR_H_
#define _SEMFOR_H_
#include <semaphore.h>
#define Sem sem_t
void wait(sem_t&);
void signal(sem_t&);
void init(sem_t&, int);
void destroy(sem_t&);


#endif
