/**
 * (c) Ivan Janevski
 */
#include <process.h>
#include <clinda.h>
#include <queue>
enum {
    ENTER,
    EXIT,
    OK
};

enum {
    ID_A,
    ID_B
};

void server() {
    bool locked = false;
    std::queue<int> qDelay;
    while (true) {
        int id, action;
        in("request", &id, &action);
        switch (action) {
            case ENTER: {
                if (! locked)  {
                    locked = true;
                    out("response", id, OK);
                } else {
                    qDelay.push(id);
                }
            } break;
            case EXIT: {
                locked = false;
                if (! qDelay.empty()) {
                    locked = true;
                    int otherId = qDelay.front();
                    qDelay.pop();
                    out("response", otherId, OK);
                }
                out("response", id, OK);
            } break;
        }
    }
}
void a() {
    while (true) {
        out("request", ID_A, ENTER);
        in("response", ID_A, OK);
        print(GREEN "(---");
        uwork(100);
        print(GREEN "---)");
        out("request", ID_A, EXIT);
        in("response", ID_A, OK);
        usleep(10);
    }
}
void b() {
    while (true) {
        out("request", ID_B, ENTER);
        in("response", ID_B, OK);
        print(REV RED "(---");
        uwork(10);
        print(REV RED "---)");
        out("request", ID_B, EXIT);
        in("response", ID_B, OK);
        usleep(10);
    }
}
void init() {
    eval(a());
    eval(b());
    eval(server());
}
