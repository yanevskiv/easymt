/**
 * (c) Ivan Janevski
 */
#include <print.h>
#include <clinda.h>
#define N 5
#define LEFT(id) (((id) + N - 1) % N)
#define RIGHT(id) (((id) + 1) % N)

enum {
    ACQUIRE,
    RELEASE
};

void test(int id, bool forkFree[], bool hungry[]) {
    if (hungry[id] && forkFree[id] && forkFree[RIGHT(id)]) {
        hungry[id] = false;
        forkFree[id] = false;
        forkFree[RIGHT(id)] = false;
        out("response", id);
    }
}

void server() {
    bool forkFree[N] = { true, true, true, true, true };
    bool hungry[N] = { false, false, false, false, false };
    while (true) {
        int id, action;
        in("request", &id, &action);
        if (action == ACQUIRE) {
            hungry[id] = true;
            test(id, forkFree, hungry);
        } else {
            forkFree[id] = true;
            forkFree[RIGHT(id)] = true;
            out("response", id);
            test(LEFT(id), forkFree, hungry);
            test(RIGHT(id), forkFree, hungry);
        }
    }
}

void phil(int id) {
    while (true) {
        // Think
        println("Thinking %d", id);
        usleep(10);
        // Acquire forks
        out("request", id, ACQUIRE);
        in("response", id);
        // Eating
        println("Eating %d", id);
        usleep(10);
        // Release forks
        out("request", id, RELEASE);
        in("response", id);
    }
}

void init() {
    eval(server());
    for (int i = 0; i < N; i++)
        eval(phil(i));
}
