#ifndef _CLINDA_H_
#define _CLINDA_H_
#include <functional>
#include <process.h>

// tuple fragment types
enum {
    LT_NONE,
    LT_INT,
    LT_DOUBLE,
    LT_STRING
};

// tuple fragment
struct LT {
    bool m_isPtr;
    int m_type;
    union {
        const char *m_str;
        int m_int;
        double m_double;
        void *m_ptr;
    } m_data;
    LT();
    LT(const char*);
    LT(int);
    LT(double);
    LT(const char**);
    LT(int*);
    LT(double*);
    bool isPtr() const;
    int type() const;
    void assign(const LT&);
    bool equals(const LT&);
};

// tuple
#define LTP_MAX 6
struct LTP {
    LT m_data[LTP_MAX];
    LTP(LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});
    void assign(const LTP& ltp);
    bool equals(const LTP& ltp);
};

// remove a tuple and return true if successful (non-blocking)
bool inp(LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});

// remove a tuple and block if it doesn't exist in tuple space
void in (LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});

// check if tuple exists and block if it doesn't 
void rd (LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});

// check if tuple exists and return `true` if it does (non-blocking)
bool rdp(LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});

// add tuple into tuple space
void out(LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});

// list tuples in the tuple space
void listp();

// add thread
void add_eval(std::function<void()>*);

// activate thread (lambda)
#define eval(CODE) \
    do {\
        std::function<void()> *_f = new std::function<void()>([=](){ \
           CODE; \
        }); \
        add_eval(_f);\
    } while (0)

// `main` is replaced by `init` in Linda
#define main init 
#endif
