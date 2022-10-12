// Copyright (c) Ivan Janevski
#include <thread.h>
#define N 5

typedef bool Lock;

volatile Lock *clh_lock = new Lock { false };

void thread(int id)
{
    while (true) {
        /* LOCK */
        volatile Lock *new_lock = new Lock { true };
        volatile Lock *old_lock = get_and_set(&clh_lock, new_lock);
        while (*old_lock)
            skip();

        /* CRITICAL SECTION */
        print("\e[7m\e[%dm(---", 30 + id);
        uwork(500);
        print("\e[7m\e[%dm---)", 30 + id);

        /* UNLOCK  */
        *new_lock = false;

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
    delete clh_lock;
    return 0;
}
