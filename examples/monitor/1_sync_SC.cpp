#include <thread.h>
#include <stdlib.h>
#include <monitor.h>
#define B 10

class AB : SC_Monitor {
    char ch = 'a';
    cond condA;
    cond condB;
public:
    AB() : condA(this), condB(this) {
        // empty
    }
    void wait_a() {
        monitor {
            while (ch != 'a')
                condA.wait();
        }
    }
    void wait_b() {
        monitor {
            while (ch != 'b')
                condB.wait();
        }
    }
    void signal_a() {
        monitor {
            ch = 'a';
            condA.signal();
        }
    }
    void signal_b() {
        monitor {
            ch = 'b';
            condB.signal();
        }
    }
} ab;


void a() {
    while (true) {
        ab.wait_a();
        print("A");
        ab.signal_b();
        usleep(10);
    }
}

void b() {
    while (true) {
        ab.wait_b();
        print("B");
        ab.signal_a();
        usleep(10);
    }
}

int main() {
    Thread ta = createThread(a);
    Thread tb = createThread(b);
    join(ta);
    join(tb);
    return 0;
}
