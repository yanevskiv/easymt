#ifndef _THREAD_H_
#define _THREAD_H_
#include <print.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#define Thread pthread_t
#define TS(x) test_and_set(&(x))
#define FA(x, y) fetch_and_add(&(x), (y))
#define sleep(n) (usleep((n) * 1000))
#define sleep_rand(n, m) (usleep(rand() % (m) + (n)))
pthread_t createThread(void (*func)(void));
pthread_t createThread(void (*func)(int), int);
pthread_t createThread(void (*func)(char*), char*);

void join(pthread_t);
void delay(int mili = 0);
void rand_delay(int max = 0, int min = 0);
void rand_usleep(int max = 0, int min = 0);
void skip();
void yield();

//int test_and_set(volatile int *lock);
//int fetch_and_add(volatile int *x, int val);
//int get_and_set(volatile int *x, int val);
//int cmp_and_swap(volatile int *x, int val);
#define test_and_set(x)     __atomic_test_and_set((x), 0)
#define fetch_and_add(x, v) __atomic_fetch_add((x), (v), 0)
//#define get_and_set(x, v)   __atomic_exchange_n((x), (v), 0)
#define get_and_set(x, v)   __atomic_exchange_n((x), (v), 0)
#define cmp_and_swap(x, y)  __atomic_compare_exchange_n((x), (y))
void work(int ms);
void nanowork(int us);
#define main KDP_main
#endif

