// Copyright (c) Ivan Janevski
#include <thread.h>
#define N 5

volatile int inLevel[N] = { 0 };
volatile int lastId[N] = { 0 };

void thread(int id)
{
    while (true) {
        /* LOCK */
        for (int level = 0; level < N - 1; level++) {
            inLevel[id] = level;
            lastId[level] = id;
            for (int otherId = 0; otherId < N; otherId++) {
                if (id == otherId)
                    continue;
                while (inLevel[id] < inLevel[otherId] && lastId[level] == id)
                    skip();
            }
        }


        /* CRITICAL SECTION */
        print("\e[7m\e[%dm(---", 30 + id);
        uwork(500);
        print("\e[7m\e[%dm---)", 30 + id);

        /* UNLOCK  */
        inLevel[id] = 0;

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
