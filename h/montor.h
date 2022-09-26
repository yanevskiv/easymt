#ifndef _MONTOR_H_
#define _MONTOR_H_
#include <thread.h>
#define SC 0
#define SW 1
#define SUW 2

struct Monitor;
struct cond;
struct ImplMonitor;
struct ImplCond;

// condition
struct cond {
    // 
    cond(Monitor *mon = nullptr);

    ~cond();
    bool empty();
    int queue();
    void wait(int prio = 0);
    void signal();
    void signalAll();
    void signal_all();
    int minrank();
    void set_monitor(Monitor*);
private:
    ImplCond *m_impl;
};

// generic monitor
struct Monitor {
    Monitor(int disc = SC);
    virtual ~Monitor();
    void enter();
    void leave();
    int disc() const;
protected:
    friend class cond;
    ImplMonitor *m_impl;
};

// wait on condition 
// if priority is omitted, FIFO is assumed
void wait(cond&, int prio = 0);

// signal a thread waiting on condition
// if there are no threads do nothing 
void signal(cond&);

// signal every thread waiting on condition
// (only allowed under `Signal-And-Continue` discipline)
void signalAll(cond&);

// same as above
void signal_all(cond&);

//
int minrank(cond&);
bool empty(cond&);
int queue(cond&);


// Signal-And-Continue monitor
struct SC_Monitor : Monitor {
    SC_Monitor() : Monitor(SC) {
    
    }
};

// Signal-And-Wait monitor
struct SW_Monitor : Monitor {
    SW_Monitor() : Monitor(SW) {
    
    }
};

// Signal-And-Urgent-Wait monitor
struct SUW_Monitor : Monitor {
    SUW_Monitor() : Monitor(SUW) {
    
    }
};

// RAII monitor lock
struct RAII_MONITOR_LOCK {
    RAII_MONITOR_LOCK (Monitor *_monitor) 
        : m_monitor(_monitor), 
          m_checked(false)
    {
        m_monitor->enter();
    }
    ~RAII_MONITOR_LOCK () {
        m_monitor->leave();
    }
    bool checkOnce() {
        if (m_checked)
            return false;
        return (m_checked = true);
    }
protected:
    bool m_checked;
    Monitor *m_monitor;
};

// monitor block
// (same as `syncrhonized (this)` in other languages)
#define monitor \
    for (RAII_MONITOR_LOCK _m_lock(this);_m_lock.checkOnce();)

#endif

