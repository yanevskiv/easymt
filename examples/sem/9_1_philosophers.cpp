#include <thread.h>
#include <sem.h>
#define N 5

Sem forks[N] = { 1, 1, 1, 1, 1 };

void phil(int id) 
{
    int right = (id + 1) % N;
    int left = (id == 0) ? (N - 1) : (id - 1);
    while (true) {
        /* THINK */
        print(" %d ", id);
        /* ACQUIRE FORKS */
        if (id % 2 == 0) {
            wait(forks[left]);
            wait(forks[right]);
        } else {
            wait(forks[right]);
            wait(forks[left]);
        }
        /* EAT */
        print(REV " %d ", id);
        /* RELEASE FORKS */
        signal(forks[left]);
        signal(forks[right]);
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
