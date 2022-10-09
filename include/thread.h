#ifndef _THREAD_H_
#define _THREAD_H_
#include <print.h>
#include <pthread.h>
#include <unistd.h>
#define sleep_rand(n, m) (usleep(rand() % (m) + (n)))
#define test_and_set(x)     __atomic_test_and_set((x), 0)
#define fetch_and_add(x, v) __atomic_fetch_add((x), (v), 0)
#define get_and_set(x, v)   __atomic_exchange_n((x), (v), 0)
#define cmp_and_swap(x, y, e)  __atomic_compare_exchange_n((x), (y), (e), 1, 0, 0)
#define TS(x) test_and_set(&(x))
#define FA(x, y) fetch_and_add(&(x), (y))
#define GS(x, y) get_and_set(&(x), (y))
#define CAS(x, y, e) cmp_and_swap(x, y, e)

struct ImplThread;

struct Thread {
    // create an uninitialized thread
    Thread();

    // create a new thread executing a `void f(void)` function
    Thread(void (*func)(void));

    // create a new thread executing a `void f(int)` with a given argument 
    Thread(void (*func)(int), int);

    // create a new thread executing a `void f(const char*)` with a given argument 
    Thread(void (*func)(const char*), const char*);

    // move constructor
    Thread(Thread&&);

    // disallow copying
    Thread(const Thread&) = delete;

    // disallow copying
    Thread& operator=(const Thread&) = delete;

    // move operator
    Thread& operator=(Thread&&);

    // destructor
    ~Thread();

    // is thread runnig something
    bool active() const;

    // join thread
    void join();
private:
    ImplThread *m_impl;
};


// create a new thread executing a `void f(void)` function
Thread createThread(void (*func)(void));

// create a new thread executing a `void f(int)` with a given argument 
Thread createThread(void (*func)(int), int);

// create a new thread executing a `void f(const char*)` with a given argument 
Thread createThread(void (*func)(const char*), const char*);

// join a thread
void join(Thread&);

// delay execution (miliseconds)
void delay(int mili = 0);

// random delay (miliseconds)
void rand_delay(int max = 0, int min = 0);

// random delay (microseconds)
void rand_usleep(int max = 0, int min = 0);

// place thread back into scheduler
void skip();

// yield sxecution
void yield();

// busy wait (miliseconds)
void work(int ms = 1);

// busy wait (microseconds)
void uwork(int us = 1);

#endif

