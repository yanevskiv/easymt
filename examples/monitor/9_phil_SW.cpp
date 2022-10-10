// (c) Ivan Janevski
#include <thread.h>
#include <monitor.h>
#define N 5
#define LEFT(id) ((id) == 0 ? N - 1 : (id) - 1)
#define RIGHT(id) (((id) + 1) % N)

class Table : SW_Monitor {
    bool m_forkFree[N] = { true, true, true, true, true };
    bool m_hungry[N] = { false, false, false, false, false };
    cond m_waitForForks[N];
public:
    Table() : m_waitForForks {this, this, this, this, this} {
        
    }
    void acquire_forks(int left, int right) {
        monitor {
            int id = RIGHT(left); // same as LEFT(right)
            if (! (m_forkFree[left] && m_forkFree[right])) {
                m_hungry[id] = true;
                m_waitForForks[id].wait();
            }
            m_hungry[id] = false;
            m_forkFree[left] = false;
            m_forkFree[right] = false;
        }
    }

    void release_forks(int left, int right) {
        monitor {
            m_forkFree[left] = true;
            m_forkFree[right] = true;
            if (m_hungry[left] && m_forkFree[LEFT(left)])  {
                m_waitForForks[left].signal();
            } else if (m_hungry[right] && m_forkFree[RIGHT(right)]) {
                m_waitForForks[right].signal();
            }
        }
    }
} table;

void phil(int id) {
    int left = LEFT(id);
    int right = RIGHT(id);
    while (true) {
        // Think
        print(" %d ", id);
        // Acquire forks
        table.acquire_forks(left, right);
        // Eat
        print(REV " %d ", id);
        // Release forks
        table.release_forks(left, right);
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
