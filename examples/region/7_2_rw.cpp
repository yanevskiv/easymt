// Copyright (c) Ivan Janevski
#include <thread.h>
#include <region.h>
#include <queue>
#define NR 10
#define NW 5
enum {
    READER,
    WRITER
};
struct RW : Region {
    int count[2] = { 0, 0 };
    std::queue<int> delay;
} rw;

void reader(int id) {
    while (true) {
        region (rw) {
            rw.delay.push(id);
            await(rw.delay.front() == id && rw.count[WRITER] == 0);
            rw.delay.pop();
            rw.count[READER] += 1;
        }
        print(GREEN "r");
        uwork(500);
        region (rw) {
            rw.count[READER] -= 1;
        }
        usleep(10);
    }
}

void writer(int id) {
    while (true) {
        region (rw) {
            rw.delay.push(id);
            await(rw.delay.front() == id && rw.count[WRITER] == 0 && rw.count[READER] == 0);
            rw.delay.pop();
            rw.count[WRITER] += 1;
        }
        print(RED "w");
        uwork(500);
        region (rw) {
            rw.count[WRITER] -= 1;
        }
        usleep(10);
    }
}

int main() {
    Thread tr[NR], tw[NW];
    for (int i = 0; i < NR; i++) 
        tr[i] = createThread(reader, i);
    for (int i = 0; i < NW; i++) 
        tw[i] = createThread(writer, NR + i);
    for (int i = 0; i < NR; i++) 
        join(tr[i]);
    for (int i = 0; i < NW; i++) 
        join(tw[i]);
    return 0;
}

