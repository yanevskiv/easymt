// Copyright (c) Ivan Janevski
#include <thread.h>
#include <region.h>
struct CritcalSection : Region {
    // empty
} cs;

void a() {
    while (true) {
        region (cs) {
            print(REV RED "(---");
            uwork(500);
            print(REV RED "---)");
        }
        usleep(10);
    }
}

void b() {
    while (true) {
        region (cs) {
            print(GREEN "(---");
            uwork(500);
            print(GREEN "---)");
        }
        usleep(10);
    }
}
int main() {
    Thread ta = createThread(a);
    Thread tb = createThread(b);
    join(ta);
    join(tb);
    return 0;
}
