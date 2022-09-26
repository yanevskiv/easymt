#ifndef _SEMFOR_H_
#define _SEMFOR_H_
#include <semaphore.h>
//#define Sem sem_t
struct ImplSem;
struct Sem {
    Sem();
    Sem(int val);
    Sem(const Sem&) = delete;
    Sem(Sem&&);
    Sem& operator=(const Sem&) = delete;
    Sem& operator=(Sem&&);
    ~Sem();
    void init(int);
    void wait();
    void signal();
    void destroy();
private:
    ImplSem *m_impl;
};

void wait(Sem&);
void signal(Sem&);
void inits(Sem&, int);
void destroy(Sem&);


#endif
