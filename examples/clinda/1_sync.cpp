/**
 * (c) Ivan Janevski
 */
#include <process.h>
#include <clinda.h>
void a() {
    while (true) {
        in("a");
        print(GREEN "a");
        out("b");
    }
}
void b() {
    while (true) {
        in("b");
        print(RED "b");
        out("a");
    }
}
void init() {
    out("a");
    eval(a());
    eval(b());
}
