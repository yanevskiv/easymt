// (c) Ivan Janevski
#include <thread.h>
#include <monitor.h>
#define N 5
#define LEFT(id) ((id) == 0 ? N - 1 : (id) - 1)
#define RIGHT(id) (((id) + 1) % N)

class Table : SC_Monitor {
    bool m_forkFree[N] = { true, true, true, true, true };
    bool m_needFork[N] = { false, false, false, false, false};
    cond m_waitForFork[N];
public:
    Table() : m_waitForFork {this, this, this, this, this} {
        
    }
    void acquire_fork(int which) {
        monitor {
            if (! m_forkFree[which]) {
                m_needFork[which] = true;
                m_waitForFork[which].wait();
            } else {
                m_forkFree[which] = false;
            }
        }
    }

    void release_fork(int which) {
        monitor {
            m_forkFree[which] = true;
            if (m_needFork[which]) {
                m_needFork[which] = false;
                m_forkFree[which] = false;
                m_waitForFork[which].signal();
            }
        }
    }
} table;

void phil(int id) {
    int left = LEFT(id);
    int right = RIGHT(id);
    while (true) {
        // Think
        print(" x%d ", id);
        // Acquire forks
        if (id % 2 == 0) {
            table.acquire_fork(left);
            table.acquire_fork(right);
        } else {
            table.acquire_fork(right);
            table.acquire_fork(left);
        }
        // Eat
        print(" y%d ", id);
        // Release forks
        table.release_fork(left);
        table.release_fork(right);
    }
}

int main() {
    Thread tp[N];
    for (int i = 0; i < N; i++) 
        tp[i] = createThread(phil, i);
    for (int i = 0; i < N; i++) 
        join(tp[i]);
    return 0;
}
