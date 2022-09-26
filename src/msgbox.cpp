#include <msgbox.h>
mbx_t::mbx_t() 
{
    
}
mbx_t::~mbx_t()
{

}

struct impl_mbx_t {
    int m_id;
    std::vector<msg_t*> m_msgs;
};
static std::vector<impl_mbx_t> mbx_vec;

mbx_t mbx_open(const char *fmt, ...) 
{
    return mbx_t{};
}

void _mbx_get(msg_t *msg, mbx_t mbx, int len, int *status) 
{
}

void _mbx_put(msg_t *msg, mbx_t mbx) 
{
}

void mbx_close(mbx_t )
{
}


