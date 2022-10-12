// Copyright (c) Ivan Janevski
#include <thread.h>
#include <sem.h>
#define N 5

Sem door1 = 1, door2 = 0;
int count = 0;

void a(int id) 
{
    while (true) {
        wait(door1);
        /* SECTION 1 */
        print("\e[%dm %d ", 31 + id, id);
        count += 1;
        if (count == N) {
            count = 0;
            signal(door2);
        } else {
            signal(door1);
        }
        wait(door2);
        /* SECTION 2 */
        print(REV "\e[%dm %d ", 31 + id, id);
		count += 1;
        if (count == N) {
            count = 0;
            signal(door1);
        } else {
            signal(door2);
        }
    }
}

int main() {
    Thread t[N];
    for (int i = 0; i < N; i++) 
        t[i] = createThread(a, i);
    for (int i = 0; i < N; i++)
        join(t[i]);
    return 0;
}
