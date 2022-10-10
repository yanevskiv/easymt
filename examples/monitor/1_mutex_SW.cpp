// (c) Ivan Janevski
#include <thread.h>
#include <monitor.h>

class CriticalSection : SW_Monitor {
    bool m_locked = false;
    cond m_delay;
public:
    CriticalSection() : m_delay(this) {

    }
    void enter_section() {
        monitor {
            if (m_locked) 
                m_delay.wait();
            m_locked = true;
        }
    }

    void exit_section() {
        monitor {
            m_locked = false;
            m_delay.signal();
        }
    }

} cs;


void a() {
    while (true) {
        cs.enter_section();
        print(REV RED "(---");
        usleep(10);
        print(REV RED "---)");
        cs.exit_section();
        usleep(100);
    }
}

void b() {
    while (true) {
        cs.enter_section();
        print(GREEN "(---");
        usleep(10);
        print(GREEN "---)");
        cs.exit_section();
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
