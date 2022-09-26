#include <concur.h>
#include <thread.h>
#include <unistd.h>
#include <sys/time.h>
#define tvms(tv) ((tv).tv_sec * 1000 + (tv).tv_usec / 1000)
#define tvus(tv) ((tv).tv_sec * 1000 * 1000 + (tv).tv_usec)

static void *wrapper(void* void_func)
{
    void (*func)(void) = (void (*)(void)) void_func;
    func();
    return 0;
}

pthread_t createThread(void (*func)(void))
{
    pthread_t thread;
    pthread_create(&thread, 0, wrapper, (void*)func);
    return thread;
}

struct struct_id_t {
    void (*m_func)(int);
    int m_id;
};

static void *wrapper_id(void *data)
{
    struct_id_t *struct_id = (struct_id_t*) data;
    struct_id->m_func(struct_id->m_id);
    free(struct_id);
    return 0;
}

pthread_t createThread(void (*func)(int), int id)
{
    struct_id_t *struct_id = (struct_id_t*)calloc(1, sizeof(struct_id_t));
    struct_id->m_func  = func;
    struct_id->m_id = id;
    assert(struct_id != 0);

    pthread_t thread;
    int creation = pthread_create(&thread, 0, wrapper_id, (void*)struct_id);
    assert(creation == 0);
    return thread;
}

void join(pthread_t thread)
{
    pthread_join(thread, 0);
}

void skip()
{
    usleep(0);
}

void yield()
{
    pthread_yield();
}

void work(int ms) 
{
    struct timeval tv_init, tv_next;
    gettimeofday(&tv_init, 0);
    do {
        gettimeofday(&tv_next, 0);
        if (tvms(tv_next) - tvms(tv_init) >= ms)
            break;
    } while (true);
}

void nanowork(int us) 
{
    struct timeval tv_init, tv_next;
    gettimeofday(&tv_init, 0);
    do {
        gettimeofday(&tv_next, 0);
        if (tvus(tv_next) - tvus(tv_init) >= us)
            break;
    } while (true);
}

void delay(int mili)
{
    if (mili >= 0) {
        usleep(1000 * mili);
    }
}

void rand_delay(int max, int min)
{
    if (max < 0) {
        delay(max);
        return;
    } else if (min < 0 || min == max) {
        delay(max);
        return;
    }
    if (min > max) {
        int tmp = min;
        min = max;
        max = tmp;
    }
    int milis = rand() % (max - min) + min;
    if (milis >= 0)
        delay(milis);
}

void rand_usleep(int max, int min)
{
    max = (max < 0) ? 0 : max;
    min = (min < 0) ? 0 : min;
    if (min == max) {
        usleep(max);
    } else {
        if (min > max) {
            int tmp = min;
            min = max;
            max = tmp;
        }
        int us = rand() % (max - min) + min;
        if (us >= 0)
            usleep(us);
    }
}
