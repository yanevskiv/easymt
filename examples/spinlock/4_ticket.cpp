// Copyright (c) Ivan Janevski
#include <thread.h>
#define N 5

volatile int ticket = 0;
volatile int next = 0;

void thread(int id)
{
    while (true) {
        /* LOCK */
        int number = fetch_and_add(&ticket, 1);
        while (number != next)
            skip();

        /* CRITICAL SECTION */
        print("\e[7m\e[%dm(---", 30 + id);
        uwork(500);
        print("\e[7m\e[%dm---)", 30 + id);

        /* UNLOCK  */
        next += 1;

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
