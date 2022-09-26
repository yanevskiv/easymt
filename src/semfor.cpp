#include <semfor.h>

struct ImplSem {
    ImplSem();
    ImplSem(int val);
    ~ImplSem();
    void init(int val);
    void wait();
    void signal();
    void destroy();
private:
    sem_t m_sem;
    bool m_init;
};

ImplSem::ImplSem()
    : m_init(false)
{
    // nothing
}

ImplSem::ImplSem(int val) 
{
    sem_init(&m_sem, 0, val);
    m_init = true;
}

ImplSem::~ImplSem()
{
    this->destroy();
}

void ImplSem::init(int val)
{
    this->destroy();
    sem_init(&m_sem, 0, val);
    m_init = true;
}

void ImplSem::wait()
{
    if (m_init) {
        sem_wait(&m_sem);
    }
}

void ImplSem::signal()
{
    if (m_init) {
        sem_post(&m_sem);
    }
}

void ImplSem::destroy()
{
    if (m_init) {
        sem_destroy(&m_sem);
        m_init = false;
    }
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
    this->m_impl = sem.m_impl;
    sem.m_impl = nullptr;
}

Sem& Sem::operator=(Sem&& sem)
{
    if (&sem != this) {
        this->m_impl = sem.m_impl;
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
    this->m_impl->init(val);
}

void Sem::wait()
{
    this->m_impl->wait();
}

void Sem::signal()
{
    this->m_impl->signal();
}

void Sem::destroy()
{
    this->m_impl->destroy();
}

void inits(Sem& sem, int val)
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

//void inits(sem_t& sem, int val)
//{
//    sem_init(&sem, 0, val);
//}
//
//void wait(sem_t& sem)
//{
//    sem_wait(&sem);
//}
//
//void signal(sem_t& sem)
//{
//    sem_post(&sem);
//}
//
//void destroy(sem_t& sem)
//{
//    sem_close(&sem);
//}


