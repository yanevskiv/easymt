#include <concur.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <string.h>
#include <clinda.h>

static void *wrapper(void* void_func) {
    void (*func)(void) = (void (*)(void)) void_func;
    func();
    return 0;
}
pthread_t createThread(void (*func)(void)) {
    pthread_t thread;
    pthread_create(&thread, 0, wrapper, (void*)func);
    return thread;
}

struct struct_id_t {
    void (*m_func)(int);
    int m_id;
};

static void *wrapper_id(void *data)
{
    struct_id_t *struct_id = (struct_id_t*) data;
    struct_id->m_func(struct_id->m_id);
    free(struct_id);
    return 0;
}

pthread_t createThread(void (*func)(int), int id)
{
    struct_id_t *struct_id = (struct_id_t*)calloc(1, sizeof(struct_id_t));
    struct_id->m_func  = func;
    struct_id->m_id = id;
    assert(struct_id != 0);

    pthread_t thread;
    int creation = pthread_create(&thread, 0, wrapper_id, (void*)struct_id);
    assert(creation == 0);
    return thread;
}

void join(pthread_t thread) {
    pthread_join(thread, 0);
}

void init(sem_t& sem, int val)
{
    sem_init(&sem, 0, val);
}


void wait(sem_t& sem)
{
    sem_wait(&sem);
}

void signal(sem_t& sem)
{
    sem_post(&sem);
}

void destroy(sem_t& sem)
{
    sem_close(&sem);
}

void delay(int mili)
{
    if (mili >= 0) {
        usleep(1000 * mili);
    }
}

void rand_delay(int max, int min)
{
    if (max < 0) {
        delay(max);
        return;
    } else if (min < 0 || min == max) {
        delay(max);
        return;
    }
    if (min > max) {
        int tmp = min;
        min = max;
        max = tmp;
    }
    int milis = rand() % (max - min) + min;
    if (milis >= 0)
        delay(milis);
}

void rand_usleep(int max, int min)
{
    max = (max < 0) ? 0 : max;
    min = (min < 0) ? 0 : min;
    if (min == max) {
        usleep(max);
    } else {
        if (min > max) {
            int tmp = min;
            min = max;
            max = tmp;
        }
        int us = rand() % (max - min) + min;
        if (us >= 0)
            usleep(us);
    }
}


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
void skip()
{
    usleep(0);
}

void yield()
{
    pthread_yield();
}

//int test_and_set(volatile int *lock)
//{
//    int retval;
//    __asm__ __volatile__ (
//        "movl $0x1, %0 \n"
//        "lock xchg %0, (%1) \n" : "=&r"(retval) : "r"(lock) : "memory"
//    );
//    return retval;
//}
//
//int fetch_and_add(volatile int *variable, int value)
//{
//	__asm__ volatile("lock; xaddl %0, %1"
//      : "+r" (value), "+m" (*variable) // input + output
//      : // No input-only
//      : "memory"
//    );
//    return value;
//}

#define tvms(tv) ((tv).tv_sec * 1000 + (tv).tv_usec / 1000)
#define tvus(tv) ((tv).tv_sec * 1000 * 1000 + (tv).tv_usec)
void work(int ms) 
{
    struct timeval tv_init, tv_next;
    gettimeofday(&tv_init, 0);
    do {
        gettimeofday(&tv_next, 0);
        if (tvms(tv_next) - tvms(tv_init) >= ms)
            break;
    } while (true);
}

void nanowork(int us) 
{
    struct timeval tv_init, tv_next;
    gettimeofday(&tv_init, 0);
    do {
        gettimeofday(&tv_next, 0);
        if (tvus(tv_next) - tvus(tv_init) >= us)
            break;
    } while (true);
}


LT::LT() 
{
    m_type = LT_NONE;
}

LT::LT(int x) 
{
    m_type = LT_INT;
    m_data.m_int = x;
    m_isPtr = false;
}

LT::LT(double x) 
{
    m_type = LT_DOUBLE;
    m_data.m_double = x;
    m_isPtr = false;
}

LT::LT(const char *x) 
{
    m_type = LT_STRING;
    m_data.m_str = x;
    m_isPtr = false;
}

