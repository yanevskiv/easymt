#include <process.h>
#include <msgbox.h>

struct msg_t {
    // empty 
};

mbx_t mutex;

void a() {
    int st;
    msg_t msg;
    while (true) {
        mbx_get(msg, mutex, INF, &st);
        print(GREEN "(---");
        usleep(10);
        print(GREEN "---)");
        mbx_put(msg, mutex);
        usleep(100);
    }
}

void b() {
    int st;
    msg_t msg;
    while (true) {
        mbx_get(msg, mutex, INF, &st);
        print(REV RED "(---");
        usleep(10);
        print(REV RED "---)");
        mbx_put(msg, mutex);
        usleep(10);
    }
}

int main() {
    msg_t msg;
    msg_put(msg, mutex);
    Process ta = createProcess(a);
    Process tb = createProcess(b);
    waitFor(ta);
    waitFor(tb);
    return 0;
}
