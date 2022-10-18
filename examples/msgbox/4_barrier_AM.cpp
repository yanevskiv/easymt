#include <process.h>
#include <msgbox.h>
#include <cstdlib>
#include <map>
#include <queue>
#define N 5

enum {
    BAR_1,
    BAR_2
};

struct msg_t {
    int m_id;
    int m_bar;
};

mbx_t privm[N];
mbx_t privs;

void server() {
    int st;
    msg_t snd, rcv;
    std::map<int, std::queue<int>> qBarriers;
    while (true) {
        msg_get(rcv, privs, INF, &st);
        int id = rcv.m_id;
        int bar = rcv.m_bar;
        qBarriers[bar].push(id);
        if (qBarriers[bar].size() == N) {
            while (! qBarriers[bar].empty()) {
                int pId = qBarriers[bar].front();
                qBarriers[bar].pop();
                mbx_put(snd, privm[pId]);
            }
        }
    }
} 

void a(int id) {
    int st;
    msg_t snd, rcv;
    while (true) {
        print("\e[%dm %d ", 31 + id, id);
        usleep(10);
        snd.m_id = id;
        snd.m_bar = BAR_1;
        mbx_put(snd, privs);
        mbx_get(rcv, privm[id], INF, &st);

        print("\e[7m\e[%dm %d ", 31 + id, id);
        usleep(10);
        snd.m_id = id;
        snd.m_bar = BAR_2;
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
