#include <process.h>
#include <msgbox.h>
#include <cstdlib>
#include <queue>
#define NW 15
#define NR 5
#define MAX_SEATS 5

enum {
    READER,
    WRITER,
    START_READING,
    STOP_READING,
    START_WRITING,
    STOP_WRITING
};

struct msg_t {
    int m_id;
    int m_action;
};

mbx_t privw[NW];
mbx_t privr[NR];
mbx_t privs;

void server() {
    int st;
    msg_t snd, rcv;
    int wCount = 0;
    int rCount = 0;
    typedef std::pair<int, int> PAIR;
    std::queue<PAIR> qDelay;
    while (true) {
        msg_get(rcv, privs, INF, &st);
        int id = rcv.m_id;
        int action = rcv.m_action;
        // WAIT() / DELAY()
        switch (action) {
            case START_READING: {
                if (wCount == 0 && qDelay.empty()) {
                    rCount += 1;
                    msg_put(snd, privr[id]);
                } else {
                    qDelay.push(PAIR(id, READER));
                }
            } break;
            case START_WRITING: {
                if (wCount == 0 && rCount == 0 && qDelay.empty()) {
                    wCount += 1;
                    msg_put(snd, privw[id]);
                } else {
                    qDelay.push(PAIR(id, WRITER));
                }
            } break;
            case STOP_READING: {
                rCount -= 1;
                msg_put(snd, privr[id]);
            } break;
            case STOP_WRITING: {
                wCount -= 1;
                msg_put(snd, privw[id]);
            } break;
        }
        // SIGNAL();
        while (! qDelay.empty()) {
            PAIR p = qDelay.front();
            int pId = p.first;
            int pType = p.second;
            if (pType == WRITER && wCount == 0 && rCount == 0) {
                qDelay.pop();
                wCount += 1;
                msg_put(snd, privw[pId]);
            } else if (pType == READER && wCount == 0) {
                qDelay.pop();
                rCount += 1;
                msg_put(snd, privr[pId]);
            } else break;
        }
    }
} 

void reader(int id) {
    int st;
    msg_t snd, rcv;
    while (true) {
        snd.m_id = id;
        snd.m_action = START_READING;
        msg_put(snd, privs);
        msg_get(rcv, privr[id], INF, &st);
        
        print(GREEN "r");
        usleep(10);

        snd.m_id = id;
        snd.m_action = STOP_READING;
        msg_put(snd, privs);
        msg_get(rcv, privr[id], INF, &st);
        usleep(10);
    }
}

void writer(int id) {
    int st;
    msg_t snd, rcv;
    while (true) {
        snd.m_id = id;
        snd.m_action = START_WRITING;
        msg_put(snd, privs);
        msg_get(rcv, privw[id], INF, &st);
        
        print(RED "w");
        usleep(10);

        snd.m_id = id;
        snd.m_action = STOP_WRITING;
        msg_put(snd, privs);
        msg_get(rcv, privw[id], INF, &st);
        usleep(10);
    }
}


int main() {
    Process pw[NW], pr[NR], ps;
    ps = createProcess(server);
    for (int i = 0; i < NW; i++) 
        pw[i] = createProcess(writer, i);
    for (int i = 0; i < NR; i++) 
        pr[i] = createProcess(reader, i);
    waitFor(ps);
    for (int i = 0; i < NW; i++) 
        waitFor(pw[i]);
    for (int i = 0; i < NR; i++) 
        waitFor(pr[i]);
    return 0;
}
