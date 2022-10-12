// Copyright (c) Ivan Janevski
#include <thread.h>
#include <sem.h>

Sem sa = 1, sb = 0;

void a() {
    while (true) {
        wait(sa);
        print(RED "a");
		signal(sb);
    }
}

void b() {
    while (true) {
        wait(sb);
        print(GREEN "b");
		signal(sa);
    }
}


int main() {
    Thread ta = createThread(a);
    Thread tb = createThread(b);
    join(ta);
    join(tb);
    return 0;
}

