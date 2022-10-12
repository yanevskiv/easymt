// Copyright (c) Ivan Janevski
#include <thread.h>
#define N 5

volatile bool lock = false;

void thread(int id)
{
    while (true) {
        /* LOCK */
        while (test_and_set(&lock)) 
            skip();

        /* CRITICAL SECTION */
        print("\e[7m\e[%dm(---", 31 + id);
        uwork(500);
        print("\e[7m\e[%dm---)", 31 + id);

        /* UNLOCK  */
        lock = false;

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
