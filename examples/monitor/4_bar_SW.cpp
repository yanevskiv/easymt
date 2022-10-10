// (c) Ivan Janevski
#include <thread.h>
#include <stdlib.h>
#include <monitor.h>
#define N 5
#define B N

class CriticalSection : SW_Monitor {
    bool m_locked = false;
    cond m_delay;
public:
    CriticalSection() : m_delay(this) {
        // empty
    }
    void start_section() {
        monitor {
            if (m_locked)
                m_delay.wait();
            m_locked = true;
        }
    }
    void end_section() {
        monitor {
            m_locked = false;
            m_delay.signal();
        }
    }
} cs;

class Barrier : SW_Monitor {
    int m_count = 0;
    cond m_barrier;
public:
    Barrier() : m_barrier(this) {
        // empty
    }
    void wait_barrier() {
        monitor {
            m_count += 1;
            if (m_count == B) {
                m_count = 0;
                m_barrier.signal();
            } else {
                m_barrier.wait();
                m_barrier.signal();
            }
        }
    }
} bar1, bar2;


void a(int id) {
    while (true) {
        cs.start_section();
        print("\e[%dm %d", 31 + id, id);
        cs.end_section();
        bar1.wait_barrier();

        cs.start_section();
        print(REV "\e[%dm %d", 31 + id, id);
        cs.end_section();
        bar2.wait_barrier();
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
