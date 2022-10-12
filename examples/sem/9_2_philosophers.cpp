#include <thread.h>
#include <sem.h>
#define N 5

Sem mutex = 1;
bool avail[] = { true, true, true, true, true };

void phil(int id) 
{
    int right = (id + 1) % N;
    int left = (id == 0) ? (N - 1) : (id - 1);
    while (true) {
        /* THINK */
        print(" %d ", id);
        /* ACQUIRE FORKS */
        while (true) {
            wait(mutex);
            if (avail[left] && avail[right])  {
                avail[left] = false;
                avail[right] = false;
                signal(mutex);
                break;
            } else {
                signal(mutex);
                usleep(50);
            }
        }
        /* EAT */
        print(REV " %d ", id);
        /* RELEASE FORKS */
        wait(mutex);
        avail[left] = true;
        avail[right] = true;
        signal(mutex);
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
