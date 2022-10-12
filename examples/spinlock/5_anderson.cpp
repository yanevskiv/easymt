// Copyright (c) Ivan Janevski
#include <thread.h>
#define N 5

volatile bool flag[N] = { true };
volatile int slot = 0;

void thread(int id)
{
    while (true) {
        /* LOCK */
		int mySlot = fetch_and_add(&slot, 1) % N;
        while (flag[mySlot] == false)
            skip();

        /* CRITICAL SECTION */
        print("\e[7m\e[%dm(---", 30 + id);
        uwork(500);
        print("\e[7m\e[%dm---)", 30 + id);

        /* UNLOCK  */
        flag[mySlot] = false;
        flag[(mySlot + 1) % N] = true;

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
