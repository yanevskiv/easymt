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

// monitor condition
struct cond {
    // constructor (monitor must be set later with cond::set_monitor(...))
    cond();

    // constructor with monitor argument
    cond(Monitor *mon);

    // destructor
    ~cond();

    // wait on this condition
    void wait(int prio = 0);

    // signal the next thread in the queue waiting on this condition
    void signal();

    // signal every thread waiting on condition
    // (note: only allowed in monitors using `Signal-And-Continue` discipline)
    void signalAll();

    // same as above
    void signal_all();

    // return the priority value of the first thread in the waiting queue
    int minrank();

    // check if waiting queue is empty
    bool empty();

    // return how many threads are in the waiting queue
    int queue();

    // set monitor
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
// if priority argument is omitted FCFS (FIFO) is assumed
void wait(cond&, int prio = 0);

// signal a thread waiting on condition
// if there are no threads do nothing 
void signal(cond&);

// signal every thread waiting on condition
// (only allowed under `Signal-And-Continue` discipline)
void signalAll(cond&);

// same as above
void signal_all(cond&);

// return the priority value of the first thread in the waiting queue
int minrank(cond&);

// check if waiting queue is empty
bool empty(cond&);

// return how many threads are in the waiting queue
int queue(cond&);


// Signal-And-Continue monitor
struct SC_Monitor : Monitor {
    SC_Monitor() : Monitor(SC) {
    
    }
};

// Signal-And-Wait monitor (SUW by default)
struct SW_Monitor : Monitor {
    SW_Monitor() : Monitor(SW) {
    
    }
};

// Signal-And-Urgent-Wait monitor (not implemented)
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

// monitor block (must be used  
#define monitor \
    for (RAII_MONITOR_LOCK _m_lock(this);_m_lock.checkOnce();)

#endif

