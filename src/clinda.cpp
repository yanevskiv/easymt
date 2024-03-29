// (c) Ivan Janevski
#include <thread.h>
#include <clinda.h>
#include <cstring>
#include <vector>
#include <stdexcept>

class Linda {
    // threads created using `eval()`
    std::vector<pthread_t> m_eval;

    // tuple space
    std::vector<LTP> m_data;

    // tuple space mutex
    mutable pthread_mutex_t m_mutex;

    // delay condition for `in()` and `rd()`
    mutable pthread_cond_t m_cond;

public:
    // constructor
    Linda();

    // destructor
    ~Linda();

    // check if tuple exists in tuple space (remove it if it does)
    bool inp(LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});

    // remove tuple from tuple space (block if it doesn't exist)
    void in (LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});

    // block if tuple doesn't exist
    void rd (LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});

    // check if tuple exists
    bool rdp(LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});

    // add a passive tuple into tuple space
    void out(LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});

    // create a thread 
    void add_eval(std::function<void()>*);

    // join all threads
    void waitFor();

    // print tuple space to stdout
    void listp();
};

// tuple fragment (no type)
LT::LT() 
{
    m_type = LT_NONE;
}

// tuple fragment (int)
LT::LT(int x) 
{
    m_type = LT_INT;
    m_data.m_int = x;
    m_isPtr = false;
}

// tuple fragment (double)
LT::LT(double x) 
{
    m_type = LT_DOUBLE;
    m_data.m_double = x;
    m_isPtr = false;
}

// tuple fragment (string)
LT::LT(const char *x) 
{
    m_type = LT_STRING;
    m_data.m_str = x;
    m_isPtr = false;
}

// tuple fragment (?int)
LT::LT(int *x) 
{
    m_type = LT_INT;
    m_data.m_ptr = (void*)x;
    m_isPtr = true;
}

// tuple fragment (?double)
LT::LT(double *x) 
{
    m_type = LT_DOUBLE;
    m_data.m_ptr = (void*)x;
    m_isPtr = true;
}

// tuple fragment (?string)
LT::LT(const char **x) 
{
    m_type = LT_STRING;
    m_data.m_ptr = (void*)x;
    m_isPtr = true;
}

// is template fragment (?type)
bool LT::isPtr() const
{
    return m_isPtr;
}

// tuple fragment type
int LT::type() const
{
    return m_type;
}

// populate template fragments with values
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

// check if tuple fragment is equal to another tuple fragment
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

// constructor
Linda::Linda()
{
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_cond, NULL);
}

// destructor
Linda::~Linda()
{
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
}

// tuple constructor
LTP::LTP(LT a, LT b, LT c, LT d, LT e, LT f) 
{
    m_data[0] = a;
    m_data[1] = b;
    m_data[2] = c;
    m_data[3] = d;
    m_data[4] = e;
    m_data[5] = f;
}

// tuple assignment
void LTP::assign(const LTP& ltp)
{
    if (! equals(ltp))
        return;
    for (int i = 0; i < LTP_MAX; i++) {
        m_data[i].assign(ltp.m_data[i]);
    }
}

// check if two tuples are equal
bool LTP::equals(const LTP& ltp)
{
    for (int i = 0; i < LTP_MAX; i++) {
        if (! m_data[i].equals(ltp.m_data[i]))
            return false;
    }
    return true;
}

// remove tuple from tuple space (returns true if tuple was removed)
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

// remove tuple from tuple space (block if tuple doesn't exist)
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

// put tuple into tuple space
void Linda::out(LT a, LT b, LT c, LT d, LT e, LT f)
{
    pthread_mutex_lock(&m_mutex);
    LTP data {a, b, c, d, e, f};
    m_data.push_back(data);
    pthread_cond_broadcast(&m_cond);
    pthread_mutex_unlock(&m_mutex);
}

// block if tuple doesn't exist in tuple space
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

// check if tuple exists in tuple space
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

// print tuple space to `stdout`
void Linda::listp()
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

// create static tuple space
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
void listp()
{
    linda.listp();
}

// add thread 
void Linda::add_eval(std::function<void()> *f)
{
    pthread_t p;
    pthread_create(&p, NULL, [](void* data) -> void*{
        std::function<void()> *fn = (std::function<void()>*)data;
        try {
            (*fn)();
        } catch (const char *msg) {
            println(RED " !!! " NONE "Error: %s", msg);
        }
        return data;
    }, f);
    m_eval.push_back(p);
}

// join threads
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

// `main()` needed to be replaced becuase C-Linda uses `init()` instead
// (created threads have to be joined at exit)
#undef main
extern int init();
int main() {
    try {
        init();
    } catch (const char* msg) {
        println(RED " !!! " NONE "Error: %s", msg);
    }
    linda.waitFor();
    return 0;
}

