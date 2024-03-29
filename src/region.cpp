// (c) Ivan Janevski
#include <region.h>
#include <pthread.h>

// region implementation
struct ImplRegion {
    // constructor
    ImplRegion();

    // destructor
    ~ImplRegion();

    // wait for condition to become true
    void wait();

    // enter region
    void lock();

    // exit region
    void unlock();
private:
    // mutex
    pthread_mutex_t m_mutex;

    // condition
    pthread_cond_t m_cond;
};

// create region struct
ImplRegion::ImplRegion() 
{
    pthread_mutex_init(&m_mutex, 0);
    pthread_cond_init(&m_cond, 0);
}

// destroy region struct
ImplRegion::~ImplRegion()
{
    pthread_cond_destroy(&m_cond);
    pthread_mutex_destroy(&m_mutex);
}

// enter region
void ImplRegion::lock()
{
    pthread_mutex_lock(&m_mutex);
}

// exit region
void ImplRegion::unlock()
{
    pthread_cond_broadcast(&m_cond);
    pthread_mutex_unlock(&m_mutex);
}

// wait on condition
void ImplRegion::wait() 
{
    pthread_cond_broadcast(&m_cond);
    pthread_cond_wait(&m_cond, &m_mutex);
}

/* 
 * Region
 */
Region::Region() 
    : m_impl(new ImplRegion)
{
    // nothing
}

Region::~Region()
{
    delete m_impl;
}

void Region::lock() 
{
    m_impl->lock();
}

void Region::unlock()
{
    m_impl->unlock();
}

void Region::wait()
{
    m_impl->wait();
}
