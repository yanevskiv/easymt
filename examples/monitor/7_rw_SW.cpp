// (c) Ivan Janevski
#include <thread.h>
#include <monitor.h>
#include <queue>
#define NR 10
#define NW 5

enum {
    READER = 0,
    WRITER = 1
};
class RW : SW_Monitor {
    int m_readerCount = 0;
    int m_writerCount = 0;
    std::queue<int> m_delayType;
    cond m_delay;
public:
    RW() : m_delay(this) {

    }
    void lock_reading() {
        monitor {
            if (m_writerCount > 0) {
                m_delayType.push(READER);
                m_delay.wait();
            }
            m_readerCount += 1;
            if (m_delay.queue() && m_delayType.front() == READER)  {
                m_delayType.pop();
                m_delay.signal();
            }
        }
    }
    void lock_writing() {
        monitor {
            if (m_writerCount > 0 || m_readerCount > 0) {
                m_delayType.push(WRITER);
                m_delay.wait();
            }
            m_writerCount += 1;
        }
    }
    void unlock() {
        monitor {
            if (m_writerCount > 0) {
                m_writerCount -= 1;
            } else if (m_readerCount > 0) {
                m_readerCount -= 1;
            }
            if (m_delay.queue()) {
                int shouldUndelay = (
                    (m_delayType.front() == READER && m_writerCount == 0)
                 || (m_delayType.front() == WRITER && m_writerCount == 0 && m_readerCount));
                if (shouldUndelay) {
                    m_delayType.pop();
                    m_delay.signal();
                }
            }
        }
    }
} rw;

void reader(int id) {
    while (true) {
        rw.lock_reading();
        print(GREEN "r");
        rw.unlock();
    }
}

void writer(int id) {
    while (true) {
        rw.lock_writing();
        print(RED "w");
        rw.unlock();
    }
}

int main() {
    Thread tw[NW], tr[NR];
    for (int i = 0; i < NR; i++) 
        tr[i] = createThread(reader, i);
    for (int i = 0; i < NW; i++) 
        tw[i] = createThread(writer, i);
    for (int i = 0; i < NR; i++) 
        join(tr[i]);
    for (int i = 0; i < NW; i++) 
        join(tw[i]);
    return 0;
}