LT::LT(int *x) 
{
    m_type = LT_INT;
    m_data.m_ptr = (void*)x;
    m_isPtr = true;
}

LT::LT(double *x) 
{
    m_type = LT_DOUBLE;
    m_data.m_ptr = (void*)x;
    m_isPtr = true;
}

LT::LT(const char **x) 
{
    m_type = LT_STRING;
    m_data.m_ptr = (void*)x;
    m_isPtr = true;
}

bool LT::isPtr() const
{
    return m_isPtr;
}

int LT::type() const
{
    return m_type;
}

void LT::assign(const LT& lt)
{
    if (isPtr()) {
        switch (type()) {
            case LT_INT: {
                *((int*)(m_data.m_ptr)) = lt.m_data.m_int;
            } break;
            case LT_DOUBLE: {
                *((double*)(m_data.m_ptr)) = lt.m_data.m_double;
            } break;
            case LT_STRING: {
                *((const char**)(m_data.m_ptr)) = lt.m_data.m_str;
            } break;
        }
    }
}

bool LT::equals(const LT& lt)
{
    if (type() != lt.type())
        return false;
    if (type() == lt.type() && (isPtr() || lt.isPtr())) 
        return true;
    switch (type()) {
        case LT_INT: {
            return (m_data.m_int == lt.m_data.m_int);
        } break;
        case LT_DOUBLE: {
            return (m_data.m_double == lt.m_data.m_double);
        } break;
        case LT_STRING: {
            return ! strcmp(m_data.m_str, lt.m_data.m_str);
        } break;
        case LT_NONE: {
            return true;
        } break;
    }
    return false;
}

Linda::Linda() {
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_cond, NULL);
}

Linda::~Linda() {
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
}



LTP::LTP(LT a, LT b, LT c, LT d, LT e, LT f) 
{
    m_data[0] = a;
    m_data[1] = b;
    m_data[2] = c;
    m_data[3] = d;
    m_data[4] = e;
    m_data[5] = f;
}

void LTP::assign(const LTP& ltp)
{
    if (! equals(ltp))
        return;
    for (int i = 0; i < LTP_MAX; i++) {
        m_data[i].assign(ltp.m_data[i]);
    }
}

bool LTP::equals(const LTP& ltp)
{
    for (int i = 0; i < LTP_MAX; i++) {
        if (! m_data[i].equals(ltp.m_data[i]))
            return false;
    }
    return true;
}

bool Linda::inp(LT a, LT b, LT c, LT d, LT e, LT f) 
{
    pthread_mutex_lock(&m_mutex);
    LTP ltp {a, b, c, d, e, f};
    bool found = false; 
    for (int i = m_data.size() - 1; i >= 0; i--) {
        if (m_data[i].equals(ltp)) {
            ltp.assign(m_data[i]);
            m_data.erase(m_data.begin() + i);
            found = true;
            break;
        }
    }
    pthread_mutex_unlock(&m_mutex);
    return found;
}

void Linda::in(LT a, LT b, LT c, LT d, LT e, LT f)
{
    pthread_mutex_lock(&m_mutex);
    LTP ltp {a, b, c, d, e, f};
    bool found = false; 
    while (! found) {
        for (int i = m_data.size() - 1; i >= 0; i--) {
            if (m_data[i].equals(ltp)) {
                ltp.assign(m_data[i]);
                m_data.erase(m_data.begin() + i);
                found = true;
                break;
            }
        }
        if (! found)  {
            pthread_cond_wait(&m_cond, &m_mutex);
        } 
    }
    pthread_mutex_unlock(&m_mutex);
}
void Linda::out(LT a, LT b, LT c, LT d, LT e, LT f)
{
    pthread_mutex_lock(&m_mutex);
    LTP data {a, b, c, d, e, f};
    m_data.push_back(data);
    pthread_cond_broadcast(&m_cond);
    pthread_mutex_unlock(&m_mutex);
}
void Linda::rd(LT a, LT b, LT c, LT d, LT e, LT f)
{
    pthread_mutex_lock(&m_mutex);
    LTP ltp {a, b, c, d, e, f};
    bool found = false; 
    while (! found) {
        for (int i = m_data.size() - 1; i >= 0; i--) {
            if (m_data[i].equals(ltp)) {
                ltp.assign(m_data[i]);
                found = true;
                break;
            }
        }
        if (! found)  {
            pthread_cond_wait(&m_cond, &m_mutex);
        } 
    }
    pthread_mutex_unlock(&m_mutex);
}

