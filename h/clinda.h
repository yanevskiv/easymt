#ifndef _CLINDA_H_
#define _CLINDA_H_
#include <functional>

enum {
    LT_NONE,
    LT_INT,
    LT_DOUBLE,
    LT_STRING
};

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

#define LTP_MAX 6
struct LTP {
    LT m_data[LTP_MAX];
    LTP(LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});
    void assign(const LTP& ltp);
    bool equals(const LTP& ltp);
};


bool inp(LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});
void in (LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});
void out(LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});
void rd (LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});
bool rdp(LT a = {}, LT b = {}, LT c = {}, LT d = {}, LT e = {}, LT f = {});
void show();

void add_eval(std::function<void()>*);
void add_eval(std::function<int()>);
void add_eval(std::function<double()>);
void add_eval(std::function<const char *()>);

#define eval(CODE) \
    do {\
        std::function<void()> *_f = new std::function<void()>([=](){ \
           CODE; \
        }); \
        add_eval(_f);\
    } while (0)

#endif
