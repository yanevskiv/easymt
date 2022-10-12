// Copyright (c) Ivan Janevski
#include <thread.h>
#include <region.h>
#define N 5

struct Table : Region {
    bool eating[N] = { false };
    bool fork[N] = { false };
} t;

void show_forks(const char* msg, int id) {
    // print forks (red = taken, green = not taken)
    for (int i = 0; i < N; i++)
        print("%s %d \e[0m", t.fork[i] ? REV RED : GREEN, i);
    // separator
    print("%5c", ' ');
    // print philosophers eating (blue = eating)
    for (int i = 0; i < N; i++)
        print("%s %d \e[0m", t.eating[i] ? REV BLUE : NONE , i);
    // separator
    print("%5c", ' ');
    // status of philosopher `id`
    print("%d %s\n", id, msg);
}

void phil(int id) {
    int left = id;
    int right = (id + 1) % N;
    while (true) {
        // Think
        uwork(500);
        // Acquire forks
        region (t) {
            await(t.fork[left] == false && t.fork[right] == false);
            t.eating[id] = true;
            t.fork[left] = true;
            t.fork[right] = true;
            show_forks("Eating...", id);
        }
        // Eat
        uwork(500);
        // Release forks
        region (t) {
            t.eating[id] = false;
            t.fork[left] = false;
            t.fork[right] = false;
            show_forks("Thinking...", id);
        }
    }
}

int main() {
    Thread tp[N];
    for (int i = 0; i < N; i++) 
        tp[i] = createThread(phil, i);
    for (int i = 0; i < N; i++) 
        join(tp[i]);
    return 0;
}
