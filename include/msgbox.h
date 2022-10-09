#ifndef _MSGBOX_H_
#define _MSGBOX_H_
// aliases
#define msg_put mbx_put
#define msg_get mbx_get

// wait for infinite amount of time on `msg_get`
#define INF -1

// reveived message
#define SUCCESS 1

// waiting for message
#define UNKNOWN 0

// timeout on `msg_get`
#define EXPIRED -1

struct msg_t;
struct mbx_t {
    // unique mbx id
    int m_id;

    // constructor (assigns random id)
    mbx_t();

    // constructor (assigns specific id)
    mbx_t(int);

    // constructor (creates id by name hashing)
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

// `struct msg_t` is supposed defined by the user but has an incomplete type 
// during implementation of `_mbx_put` and `_mbx_get`; the following is a way
// to overcome this and still have `pass-by-reference` interface.
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
