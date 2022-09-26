#include <montor.h>
#include <assert.h>

cond::cond(Monitor *_monitor) : m_monitor(_monitor)
{
    m_fifo = 0;
    m_count = 0;
    m_cv = new sem_t;
    m_queue_mutex = new sem_t;
    sem_init(m_cv, 0, 0);
    sem_init(m_queue_mutex, 0, 1);
    m_queue = new priority_queue<sem_prio_t*, vector<sem_prio_t*>, sem_prio_cmp_t>;
}

void cond::set_monitor(Monitor *_monitor)
{
    m_monitor = _monitor;
}

cond::~cond() 
{
    sem_destroy(m_queue_mutex);
    sem_destroy(m_cv);
    delete m_cv;
    delete m_queue_mutex;
}

void cond::wait(int prio) 
{
    sem_t *sem = new sem_t;
    int res = sem_init(sem, 0, 0);
    assert(res == 0);
    m_count += 1;
    sem_prio_t *sem_prio = new sem_prio_t;
    sem_prio->m_sem = sem;
    sem_prio->m_val = prio;
    sem_prio->m_fifo = m_fifo;
    m_fifo += 1;
    m_queue->push(sem_prio);


    if (m_monitor->m_disc == SW && m_monitor->m_waited_count > 0) {
        sem_post(m_monitor->m_waited);
    } else {
        sem_post(m_monitor->m_mutex);
    }
    sem_wait(sem);
    sem_destroy(sem);
    delete sem;
    delete sem_prio;
    if (m_monitor->m_disc == SC)
        sem_wait(m_monitor->m_mutex);
    //sem_wait(&m_cv);

    //if (m_monitor->m_disc == SIGNAL_AND_NOTHING) {
    //    pthread_cond_wait(&m_cond, &(m_monitor->m_mutex));
    //} else if (m_monitor->m_disc == SIGNAL_AND_CONTINUE) {
    //    pthread_cond_wait(&m_cond, &(m_monitor->m_mutex));
    //    //pthread_cond_wait(&m_cond, m_mutex);
    //    //pthread_cond_t *p_cond = new pthread_cond_t;
    //    //pthread_cond_init(p_cond, 0);

    //    ////cond_elem elem(p_cond, prio);
    //    ////m_queue.push(elem);
    //    //m_queue2.push_back(p_cond);
    //    //pthread_cond_wait(p_cond, &(m_monitor->m_mutex));
    //}
}

void cond::signal() 
{
    if (m_count > 0) {
        if (m_monitor->m_disc == SW) 
            m_monitor->m_waited_count += 1;
        //sem_post(&m_cv);


        //if (m_queue.top()) {
        //    cond_sem *sem = m_queue.top();
        //    m_queue.pop();
        //    printf("(%d) ", sem->m_prio);
        //    sem->signal();
        //}
        sem_prio_t *prio = nullptr;
        if (! m_queue->empty()) {
            prio = m_queue->top();
            m_queue->pop();
        }
        if (prio) {
            m_count -= 1;
            sem_post(prio->m_sem);
        }

        if (m_monitor->m_disc == SW)  {
            sem_wait(m_monitor->m_waited);
            m_monitor->m_waited_count -= 1;
        }
    }
    //if (m_monitor->m_disc == SIGNAL_AND_NOTHING) {
    //    pthread_cond_broadcast(&m_cond);
    //} else if (m_monitor->m_disc == SIGNAL_AND_CONTINUE) {
    //    //if (! m_queue.empty()) {
    //    //    //cond_elem elem = m_queue.top();
    //    //    //m_queue.pop();
    //    //    //pthread_cond_broadcast(&(elem.m_cond));
    //    //    pthread_cond_t *c = m_queue2.back();
    //    //    m_queue2.pop_back();
    //    //    pthread_cond_signal(c);
    //    //}
    //}
}

void cond::signalAll() 
{
    if (m_monitor->m_disc == SC)  {
        while (m_count > 0) {
            this->signal();
        }
    } else if (m_monitor->m_disc == SW) {
        perror("\"Signal all\" cannot be used with Signal-And-Wait discipline.\n");
    }
}

void cond::signal_all() 
{
    signalAll();
}
int cond::minrank() 
{
    if (! m_queue->empty()) {
        sem_prio_t *prio = m_queue->top();
        return prio->m_val;
    }
    return 0;
}

int cond::queue() 
{
    return m_count;
}

bool cond::empty() 
{
    return (m_count == 0);
}


Monitor::Monitor(int disc) : m_disc(disc)
{
    m_mutex = new sem_t;
    m_waited = new sem_t;
    m_waited_count = 0;
    sem_init(m_mutex, 0, 1);
    sem_init(m_waited, 0, 0);
}

Monitor::~Monitor()
{
    sem_destroy(m_mutex);
    sem_destroy(m_waited);
    delete m_mutex;
    delete m_waited;
}

void Monitor::lock() 
{
    //debug("Monitor lock()");
    sem_wait(m_mutex);
}
void Monitor::unlock() 
{
    if (m_disc == SW && m_waited_count > 0) {
        //debug("Monitor unlock() waiting...");
        sem_post(m_waited);
    } else {
        //debug("Monitor unlock() mutex...");
        sem_post(m_mutex);
    }
}
