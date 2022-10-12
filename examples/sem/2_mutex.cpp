// Copyright (c) Ivan Janevski
#include <thread.h>
#include <sem.h>

Sem mutex = 1;

void a() {
    while (true) {
        wait(mutex);
        print(REV RED "(---");
        uwork(500);
        print(REV RED "---)");
		signal(mutex);
        usleep(50);
    }
}

void b() {
    while (true) {
        wait(mutex);
        print(GREEN "(---");
        uwork(500);
        print(GREEN "---)");
		signal(mutex);
        usleep(50);
    }
}

int main() {
    Thread ta = createThread(a);
    Thread tb = createThread(b);
    join(ta);
    join(tb);
    return 0;
}
