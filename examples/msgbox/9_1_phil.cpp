#include <process.h>
#include <msgbox.h>
#define N 5
#define LEFT(n) ((n) == 0 ? N - 1 : (n) - 1)
#define RIGHT(n) (((n) + 1) % N)

struct msg_t {
    // empty
};

mbx_t forks[N];

void phil(int id) {
    int st;
    msg_t empty;
    int left = LEFT(id);
    int right = RIGHT(id);
    while (true) {
        // Think
        println("Philosopher %d is thinking", id);
        usleep(10);
        
        // Acquire forks
        if (id % 2 == 0) {
            mbx_get(empty, forks[left], INF, &st);
            mbx_get(empty, forks[right], INF, &st);
        } else {
            mbx_get(empty, forks[right], INF, &st);
            mbx_get(empty, forks[left], INF, &st);
        }

        // Eat
        println("Philosopher %d is eating", id);
        usleep(10);

        // Release forks
        mbx_put(empty, forks[left]);
        mbx_put(empty, forks[right]);
    }
}

int main() {
    msg_t empty;
    for (int i = 0; i < N; i++) 
        msg_put(empty, forks[i]);
    Process tp[N];
    for (int i = 0; i < N; i++) 
        tp[i] = createProcess(phil, i);
    for (int i = 0; i < N; i++) 
        waitFor(tp[i]);
    return 0;
}
