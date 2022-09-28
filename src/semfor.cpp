#include <semfor.h>

// Semaphore implementation
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

ImplSem::ImplSem()
    : m_active(false)
{
    // nothing
}

ImplSem::ImplSem(int val) 
    : m_active(true)
{
    sem_init(&m_sem, 0, val);
}

ImplSem::~ImplSem()
{
    this->destroy();
}

void ImplSem::init(int val)
{
    this->destroy();
    sem_init(&m_sem, 0, val);
    m_active = true;
}

void ImplSem::wait()
{
    if (m_active) {
        sem_wait(&m_sem);
    } 
}

void ImplSem::signal()
{
    if (m_active) {
        sem_post(&m_sem);
    } else throw "`signal()` called on an uninitialized semaphore!";
}

void ImplSem::destroy()
{
    if (m_active) {
        sem_destroy(&m_sem);
        m_active = false;
    } 
}

bool ImplSem::active() const
{
    return m_active;
}

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
    if (m_impl->active())
        throw "`Sem(Sem&&)` called on an already initialized semaphore!";
    delete m_impl;
    m_impl = sem.m_impl;
    sem.m_impl = nullptr;
}

Sem& Sem::operator=(Sem&& sem)
{
    if (m_impl->active())
        throw "`operator=(Sem&&)` called on an already initialized samaphore!";
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

