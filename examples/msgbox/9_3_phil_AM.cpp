#include <process.h>
#include <msgbox.h>
#define N 5
#define LEFT(n) ((n) == 0 ? N - 1 : (n) - 1)
#define RIGHT(n) (((n) + 1) % N)

enum {
    ACQUIRE_FORKS,
    RELEASE_FORKS
};

struct msg_t {
    int m_id;
    int m_action;
};


mbx_t privp[N];
mbx_t privs;


void server() {
    int st;
    msg_t rcv, snd;
    bool isAvail[N] = { true, true, true, true, true };
    bool isHungry[N] = { false, false, false, false, false};
    while (true) {
        mbx_get(rcv, privs, INF, &st);
        int id = rcv.m_id;
        int action = rcv.m_action;
        int left = LEFT(id);
        int right = RIGHT(id);
        switch (action) {
            case ACQUIRE_FORKS: {
                if (isAvail[left] && isAvail[right]) {
                    isAvail[left] = false;
                    isAvail[right] = false;
                    mbx_put(snd, privp[id]);
                } else {
                    isHungry[id] = true;
                }
            } break;
            case RELEASE_FORKS: {
                isAvail[left] = true;
                isAvail[right] = true;
                if (isHungry[left] && isAvail[LEFT(left)] && isAvail[RIGHT(left)]) {
                    isHungry[left] = false;
                    isAvail[LEFT(left)] = false;
                    isAvail[RIGHT(left)] = false;
                    mbx_put(snd, privp[left]);
                } else if (isHungry[right] && isAvail[LEFT(right)] && isAvail[RIGHT(right)]) {
                    isHungry[right] = false;
                    isAvail[LEFT(right)] = false;
                    isAvail[RIGHT(right)] = false;
                    mbx_put(snd, privp[right]);
                }
                mbx_put(snd, privp[id]);
            } break;
        }
    }
}

void phil(int id) {
    int st;
    msg_t snd, rcv;
    while (true) {
        // Think
        println("Philosopher %d is thinking", id);
        usleep(10);
        
        // Acquire forks
        snd.m_id = id;
        snd.m_action = ACQUIRE_FORKS;
        mbx_put(snd, privs);
        mbx_get(rcv, privp[id], INF, &st);

        // Eat
        println("Philosopher %d is eating", id);
        usleep(10);

        // Release forks
        snd.m_id = id;
        snd.m_action = RELEASE_FORKS;
        mbx_put(snd, privs);
        mbx_get(rcv, privp[id], INF, &st);
    }
}

int main() {
    Process tp[N], ts;
    ts = createProcess(server);
    for (int i = 0; i < N; i++) 
        tp[i] = createProcess(phil, i);
    waitFor(ts);
    for (int i = 0; i < N; i++) 
        waitFor(tp[i]);
    return 0;
}
