#include <process.h>
#include <msgbox.h>
#include <cstdlib>
#include <queue>
#define N 5

enum {
    ENTER,
    EXIT
};

struct msg_t {
    int m_id;
    int m_prio;
    int m_action;
};

mbx_t privm[N];
mbx_t privs;

void server() {
    int st;
    msg_t snd, rcv;
    typedef std::pair<int, int> PAIR;

    bool locked = false;
    std::priority_queue<PAIR, std::vector<PAIR>, std::greater<PAIR>> qDelay;
    while (true) {
        msg_get(rcv, privs, INF, &st);
        int id = rcv.m_id;
        int prio = rcv.m_prio;
        int action = rcv.m_action;
        switch (action) {
            case ENTER: {
                if (! locked) {
                    locked = true;
                    msg_put(snd, privm[id]);
                } else {
                    qDelay.push(PAIR(prio, id));
                }
            } break;
            case EXIT: {
                locked = false;
                if (! qDelay.empty()) {
                    locked = true;
                    PAIR p = qDelay.top();
                    qDelay.pop();
                    int pPrio = p.first;
                    int pId = p.second;
                    msg_put(snd, privm[pId]);
                }
                msg_put(snd, privm[id]);
            } break;
        }
    }
} 

void a(int id) {
    int prio = id;
    int st;
    msg_t snd, rcv;
    while (true) {
        snd.m_id = id;
        snd.m_prio = prio;
        snd.m_action = ENTER;
        mbx_put(snd, privs);
        mbx_get(rcv, privm[id], INF, &st);

        print(REV "\e[%dm %d ", 31 + id, id);
        usleep(10);

        snd.m_id = id;
        snd.m_prio = prio;
        snd.m_action = EXIT;
        mbx_put(snd, privs);
        mbx_get(rcv, privm[id], INF, &st);
    }
}


int main() {
    Process pa[N], ps = createProcess(server);
    for (int i = 0; i < N; i++) 
        pa[i] = createProcess(a, i);
    waitFor(ps);
    for (int i = 0; i < N; i++) 
        waitFor(pa[i]);
    return 0;
}
