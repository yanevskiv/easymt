#include <thread.h>
#include <clinda.h>
#define N 5

void bar_wait(const char *bar) {
    int count;
    in("mutex", bar);
    in("count", bar, &count);
    count += 1;
    if (count == N) {
        count = 0;
        for (int i = 0; i < N; i++)  
            out("barrier", bar);
    }
    out("count", bar, count);
    out("mutex", bar);
    in("barrier", bar);
}

void a(int id) {
    int count;
    while (true) {
        bar_wait("bar1");
        print("\e[%dm %d ", 31 + id, id);

        bar_wait("bar2");
        print("\e[7m\e[%dm %d ", 31 + id, id);
    }
}


int main() {
    out("mutex", "bar1");
    out("mutex", "bar2");
    out("count", "bar1", 0);
    out("count", "bar2", 0);
    for (int i = 0; i < N; i++) 
        eval(a(i));
    return 0;
}
