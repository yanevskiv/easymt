// Compile with: g++ -pthread -Ih -Llib main.cpp -leasymt -o main
// Run as:       ./main
//
// Expected result: ababababababab...

#include <thread.h>
#include <sem.h>

Sem sa = 1, sb = 0;

void a() {
    while (true) {
        wait(sa);
        print(GREEN "a");
        signal(sb);
    }
}

void b() {
    while (true) {
        wait(sb);
        print(RED "b");
        signal(sa);
    }
}

int main() {
    Thread ta = createThread(a);
    Thread tb = createThread(b);
    join(ta);
    join(tb);
}

