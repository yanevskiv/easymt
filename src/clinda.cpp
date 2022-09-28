#include <thread.h>
#include <clinda.h>
#include <cstring>
#include <vector>
#include <stdexcept>

class Linda {
    std::vector<pthread_t> m_eval;
    std::vector<LTP> m_data;
    mutable pthread_mutex_t m_mutex;
    mutable pthread_cond_t m_cond;
    
public:
    Linda();
    ~Linda();
    bool inp(LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});
    void in (LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});
    void out(LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});
    void rd (LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});
    bool rdp(LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});
    void add_eval(std::function<void()>*);
    void add_eval(std::function<int()>);
    void add_eval(std::function<double()>);
    void add_eval(std::function<const char *()>);
    void waitFor();
    void listp();
};

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

