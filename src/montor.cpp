#include <montor.h>
#include <assert.h>
#include <queue>
#include <vector>
#include <semaphore.h>

struct PrioSemCmp;
struct PrioSem {
    PrioSem(int rank, int fifo) 
        : m_rank(rank), m_fifo(fifo) 
        { sem_init(&m_sem, 0, 0); }
    ~PrioSem() { sem_destroy(&m_sem); }
    void wait() { sem_wait(&m_sem); }
    void signal() { sem_post(&m_sem); }
    int rank() const { return m_rank; }
private:
    friend class PrioSemCmp;
    int m_fifo;
    int m_rank;
    sem_t m_sem;
};

struct PrioSemCmp {
    bool operator()(const PrioSem* sa, const PrioSem* sb)
    {
        if (sa == nullptr || sb == nullptr)
            return false;
        return sa->m_rank > sb->m_rank
            || (sa->m_rank == sb->m_rank && sa->m_fifo > sb->m_fifo);
    }
};

struct ImplMonitor {
    ImplMonitor(int disc);
    ~ImplMonitor();
    void enter();
    void leave();
    int disc() const;
private:
    friend class ImplCond;
    int m_disc;
    sem_t m_mutex;
    std::queue<PrioSem*> m_wqueue;
};

struct ImplCond {
    ImplCond(ImplMonitor *mon);
    ~ImplCond();
    bool empty() const;
    int queue() const;
    void wait(int prio = 0);
    void signal();
    void signalAll();
    void signal_all();
    int minrank() const;
    void set_monitor(ImplMonitor*);
private:
    std::priority_queue<PrioSem*, std::vector<PrioSem*>, PrioSemCmp> m_queue;
    ImplMonitor *m_monitor;
    int m_fifo;
};


ImplMonitor::ImplMonitor(int disc)
    : m_disc(disc)
{
    sem_init(&m_mutex, 0, 1);
}

ImplMonitor::~ImplMonitor()
{
    sem_destroy(&m_mutex);
}

void ImplMonitor::enter() 
{
    sem_wait(&m_mutex);
}


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

int ImplMonitor::disc() const
{
    return m_disc;
}

ImplCond::ImplCond(ImplMonitor *mon)
    : m_monitor(mon), m_fifo (0)
{
}

ImplCond::~ImplCond()
{
    // empty
}

bool ImplCond::empty() const
{
    return m_queue.empty();
}

int ImplCond::queue() const
{
    return m_queue.size();
}

void ImplCond::wait(int prio)
{
    PrioSem *sem = new PrioSem{prio, m_fifo++};
    m_queue.push(sem);
    m_monitor->leave();
    sem->wait();
    delete sem;
    if (m_monitor->disc() == SC)
        m_monitor->enter();
}

void ImplCond::signal()
{
    if (! m_queue.empty()) {
        PrioSem *wsem = nullptr;
        if (m_monitor->disc() == SW) {
            wsem = new PrioSem{0, 0};
            m_monitor->m_wqueue.push(wsem);
        }
        PrioSem* sem = m_queue.top();
        m_queue.pop();
        sem->signal();
        if (m_monitor->disc() == SUW && wsem != nullptr) {
            wsem->wait();
        }
    }
}

void ImplCond::signalAll()
{
    if (m_monitor->disc() == SC) {
        while (! m_queue.empty())
            this->signal();
    } else {
        throw "signalAll() can only be called under Signal-And-Wait monitor!";
    }
}

void ImplCond::signal_all()
{
    this->signalAll();
}

int ImplCond::minrank() const
{
    if (! m_queue.empty()) {
        PrioSem *prio = m_queue.top();
        return prio->rank();
    }
    return 0;
}

void ImplCond::set_monitor(ImplMonitor *mon)
{
    m_monitor = mon;
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

