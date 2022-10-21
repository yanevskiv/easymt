/**
 * (c) Ivan Janevski
 */
#include <print.h>
#include <clinda.h>
#define N 5


void phil(int id) {
    int left = id;
    int right = (id + 1) % N;
    while (true) {
        // Think
        println("Thinking %d", id);
        usleep(10);
        // Acquire forks
        if (id % 2 == 0) {
            in("fork", left);
            in("fork", right);
        } else {
            in("fork", right);
            in("fork", left);
        }
        // Eating
        println("Eating %d", id);
        usleep(10);
        // Release forks
        out("fork", left);
        out("fork", right);
    }
}

void init() {
    for (int i = 0; i < N; i++) 
        out("fork", i);
    for (int i = 0; i < N; i++) 
        eval(phil(i));
}
