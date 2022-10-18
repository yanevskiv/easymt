#include <process.h>
#include <msgbox.h>

struct msg_t {
    // empty 
};

mbx_t ma, mb;

void a() {
    int st;
    msg_t msg;
    while (true) {
        mbx_get(msg, ma, INF, &st);
        print(GREEN "a");
        mbx_put(msg, mb);
    }
}

void b() {
    int st;
    msg_t msg;
    while (true) {
        mbx_get(msg, mb, INF, &st);
        print(RED "a");
        mbx_put(msg, ma);
    }
}

int main() {
    msg_t msg;
    msg_put(msg, ma);
    Process ta = createProcess(a);
    Process tb = createProcess(b);
    waitFor(ta);
    waitFor(tb);
    return 0;
}
