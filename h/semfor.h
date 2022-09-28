#ifndef _SEMFOR_H_
#define _SEMFOR_H_
#include <semaphore.h>
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

// wait on semaphore (P(s))
void wait(Sem&);

// signal semaphore (V(s))
void signal(Sem&);

// initialize semaphore
void init(Sem&, int);

// destroy semaphore
void destroy(Sem&);


#endif
