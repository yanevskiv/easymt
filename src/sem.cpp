#include <sem.h>

// semaphore implementation
struct ImplSem {
    ImplSem();
    ImplSem(int val);
    ~ImplSem();
    void init(int val);
    void wait();
    void signal();
    void destroy();
    bool active() const;
private:
    sem_t m_sem;
    bool m_active;
};

// create uninitialized semaphore
ImplSem::ImplSem()
    : m_active(true)
{
    sem_init(&m_sem, 0, 0);
}

// create initialized semaphore with value
ImplSem::ImplSem(int val) 
    : m_active(true)
{
    sem_init(&m_sem, 0, val);
}

// destructor (deinitialize semaphore)
ImplSem::~ImplSem()
{
    this->destroy();
}

// initialize an uninitialized semaphore
void ImplSem::init(int val)
{
    this->destroy();
    sem_init(&m_sem, 0, val);
    m_active = true;
}

// wait semaphore
void ImplSem::wait()
{
    if (m_active) {
        sem_wait(&m_sem);
    }  else throw "`wait()` called on an uninitialized semaphore!";
} 

// signal semaphore
void ImplSem::signal()
{
    if (m_active) {
        sem_post(&m_sem);
    } else throw "`signal()` called on an uninitialized semaphore!";
}

// deinitialize semaphore
void ImplSem::destroy()
{
    if (m_active) {
        sem_destroy(&m_sem);
        m_active = false;
    } 
}

// check if semaphore is initialized
bool ImplSem::active() const
{
    return m_active;
}

/* 
 * Semaphore
 */
Sem::Sem() 
    : m_impl(new ImplSem)
{
    // nothing
}

Sem::Sem(int val)
    : m_impl(new ImplSem(val))
{
    // nothing
}

Sem::Sem(Sem&& sem)
{
    delete m_impl;
    m_impl = sem.m_impl;
    sem.m_impl = nullptr;
}

Sem& Sem::operator=(Sem&& sem)
{
    if (&sem != this) {
        delete m_impl;
        m_impl = sem.m_impl;
        sem.m_impl = nullptr;
    }
    return *this;
}

Sem::~Sem() 
{
    delete m_impl;
}

void Sem::init(int val)
{
    m_impl->init(val);
}

void Sem::wait()
{
    if (! m_impl->active())
        throw "`wait()` called on an uninitialized semaphore!";
    m_impl->wait();
}

void Sem::signal()
{
    if (! m_impl->active())
        throw "`signal()` called on an uninitialized semaphore!";
    m_impl->signal();
}

void Sem::destroy()
{
    m_impl->destroy();
}

/* 
 * Procedual-style semaphore operations
 */
void init(Sem& sem, int val)
{
    sem.init(val);
}

void signal(Sem& sem)
{
    sem.signal();
}

void wait(Sem& sem)
{
    sem.wait();
}

void destroy(Sem& sem)
{
    sem.destroy();
}

