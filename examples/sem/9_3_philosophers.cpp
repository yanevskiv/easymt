#include <thread.h>
#include <sem.h>
#define N 5
#define LEFT(id) (((id) + 1) %  N)
#define RIGHT(id) (((id) == 0) ? (N - 1) : ((id) - 1))
#define CAN_EAT(id) (available[LEFT(id)] && available[RIGHT(id)])

Sem mutex = 1;
Sem privs[] = { 0, 0, 0, 0, 0 };
bool available[] = { true, true, true, true, true };
bool hungry[] = { false, false, false, false, false };

void phil(int id) 
{
    int left = LEFT(id);
    int right = RIGHT(id);
    while (true) {
        /* THINK */
        print(" %d ", id);
        /* ACQUIRE FORKS */
        wait(mutex);
        if (! CAN_EAT(id)) {
            hungry[id] = true;
            signal(mutex);
            wait(privs[id]);
        }
        hungry[id] = false;
        available[left] = false;
        available[right] = false;
        signal(mutex);
        /* EAT */
        print(REV " %d ", id);
        /* RELEASE FORKS */
        wait(mutex);
        available[left] = true;
        available[right] = true;
        if (hungry[left] && CAN_EAT(left)) {
            signal(privs[left]);
        } else if (hungry[right] && CAN_EAT(right)) {
            signal(privs[right]);
        } else {
            signal(mutex);
        }
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
