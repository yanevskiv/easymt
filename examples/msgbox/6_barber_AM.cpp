#include <process.h>
#include <msgbox.h>
#include <cstdlib>
#include <queue>
#define NC 15
#define NB 1
#define MAX_SEATS 5

enum {
    INVALID = -1,
    BARB_ENTER,
    CUST_ENTER
};

struct msg_t {
    int m_id;
    int m_action;
};

mbx_t privb[NB];
mbx_t privc[NC];
mbx_t privs;

void server() {
    int st;
    msg_t snd, rcv;

    std::queue<int> qBarbers, qCustomers;
    while (true) {
        msg_get(rcv, privs, INF, &st);
        int id = rcv.m_id;
        int action = rcv.m_action;
        switch (action) {
            case CUST_ENTER: {
                if (qCustomers.size() >= MAX_SEATS) {
                    snd.m_id = INVALID;
                    msg_put(snd, privc[id]);
                } else if (qCustomers.empty() && qBarbers.size() > 0)  {
                    int barbId = qBarbers.front();
                    qBarbers.pop();
                    snd.m_id = id;
                    msg_put(snd, privb[barbId]);
                    snd.m_id = barbId;
                    msg_put(snd, privc[id]);
                } else {
                    qCustomers.push(id);
                }
            } break;
            case BARB_ENTER: {
                if (qBarbers.empty() && qCustomers.size() > 0) {
                    int custId = qCustomers.front();
                    qCustomers.pop();
                    snd.m_id = custId;
                    msg_put(snd, privb[id]);
                    snd.m_id = id;
                    msg_put(snd, privc[custId]);
                } else {
                    qBarbers.push(id);
                }
            } break;
        }
    }
} 

void barber(int id) {
    int custId = INVALID;
    int st;
    msg_t snd, rcv;
    while (true) {
        snd.m_id = id;
        snd.m_action = BARB_ENTER;
        mbx_put(snd, privs);
        mbx_get(rcv, privb[id], INF, &st);
        custId = rcv.m_id;
        println(RED "I'm barber %d and cutting hair of %d", id, custId);
    }
}

void customer(int id) {
    int barbId = INVALID;
    int st;
    msg_t snd, rcv;
    while (true) {
        snd.m_id = id;
        snd.m_action = CUST_ENTER;
        mbx_put(snd, privs);
        mbx_get(rcv, privc[id], INF, &st);
        barbId = rcv.m_id;
        if (barbId != INVALID) {
            /* CUT HAIR */
            println(YELLOW "I'm customer %d and getting haircut from %d", id, barbId);
        }
    }
}


int main() {
    Process pc[NC], pb[NB], ps;
    ps = createProcess(server);
    for (int i = 0; i < NC; i++) 
        pc[i] = createProcess(customer, i);
    for (int i = 0; i < NB; i++) 
        pb[i] = createProcess(barber, i);

    for (int i = 0; i < NC; i++) 
        waitFor(pc[i]);
    for (int i = 0; i < NB; i++) 
        waitFor(pb[i]);
    waitFor(ps);
    return 0;
}
