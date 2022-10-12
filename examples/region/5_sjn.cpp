// Copyright (c) Ivan Janevski
#include <thread.h>
#include <region.h>
#include <stdlib.h>
#include <queue>
#define N 5

typedef std::pair<int, int> PAIR;

struct SJN : Region {
    bool locked = false;
    std::priority_queue<PAIR, std::vector<PAIR>, std::greater<PAIR>> qEnter;
} sjn;

void a(int id) 
{
    int prio = id; // id based priority
    while (true) {
        region (sjn) {
            sjn.qEnter.push(PAIR(prio, id));
            await(sjn.locked == false && sjn.qEnter.top().second == id);
            sjn.qEnter.pop();
            sjn.locked = true;
        }
        print(REV "\e[%dm(---", 31 + id);
        uwork(500);
        print(REV "\e[%dm---)", 31 + id);
        region (sjn) {
            sjn.locked = false;
        }
        usleep(50);
    }
}


int main() 
{
    Thread tp[N];
    for (int i = 0; i < N; i++) 
        tp[i] = createThread(a, i);
    for (int i = 0; i < N; i++) 
        join(tp[i]);
    return 0;
}
