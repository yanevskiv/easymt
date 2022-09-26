#include <montor.h>
#include <assert.h>
#include <queue>
#include <vector>
#include <semaphore.h>

struct SemPrioCmp;
struct SemPrio {
    SemPrio(int prio, int fifo) : m_prio(prio), m_fifo(fifo) { sem_init(&m_sem, 0, 0); }
    ~SemPrio() { sem_destroy(&m_sem); }
    void wait() { sem_wait(&m_sem); }
    void signal() { sem_post(&m_sem); }
    int rank() const { return m_prio; }
private:
    friend class SemPrioCmp;
    int m_fifo;
    int m_prio;
    sem_t m_sem;
};

struct SemPrioCmp {
    bool operator()(const SemPrio* sa, const SemPrio* sb)
    {
        if (sa == nullptr || sb == nullptr)
            return false;
        return sa->m_prio > sb->m_prio
            || (sa->m_prio == sb->m_prio && sa->m_fifo > sb->m_fifo);
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
    int m_wfifo;
    int m_efifo;
    int m_wcount;
    int m_disc;
    sem_t m_wait;
    sem_t m_mutex;
    std::queue<SemPrio*> m_wqueue;
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
    std::priority_queue<SemPrio*, std::vector<SemPrio*>, SemPrioCmp> m_queue;
    ImplMonitor *m_monitor;
    int m_count;
    int m_fifo;
};


ImplMonitor::ImplMonitor(int disc)
    : m_disc(disc), m_wcount(0), m_wfifo(0), m_efifo(0)
{
    sem_init(&m_wait, 0, 0);
    sem_init(&m_mutex, 0, 1);
}

ImplMonitor::~ImplMonitor()
{
    sem_destroy(&m_wait);
    sem_destroy(&m_mutex);
}

void ImplMonitor::enter() 
{
    sem_wait(&m_mutex);
}


void ImplMonitor::leave()
{
    if (m_disc == SW && m_wcount > 0) {
        sem_post(&m_wait);
    } else if (m_disc == SUW && ! m_wqueue.empty()) {
        SemPrio *sem = m_wqueue.front(); 
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
    SemPrio *sem = new SemPrio{prio, m_fifo++};
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
        SemPrio *wsem = nullptr;
        if (m_monitor->disc() == SW) {
            m_monitor->m_wcount++;
        } if (m_monitor->disc() == SUW) {
            wsem = new SemPrio{0, m_monitor->m_wfifo++};
            m_monitor->m_wqueue.push(wsem);
        }
        SemPrio* sem = m_queue.top();
        m_queue.pop();
        sem->signal();
        if (m_monitor->disc() == SW) {
            sem_wait(&m_monitor->m_wait);
            m_monitor->m_wcount--;
        } else if (m_monitor->disc() == SUW && wsem != nullptr) {
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
    }
}

int ImplCond::minrank() const
{
    if (! m_queue.empty()) {
        SemPrio *prio = m_queue.top();
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

