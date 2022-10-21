/**
 * (c) Ivan Janevski
 */
#include <thread.h>
#include <clinda.h>
#include <queue>
#define N 5
enum {
    ENTER,
    EXIT
};

void server() {
    typedef std::pair<int, int> PAIR;
    std::priority_queue<PAIR, std::vector<PAIR>, std::greater<PAIR>> qEnterDelay;
    bool locked = false;
    while (true) {
        int id, action, prio;
        in("request", &id, &action, &prio);
        switch (action) {
            case ENTER: {
                if (! locked) {
                    locked = true;
                    out("response", id);
                } else {
                    qEnterDelay.push(PAIR(prio, id));
                }
            } break;
            case EXIT: {
                locked = false;
                if (! qEnterDelay.empty()) {
                    locked = true;
                    PAIR p = qEnterDelay.top();
                    int qId = p.second;
                    qEnterDelay.pop();
                    out("response", qId);
                }
                out("response", id);
            } break;
        }
    }
}

void a(int id) {
    int prio = id;
    while (true) {
        out("request", id, ENTER, prio);
        in("response", id);
        print("\e[%dm %d ", 31 + id, id);

        out("request", id, EXIT, 0);
        in("response", id);
        print("\e[7m\e[%dm %d ", 31 + id, id);
    }
}


int main() {
    eval(server());
    for (int i = 0; i < N; i++) 
        eval(a(i));
    return 0;
}
