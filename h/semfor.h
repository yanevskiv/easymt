#ifndef _SEMFOR_H_
#define _SEMFOR_H_
#include <semaphore.h>
struct ImplSem;
struct Sem {
    // create uninitialized semaphore
    Sem();

    // create initialized semaphore with value `val`
    Sem(int val);

    // disallow copying
    Sem(const Sem&) = delete;

    // move constructor
    Sem(Sem&&);

    // disallow copy operator
    Sem& operator=(const Sem&) = delete;

    // move operator
    Sem& operator=(Sem&&);

    // deinitialize and destroy semaphore
    ~Sem();

    // initialize semaphore if uninitialized
    void init(int);

    // wait semaphore
    void wait();
    
    // signal semaphore
    void signal();

    // deinitialize semaphore
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
