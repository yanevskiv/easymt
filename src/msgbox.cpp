#include <msgbox.h>
#include <map>
#include <queue>
#include <pthread.h>
#include <cstdlib>
#include <print.h>
#include <semaphore.h>
#include <unordered_map>
#include <ctime>
#include <cerrno>
#include <cstdarg>

// message box implementation
struct ImplMbx {
    // create message box
    ImplMbx(int id) : m_id(id) {
        pthread_mutex_init(&m_mutex, nullptr);
        pthread_cond_init(&m_empty, nullptr);
    }

    // destroy message box
    ~ImplMbx() {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_empty);
    }

    // put a message in this message box
    void put(msg_t* msg) {
        pthread_mutex_lock(&m_mutex);
        m_messages.push(msg);
        pthread_cond_signal(&m_empty);
        pthread_mutex_unlock(&m_mutex);
    }

    // get message from this message box or block if there are are none
    msg_t *get(int len = INF, int *st = nullptr) {
        pthread_mutex_lock(&m_mutex);
        if (st) {
            *st = UNKNOWN;
        }
        while (m_messages.empty())  {
            if (len == INF) {
                pthread_cond_wait(&m_empty, &m_mutex);
            } else if (len >= 0) {
                struct timespec abstime;
                abstime.tv_sec = len / 1000;
                abstime.tv_nsec = len * 1000;
                if (pthread_cond_timedwait(&m_empty, &m_mutex, &abstime) == ETIMEDOUT) {
                    pthread_mutex_unlock(&m_mutex);
                    if (st) {
                        *st = EXPIRED;
                    }
                    return nullptr;
                }
            }
        }
        if (st) {
            *st = SUCCESS;
        }
        msg_t *result = m_messages.front();
        m_messages.pop();
        pthread_mutex_unlock(&m_mutex);
        return result;
    }
private:
    // unique identifier
    int m_id;

    // block condition if message box is empty
    pthread_cond_t m_empty;

    // mutex
    pthread_mutex_t m_mutex;

    // message queue
    std::queue<msg_t*> m_messages;
};

// all message boxes
struct AllBoxes {
    // constructor
    AllBoxes() {
        pthread_mutex_init(&m_mutex, nullptr);
    }

    // destructor
    ~AllBoxes() {
        pthread_mutex_destroy(&m_mutex);
    }

    // get existing message box with `id` 
    // create it if it doesn't exist
    ImplMbx* operator[](int id) {
        pthread_mutex_lock(&m_mutex);
        ImplMbx* result = m_boxes[id];
        if (result == nullptr) {
            result = new ImplMbx(id);
            m_boxes[id] = result;
        }
        pthread_mutex_unlock(&m_mutex);
        return result;
    }
private:
    // map of `id -> message box`
    std::unordered_map<int, ImplMbx*> m_boxes;

    // mutex
    pthread_mutex_t m_mutex;
};

// global message boxes
static AllBoxes BOXES;

// create message box
mbx_t::mbx_t() 
    : m_id(rand())
{
}

// create message box with id
mbx_t::mbx_t(int id)
    : m_id(id)
{
    // nothing
}

// create message box with name
mbx_t::mbx_t(const char* name)
{
    unsigned hash = 0;
    for ( ; *name ; name++)
        hash = ((hash << 5) - hash) + *name;
    m_id = (int)hash;
}

// open message box with id
mbx_t mbx_open(int id)
{
    return mbx_t{id};
}

// open message box with name (can be formatted)
mbx_t mbx_open(const char *fmt, ...) 
{
    char buffer[BUFSIZ];
    va_list va;
    va_start(va, fmt);
    vsnprintf(buffer, BUFSIZ, fmt, va);
    mbx_t result = mbx_t{buffer};
    va_end(va);
    return result;
}

// get message from message box
void _mbx_get(msg_t **msg, mbx_t mbx, int len, int *st) 
{
    *msg = BOXES[mbx.m_id]->get(len, st);
}

// put message in message box
void _mbx_put(msg_t *msg, mbx_t mbx) 
{
    BOXES[mbx.m_id]->put(msg);
}

// close message box
void mbx_close(mbx_t& mbx)
{
    mbx.m_id = -1;
}


