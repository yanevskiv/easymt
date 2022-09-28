#include <concur.h>
#include <thread.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#define tvms(tv) ((tv).tv_sec * 1000 + (tv).tv_usec / 1000)
#define tvus(tv) ((tv).tv_sec * 1000 * 1000 + (tv).tv_usec)

// `Thread` implementation
struct ImplThread {
    ImplThread();
    ImplThread(void (*)(void));
    template <typename T> ImplThread(void (*)(T), const T&);
    ~ImplThread();
    void join();
    bool active() const;
private:
    pthread_t m_pthread;
    bool m_active;
};

// wrapper function used for `void f(void)` threads
static void *wrapper_void_func(void* void_func) try
{
    void (*func)(void) = (void (*)(void)) void_func;
    try {
        func();
    } catch (const char *msg) {
        println(RED " !!! " NONE "Error: %s", msg);
    }

    return nullptr;
} catch (const char *msg) {
    println(RED " !!! " NONE "Error: %s", msg);
    return nullptr;
}

// generic data 
template <typename T> struct WrapperData {
    void (*m_func)(T);
    T m_data;
    WrapperData(void (*func)(T), const T& data)
        : m_func(func), m_data(data)
    {
        // nothing
    }
};

// generic pthread wrapper function
template <typename T> void *wrapper_data_func(void *data) 
{
    WrapperData<T>* wrapperData = static_cast<WrapperData<T>*>(data);
    if (wrapperData) {
        try {
            wrapperData->m_func(wrapperData->m_data);
        } catch (const char *msg) {
            println(RED " !!! " NONE "Error: %s", msg);
        }
    }
    delete wrapperData;
    return nullptr;
} 

ImplThread::ImplThread()
    : m_active(false)
{
}

ImplThread::ImplThread(void (*func)(void))
    : m_active(true)
{
    pthread_create(
        &m_pthread, 
        NULL, 
        wrapper_void_func, 
        (void*)func
    );
}

template <typename T> ImplThread::ImplThread(void (*func)(T), const T& data)
    : m_active(true)
{
    pthread_create(
        &m_pthread, 
        NULL, 
        wrapper_data_func<T>, 
        new WrapperData<T>(func, data)
    );
}

ImplThread::~ImplThread() {

}

// join thread
void ImplThread::join()
{
    if (m_active)  
        pthread_join(m_pthread, NULL);
}

// check thread is executing something
bool ImplThread::active() const
{
    return m_active;
}


Thread::Thread()
    : m_impl (new ImplThread())
{
    // nothing
}

Thread::Thread(void (*func)(void))
    : m_impl (new ImplThread(func))
{
    // nothing
}

Thread::Thread(void (*func)(int), int id)
    : m_impl (new ImplThread(func, id))
{
    // nothing
}

Thread::Thread(void (*func)(const char*), const char *name)
    : m_impl (new ImplThread(func, name))
{
    // nothing
}

Thread::Thread(Thread&& th)
{
    if (&th != this) {
        delete this->m_impl;
        this->m_impl = th.m_impl;
    }
}

Thread& Thread::operator=(Thread&& th) 
{
    if (&th != this) {
        delete this->m_impl;
        this->m_impl = th.m_impl;
    }
    return *this;
}

Thread::~Thread() 
{
    // nothing
}

void Thread::join()
{
    m_impl->join();
}

bool Thread::active() const
{
    return m_impl->active();
}


// create a new thread
Thread createThread(void (*func)(void))
{
    return Thread(func);
}

// createa a new thread with id
Thread createThread(void (*func)(int), int id)
{
    return Thread(func, id);
}

// create a new thread with name
Thread createThread(void (*func)(const char*), const char* name)
{
    return Thread(func, name);
}

// join thread
void join(Thread& thread)
{
    thread.join();
}

// yield execution
void skip()
{
    usleep(0);
}

// yield execution no delay
void yield()
{
    pthread_yield();
}

// busy wait (miliseconds)
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

// busy wait (microseconds);
void uwork(int us) 
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
