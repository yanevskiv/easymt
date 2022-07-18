#ifndef _REGION_H_
#define _REGION_H_
#include <thread.h>
#include <semfor.h>
struct Region {
    Region() {
        pthread_mutex_init(&m_mutex, 0);
        pthread_cond_init(&m_cond, 0);
    }
    virtual ~Region() {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }
    void lock() {
        pthread_mutex_lock(&m_mutex);
    }
    void unlock() {
        pthread_cond_broadcast(&m_cond);
        pthread_mutex_unlock(&m_mutex);
    }
    void wait() {
        pthread_cond_broadcast(&m_cond);
        pthread_cond_wait(&m_cond, &m_mutex);
    }
    mutable pthread_mutex_t m_mutex;
    mutable pthread_cond_t m_cond;
};

struct REGION {
    Region& m_region;
    REGION(Region& r) : m_region(r) {
        m_region.lock();
    }
    ~REGION() {
        m_region.unlock();
    }
    void wait() {
        m_region.wait();
    }
    bool once() {
        if (m_once) {
            m_once = false;
            return true;
        }
        return false;
    }
    bool m_once = true;
};

#define region(r) \
    for (REGION _region(r);_region.once();)

#define await(x)\
    while (! (x)) { \
        _region.wait(); \
    }

#endif
