#include <thread.h>
#include <stdlib.h>
#include <monitor.h>
#define N 5

class SJN : SC_Monitor {
    bool m_locked = false;
    cond m_delay;
public:
    SJN() : m_delay(this) {
        // empty
    }
    void enter_section(int prio) {
        monitor {
            if (m_locked || m_delay.queue()) 
                m_delay.wait(prio);
            m_locked = true;
        }
    }
    void exit_section() {
        monitor {
            m_locked = false;
            m_delay.signal();
        }
    }
} sjn;


void a(int id) {
    int prio = rand() % 1000; // nasumicni prioritet
    while (true) {
        sjn.enter_section(prio);
        print("%d", id);
        sjn.exit_section();
    }
}

int main() {
    Thread ta[N];
    for (int i = 0; i < N; i++) 
        ta[i] = createThread(a, i);
    for (int i = 0; i < N; i++) 
        join(ta[i]);
    return 0;
}
