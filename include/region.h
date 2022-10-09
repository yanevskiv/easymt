#ifndef _REGION_H_
#define _REGION_H_
#include <thread.h>

struct ImplRegion;
struct Region {
    // create regionable object
    Region();

    // virtual destructor
    virtual ~Region();

    // enter region
    void lock();

    // exit region
    void unlock();

    // wait condition
    void wait();
private:
    ImplRegion *m_impl;
};


// RAII region lock
struct RAII_REGION_LOCK {
    RAII_REGION_LOCK(Region& reg) 
        : m_region(reg), m_checked(false)
    {
        m_region.lock();
    }
    ~RAII_REGION_LOCK() {
        m_region.unlock();
    }
    void wait() {
        m_region.wait();
    }
    bool checkOnce() {
        if (m_checked) 
            return false;
        m_checked = true;
        return true;
    }
private:
    bool m_checked;
    Region& m_region;
};

// `region (x) { ... }` expression
#define region(r) \
    for (RAII_REGION_LOCK _m_region(r);_m_region.checkOnce();)

// `await(...)` condition 
#define await(x) \
    while (! (x)) { \
        _m_region.wait(); \
    }

#endif
