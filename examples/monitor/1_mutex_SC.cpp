#include <thread.h>
#include <monitor.h>

class CriticalSection : SC_Monitor {
    bool m_locked = false;
    cond m_delay;
public:
    CriticalSection() : m_delay(this) {

    }
    void enter_section() {
        monitor {
            while (m_locked) 
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
        print(REV RED "---)");
        cs.exit_section();
    }
}

void b() {
    while (true) {
        cs.enter_section();
        print(GREEN "(---");
        print(GREEN "---)");
        cs.exit_section();
    }
}

int main() {
    Thread ta = createThread(a);
    Thread tb = createThread(b);
    join(ta);
    join(tb);
    return 0;
}
