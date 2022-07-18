#ifndef _MSGBOX_H_
#define _MSGBOX_H_

#define INF -1
#define msg_put mbx_put
#define msg_get mbx_get
#define SUCCESS 0
#define FAILURE -1
#define EXPIRED -2
#include <vector>

struct msg_t;
struct mbx_t {
    static int globalId;
    int m_id;
    mbx_t();
    ~mbx_t();
};

void  _mbx_put(msg_t *msg, mbx_t mbx);
void  _mbx_get(msg_t *msg, mbx_t mbx, int duration, int* status = 0);
mbx_t mbx_open(const char *, ...);
void  mbx_close(mbx_t);
#define mbx_put(msg, mbx) \
    _mbx_put(new msg_t{ (msg) }, (mbx))
#define mbx_get(msg, mbx, len, st) \
    _mbx_get(&(msg), (mbx), (len), (st))


#endif
