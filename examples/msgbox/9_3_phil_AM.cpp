#include <process.h>
#include <msgbox.h>
#define N 5
#define LEFT(n) (((n) + N - 1) % N)
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

void test(int id, bool isAvail[], bool isHungry[]) {
    msg_t snd;
    if (isHungry[id] && isAvail[id] && isAvail[RIGHT(id)]) {
        isHungry[id] = false;
        isAvail[id] = false;
        isAvail[RIGHT(id)] = false;
        mbx_put(snd, privp[id]);
    }
}

void server() {
    int st;
    msg_t rcv, snd;
    bool isAvail[N] = { true, true, true, true, true };
    bool isHungry[N] = { false, false, false, false, false};
    while (true) {
        mbx_get(rcv, privs, INF, &st);
        int id = rcv.m_id;
        int action = rcv.m_action;
        switch (action) {
            case ACQUIRE_FORKS: {
                isHungry[id] = true;
                test(id, isAvail, isHungry);
            } break;
            case RELEASE_FORKS: {
                isAvail[id] = true;
                isAvail[RIGHT(id)] = true;
                mbx_put(snd, privp[id]);
                test(LEFT(id), isAvail, isHungry);
                test(RIGHT(id), isAvail, isHungry);
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
