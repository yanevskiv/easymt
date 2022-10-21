/**
 * (c) Ivan Janevski
 */
#include <thread.h>
#include <clinda.h>
#define N 5

void a(int id) {
    int count;
    while (true) {
        in("door1");

        print("\e[%dm %d ", 30 + id, id);

        in("count", &count);
        if (count + 1 == N)  {
            out("count", 0);
            out ("door2");
        } else {
            out("count", count + 1);
            out("door1");
        }
        in("door2");
        print("\e[7m\e[%dm %d ", 30 + id, id);
        if (count + 1 == N) {
            out("count", 0);
            out("door1");
        } else {
            out("count", count + 1);
            out("door2");
        }
    }
}


int main() {
    out("door1");
    out("count", 0);
    for (int i = 0; i < N; i++) 
        eval(a(i));
    return 0;
}
