#ifndef _MONTOR_H_
#define _MONTOR_H_
#include <thread.h>
#include <semfor.h>
#include <region.h>
#include <queue>
#include <vector>
using namespace std;
struct Monitor;

struct sem_prio_t {
    int m_fifo;
    int m_val;
    sem_t *m_sem;
};

struct sem_prio_cmp_t {
    bool operator()(const sem_prio_t* p_a, const sem_prio_t* p_b) {
        if (p_a == 0 || p_b == 0)
            return false;
        return p_a->m_val > p_b->m_val || (p_a->m_val == p_b->m_val && p_a->m_fifo > p_b->m_fifo);
    }
};

struct cond {
    cond(Monitor *_monitor = nullptr);
    ~cond();
    bool empty();
    int queue();
    void wait(int prio = 0);
    void signal();
    void signalAll();
    void signal_all();
    int minrank();
    void set_monitor(Monitor *_monitor);
protected:
    Monitor *m_monitor;
    int m_count;
    int m_fifo;
    sem_t *m_cv;
    sem_t *m_queue_mutex;
    priority_queue<sem_prio_t*, vector<sem_prio_t*>, sem_prio_cmp_t> *m_queue;
};
enum {
    SC,
    SW,
    SUW
};

struct Monitor {
    Monitor(int disc = SC);
    virtual ~Monitor();
    void lock();
    void unlock();
protected:
    int m_disc;
    sem_t *m_waited;
    sem_t *m_mutex;
    int m_waited_count;
    friend class cond;
};


struct SC_Monitor : Monitor {
    SC_Monitor() : Monitor(SC) {
    
    }
};

struct SW_Monitor : Monitor {
    SW_Monitor() : Monitor(SW) {
    
    }

};

struct SUW_Monitor : Monitor {
    SUW_Monitor() : Monitor(SUW) {
    
    }
};

struct MONITOR {
    MONITOR (Monitor *_monitor) : m_monitor(_monitor), m_once(true){
        m_monitor->lock();
    }
    ~MONITOR () {
        m_monitor->unlock();
    }
    bool once() {
        bool old = m_once;
        m_once = false;
        return old;
    }
protected:
    bool m_once;
    Monitor *m_monitor;
};

#define monitor \
    for (MONITOR _m_lock(this);_m_lock.once();)

#endif
