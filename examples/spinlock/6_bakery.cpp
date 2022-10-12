// Copyright (c) Ivan Janevski
#include <thread.h>
#define N 5

volatile int turn[N];

void thread(int id)
{
    while (true) {
        /* LOCK */
        turn[id] = 1;
        int max = 0;
        for (int otherId = 0; otherId < N; otherId++)  {
            if (id == otherId)
                continue;
            if (turn[otherId] > max)  
                max = turn[otherId];
        }
        turn[id] = max + 1;
        for (int otherId = 0; otherId < N; otherId++)  {
            if (id == otherId)
                continue;
            while (turn[otherId] < turn[id] || (turn[otherId] == turn[id] && otherId < id))
                skip();
        }

        /* CRITICAL SECTION */
        print("\e[7m\e[%dm(---", 30 + id);
        uwork(500);
        print("\e[7m\e[%dm---)", 30 + id);

        /* UNLOCK  */
        turn[id] = 0;

        /* NON-CRITICAL SECTION */
        usleep(50);
    }
}

int main() {
    Thread ta[N];
    for (int i = 0; i < N; i++) 
        ta[i] = createThread(thread, i);
    for (int i = 0; i < N; i++) 
        join(ta[i]);
    return 0;
}
