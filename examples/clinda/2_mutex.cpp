#include <process.h>
#include <clinda.h>
void a() {
    while (true) {
        in("mutex");
        print(GREEN "(---");
        uwork(0);
        print(GREEN "---)");
        out("mutex");
        usleep(10);
    }
}
void b() {
    while (true) {
        in("mutex");
        print(REV RED "(---");
        uwork(0);
        print(REV RED "---)");
        out("mutex");
        usleep(10);
    }
}
void init() {
    out("mutex");
    eval(a());
    eval(b());
}