bool Linda::rdp(LT a, LT b, LT c, LT d, LT e, LT f)
{
    pthread_mutex_lock(&m_mutex);
    LTP ltp {a, b, c, d, e, f};
    bool found = false; 
    for (int i = m_data.size() - 1; i >= 0; i--) {
        if (m_data[i].equals(ltp)) {
            ltp.assign(m_data[i]);
            found = true;
            break;
        }
    }
    pthread_mutex_unlock(&m_mutex);
    return found;
}

void Linda::show()
{
    pthread_mutex_lock(&m_mutex);
    print("Tuple space (count: %d):\n", (int)m_data.size());
    for (int i = 0; i < m_data.size(); i++) {
        print("  (");
        for (int j = 0; j < LTP_MAX; j++) {
            LT lt = m_data[i].m_data[j];
            if (lt.type() == LT_NONE)
                break;
            if (j != 0)
                print(", ");
            if (! lt.isPtr()) {
                switch (lt.type()) {
                    case LT_INT: {
                        print("%d", lt.m_data.m_int);
                    } break;
                    case LT_DOUBLE: {
                        print("%g", lt.m_data.m_double);
                    } break;
                    case LT_STRING: {
                        print("\"%s\"", lt.m_data.m_str);
                    } break;
                }
            } else {
                print("%s", 
                    lt.type() == LT_INT ? "INT"
                    : lt.type() == LT_STRING ? "STR"
                    : lt.type() == LT_DOUBLE ? "DBL"
                    : "UNK"
                );
            }
        }
        print(")\n");
    }
    pthread_mutex_unlock(&m_mutex);
}


static Linda linda;

bool inp(LT a, LT b, LT c, LT d, LT e, LT f) 
{
    return linda.inp(a, b, c, d, e, f);
}
void in (LT a, LT b, LT c, LT d, LT e, LT f)
{
    linda.in(a, b, c, d, e, f);
}
void out(LT a, LT b, LT c, LT d, LT e, LT f)
{
    linda.out(a, b, c, d, e, f);
}
void rd (LT a, LT b, LT c, LT d, LT e, LT f)
{
    linda.rd(a, b, c, d, e, f);
}
bool rdp(LT a, LT b, LT c, LT d, LT e, LT f)
{
    return linda.rdp(a, b, c, d, e, f);
}
void show()
{
    linda.show();
}

void Linda::add_eval(std::function<void()> *f)
{
    pthread_t p;
    pthread_create(&p, NULL, [](void* data) -> void*{
        std::function<void()> *fn = (std::function<void()>*)data;
        (*fn)();
        return data;
    }, f);
    m_eval.push_back(p);
}

void Linda::waitFor()
{
    for (int i = 0; i < m_eval.size(); i++) {
        std::function<void()> *p = nullptr;
        pthread_join(m_eval[i], (void**)&p);
        delete p;
    }
}


void add_eval(std::function<void()> *f)
{
    linda.add_eval(f);
}
//void add_eval(std::function<int()> f)
//{
//    linda.add_eval(f);
//}
//void add_eval(std::function<double()> f)
//{
//    linda.add_eval(f);
//}
//void add_eval(std::function<const char *()> f)
//{
//    linda.add_eval(f);
//}

#undef main
extern int KDP_main();
int main() {
    KDP_main();
    linda.waitFor();
    return 0;
}

mbx_t::mbx_t() 
{
    
}
mbx_t::~mbx_t()
{

}

struct impl_mbx_t {
    int m_id;
    std::vector<msg_t*> m_msgs;
};
static std::vector<impl_mbx_t> mbx_vec;

mbx_t mbx_open(const char *fmt, ...) 
{
    return mbx_t{};
}

void _mbx_get(msg_t *msg, mbx_t mbx, int len, int *status) 
{
}

void _mbx_put(msg_t *msg, mbx_t mbx) 
{
}

void mbx_close(mbx_t )
{
}


