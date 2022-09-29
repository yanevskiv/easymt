#ifndef _MSGBOX_H_
#define _MSGBOX_H_
#define msg_put mbx_put
#define msg_get mbx_get
#define INF -1
#define SUCCESS 1
#define UNKNOWN 0
#define EXPIRED -1

struct msg_t;
struct mbx_t {
    int m_id;
    mbx_t();
    mbx_t(int);
    mbx_t(const char*);
};

// put message in message box
void  _mbx_put(msg_t *msg, mbx_t mbx);

// get message from message box
void  _mbx_get(msg_t **msg, mbx_t mbx, int len = INF, int* st = nullptr);

// open message box with name
mbx_t mbx_open(const char *, ...);

// close message box
void  mbx_close(mbx_t&);

// `struct msg_t` is supposed defined by the user and has an incomplete type 
// to overcome this, this hack is there to emulate a `pass-by-reference`
#define mbx_put(msg, mbx) \
    _mbx_put(new msg_t{ (msg) }, (mbx))

#define mbx_get(msg, mbx, len, st) \
    do { \
        msg_t *_m_tmp; \
        _mbx_get(&_m_tmp, (mbx), (len), (st)); \
        if (_m_tmp) \
            (msg) = *_m_tmp; \
        delete _m_tmp; \
    } while (0)

#endif
