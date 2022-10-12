// Copyright (c) Ivan Janevski
#include <thread.h>
#include <region.h>
struct CritcalSection : Region {
    int lock = false;
} cs;

void a() {
    while (true) {
        region (cs) {
            await(cs.lock == false);
            cs.lock = true;
        }
        print(REV RED "(---");
        uwork(500);
        print(REV RED "---)");
        region (cs) {
            cs.lock = false;
        }
        usleep(10);
    }
}

void b() {
    while (true) {
        region (cs) {
            await(cs.lock == false);
            cs.lock = true;
        }
        print(GREEN "(---");
        uwork(500);
        print(GREEN "---)");
		region (cs) {
            cs.lock = false;
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
