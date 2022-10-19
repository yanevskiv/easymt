#include <process.h>
#include <msgbox.h>
#include <cstdlib>

enum {
    MATCHES,
    PAPER,
    TOBACCO
};

struct msg_t {
    int m_item1;
    int m_item2; 
};

mbx_t boxMatches, boxPaper, boxTobacco, boxOk;

void agent() {
    int st;
    msg_t snd, rcv;
    while (true) {
        int item = rand() % 3;
        switch (item) {
            case MATCHES: {
                println(RED "Agnet puts PAPER and TOBACCO");
                snd.m_item1 = PAPER;
                snd.m_item2 = TOBACCO;
                mbx_put(snd, boxMatches);
                mbx_get(rcv, boxOk, INF, &st);
            } break;
            case PAPER: {
                println(YELLOW "Agnet puts TOBACCO and MATCHES");
                snd.m_item1 = TOBACCO;
                snd.m_item2 = MATCHES;
                mbx_put(snd, boxPaper);
                mbx_get(rcv, boxOk, INF, &st);
            } break;
            case TOBACCO: {
                println(GREEN "Agnet puts MATCHES and PAPER");
                snd.m_item1 = MATCHES;
                snd.m_item2 = PAPER;
                mbx_put(snd, boxTobacco);
                mbx_get(rcv, boxOk, INF, &st);
            } break;
        }
    }
}

void smoker(int item) {
    int st;
    msg_t snd, rcv;
    int item1 = -1;
    int item2 = -1;
    while (true) {
        switch (item) {
            case MATCHES: {
                mbx_get(rcv, boxMatches, INF, &st);
                println(RED "Smoker gets PAPER and TOBACCO");
                item1 = rcv.m_item1;
                item2 = rcv.m_item2;
                mbx_put(snd, boxOk);
            } break;
            case PAPER: {
                mbx_get(rcv, boxPaper, INF, &st);
                println(YELLOW "Smoker gets TOBACCO and MATCHES");
                item1 = rcv.m_item1;
                item2 = rcv.m_item2;
                mbx_put(snd, boxOk);
            } break;
            case TOBACCO: {
                mbx_get(rcv, boxTobacco, INF, &st);
                println(GREEN "Smoker gets MATCHES and PAPER");
                item1 = rcv.m_item1;
                item2 = rcv.m_item2;
                mbx_put(snd, boxOk);
            } break;
        }
    }
}

int main() {
    Process ta = createProcess(agent);
    Process ts1 = createProcess(smoker, MATCHES);
    Process ts2 = createProcess(smoker, PAPER);
    Process ts3 = createProcess(smoker, TOBACCO);
    waitFor(ta);
    waitFor(ts1);
    waitFor(ts2);
    waitFor(ts3);
    return 0;
}
