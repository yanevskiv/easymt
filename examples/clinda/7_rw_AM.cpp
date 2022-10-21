/**
 * (c) Ivan Janevski
 */
#include <thread.h>
#include <print.h>
#include <clinda.h>
#include <queue>
#define NR 10
#define NW 5 

enum {
    READER,
    WRITER,

    START_READ,
    START_WRITE,
    END_READ,
    END_WRITE
};

void server() {
    typedef std::pair<int, int> PAIR;
    std::queue<PAIR> qDelay;
    int wCount = 0;
    int rCount = 0;
    while (true) {
        int action, id;
        in("request", &action, &id);

        // Try enter
        switch (action) {
            // Reader start
            case START_READ: {
                if (wCount == 0 && qDelay.empty())  {
                    rCount += 1;
                    out("response", id);
                } else {
                    qDelay.push(PAIR(id, READER));
                }
            } break;
            // Writer start
            case START_WRITE: {
                if (wCount == 0 && rCount == 0 && qDelay.empty()) {
                    wCount += 1;
                    out("response", id);
                } else {
                    qDelay.push(PAIR(id, WRITER));
                }
            } break;
            // Reader end
            case END_READ: {
                rCount -= 1;
                out("response", id);
            } break;
            // Writer end
            case END_WRITE: {
                wCount -= 1;
                out("response", id);
            } break;
        }
        // Try wake up delayed 
        while (! qDelay.empty()) {
            PAIR p = qDelay.front();
            int pId = p.first;
            int pType = p.second;
            if (pType == WRITER && wCount == 0 && rCount == 0) {
                qDelay.pop();
                wCount += 1;
                out("response", pId);
            } else if (pType == READER && wCount == 0)  {
                qDelay.pop();
                rCount += 1;
                out("response", pId);
            } else break;
        }
    }
}

void reader(int id) {
    usleep(21);
    while (true) {
        out("request", START_READ, id);
        in("response", id);
        print(GREEN "r");
        usleep(20);
        out("request", END_READ, id);
        in("response", id);
        usleep(21);
    }
}

void writer(int id) {
    usleep(2);
    while (true) {
        out("request", START_WRITE, id);
        in("response", id);
        print(RED "w");
        usleep(20);
        out("request", END_WRITE, id);
        in("response", id);
        usleep(2);
    }
}

int main() {
    eval(server());
    for (int i = 0; i < NR; i++) 
        eval(reader(i));
    for (int i = 0; i < NW; i++) 
        eval(writer(NR + i));
    return 0;
}
