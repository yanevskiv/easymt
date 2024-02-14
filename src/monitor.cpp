// (c) Ivan Janevski
#include <monitor.h>
#include <assert.h>
#include <queue>
#include <vector>
#include <semaphore.h>

struct PrioSemCmp;

// semaphore that can be ordered by priority (otherwise ordered FIFO)
struct PrioSem {
    // semaphore constructor
    PrioSem(int rank, int fifo) 
        : m_rank(rank), m_fifo(fifo) 
        { sem_init(&m_sem, 0, 0); }

    // semaphore destructor
    ~PrioSem() { sem_destroy(&m_sem); }

    // wait on semaphore
    void wait() { sem_wait(&m_sem); }

    // signal semaphore
    void signal() { sem_post(&m_sem); }

    // get semaphore priority
    int rank() const { return m_rank; }
private:
    friend class PrioSemCmp;

    // semaphore priority
    int m_rank;

    // fifo order (when priorities are equal)
    int m_fifo;

    // POSIX semaphore
    sem_t m_sem;
};

// semaphore comparator
struct PrioSemCmp {
    bool operator()(const PrioSem* sa, const PrioSem* sb)
    {
        if (sa == nullptr || sb == nullptr)
            return false;
        return sa->m_rank > sb->m_rank
            || (sa->m_rank == sb->m_rank && sa->m_fifo > sb->m_fifo);
    }
};

// monitor implementation
struct ImplMonitor {
    // constructor (discipline argumentrequired)
    ImplMonitor(int disc);

    // destructor
    ~ImplMonitor();

    // enter monitor
    void enter();

    // leave monitor
    void leave();

    // get monitor discipline
    int disc() const;
private:
    friend class ImplCond;

    // monitor discipline
    int m_disc;

    // monitor mutex
    sem_t m_mutex;

    // waiting queue (only used in SW monitor)
    std::queue<PrioSem*> m_wqueue;
};

// conditional variable implementation
struct ImplCond {
    // constructor (monitor argument required)
    ImplCond(ImplMonitor *mon);

    // destructor
    ~ImplCond();

    // check if queue is empty
    bool empty() const;

    // get number of queued threads
    int queue() const;

    // block thread
    void wait(int prio = 0);

    // unblock thread
    void signal();

    // unblock all threads
    void signalAll();

    // equivalent to the method above 
    void signal_all();

    // get priority of the next thread to unblock
    int minrank() const;

    // set monitor
    void set_monitor(ImplMonitor*);
private:
    // thread waiting queue
    std::priority_queue<PrioSem*, std::vector<PrioSem*>, PrioSemCmp> m_queue;

    // monitor
    ImplMonitor *m_monitor;

    // FIFO counter
    int m_fifo;
};


// monitor constructor 
ImplMonitor::ImplMonitor(int disc)
    : m_disc(disc)
{
    sem_init(&m_mutex, 0, 1);
}

// monitor destructor
ImplMonitor::~ImplMonitor()
{
    sem_destroy(&m_mutex);
}

// enter monitor
void ImplMonitor::enter() 
{
    sem_wait(&m_mutex);
}

// leave monitor
void ImplMonitor::leave()
{
    if (m_disc == SW && ! m_wqueue.empty()) {
        PrioSem *sem = m_wqueue.front(); 
        m_wqueue.pop();
        sem->signal();
    } else {
        sem_post(&m_mutex);
    }
}

// monitor discipline
int ImplMonitor::disc() const
{
    return m_disc;
}

// condition constructor
ImplCond::ImplCond(ImplMonitor *mon)
    : m_monitor(mon), m_fifo (0)
{
}

// condition destructor
ImplCond::~ImplCond()
{
    // empty
}

// check if waiting queue is empty
bool ImplCond::empty() const
{
    return m_queue.empty();
}

// get number of threads in waiting queue
int ImplCond::queue() const
{
    return m_queue.size();
}

// block thread
void ImplCond::wait(int prio)
{
    if (m_monitor == nullptr)
        throw "cond::wait() called but monitor is not set!";
    PrioSem *sem = new PrioSem{prio, m_fifo++};
    m_queue.push(sem);
    m_monitor->leave();
    sem->wait();
    delete sem;
    if (m_monitor->disc() == SC)
        m_monitor->enter();
}

// unblock one thread
void ImplCond::signal()
{
    if (m_monitor == nullptr)
        throw "cond::signal() called but monitor is not set!";
    if (! m_queue.empty()) {
        PrioSem *wsem = nullptr;
        if (m_monitor->disc() == SW) {
            wsem = new PrioSem{0, 0};
            m_monitor->m_wqueue.push(wsem);
        }
        PrioSem* sem = m_queue.top();
        m_queue.pop();
        sem->signal();
        if (m_monitor->disc() == SW && wsem != nullptr) {
            wsem->wait();
        }
    }
}

// unblock all threads
void ImplCond::signalAll()
{
    if (m_monitor == nullptr)
        throw "cond::signalAll() called but monitor is not set!";
    if (m_monitor->disc() == SC) {
        while (! m_queue.empty())
            this->signal();
    } else {
        throw "cond::signalAll() can only be called in Signal-And-Continue monitors!";
    }
}

// equivalent to above
void ImplCond::signal_all()
{
    if (m_monitor == nullptr)
        throw "cond::signal_all() called but monitor is not set!";
    this->signalAll();
}

// get priority of the next thread to unblock
int ImplCond::minrank() const
{
    if (! m_queue.empty()) {
        PrioSem *prio = m_queue.top();
        return prio->rank();
    }
    return 0;
}

// set monitor
void ImplCond::set_monitor(ImplMonitor *mon)
{
    m_monitor = mon;
}

/*
 * Condition
 */
cond::cond() 
    : m_impl(new ImplCond(nullptr))
{
}
cond::cond(Monitor *mon)
    : m_impl(new ImplCond(mon->m_impl))
{
    // empty
}

cond::~cond() 
{
    delete m_impl;
}

bool cond::empty()
{
    return m_impl->empty();
}

int cond::queue()
{
    return m_impl->queue();
}

int cond::minrank()
{
    return m_impl->minrank();
}

void cond::wait(int prio)
{
    m_impl->wait(prio);
}

void cond::signal()
{
    m_impl->signal();
}

void cond::signalAll()
{
    m_impl->signalAll();
}

void cond::signal_all()
{
    m_impl->signalAll();
}

void cond::set_monitor(Monitor *mon)
{
    m_impl->set_monitor(mon->m_impl);
}

/*
 * Procedual-style condition operations
 */
int Monitor::disc() const
{
    return m_impl->disc();
}

void wait(cond& c, int prio)
{
    c.wait(prio);
}

void signal(cond& c)
{
    c.signal();
}

void signalAll(cond& c)
{
    c.signalAll();
}
void signal_all(cond& c)
{
    c.signal_all();
}
int minrank(cond& c)
{
    return c.minrank();
}

bool empty(cond& c)
{
    return c.empty();
}

int queue(cond& c)
{
    return c.queue();
}

/*
 * Monitor
 */
Monitor::Monitor(int disc) 
    : m_impl(new ImplMonitor(disc))
{
    // empty
}

Monitor::~Monitor()
{
    delete m_impl;
}

void Monitor::enter()
{
    m_impl->enter();
}

void Monitor::leave()
{
    m_impl->leave();
}


