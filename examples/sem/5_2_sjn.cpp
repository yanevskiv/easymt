// Copyright (c) Ivan Janevski
#include <thread.h>
#include <sem.h>
#include <queue>
#include <stdlib.h>
#define N 5

typedef std::pair<int, int> PAIR;

Sem mutex = 1;
Sem privs[N] = { 0, 0, 0, 0, 0 };
std::priority_queue<PAIR, std::vector<PAIR>, std::greater<PAIR>> qDelay;
bool isWorking = false;

void sjn_wait(int id, int prio)
{
    wait(mutex);
    if (isWorking) {
        PAIR p(prio, id);
        qDelay.push(p);
        signal(mutex);
        wait(privs[id]);
        wait(mutex);
    } 
    isWorking = true;
    signal(mutex);
}

void sjn_signal() 
{
    wait(mutex);
    isWorking = false;
    if (! qDelay.empty()) {
        PAIR p = qDelay.top();
        int pPrio = p.first;
        int pId = p.second;
        qDelay.pop();
        signal(privs[pId]);
    }
    signal(mutex); 
}

void a(int id) 
{
    int prio = id; // priority equal to id
    while (true) {
        sjn_wait(id, prio);
        print(REV "\e[%dm(---", 30 + id);
        uwork(500);
        print(REV "\e[%dm---)", 30 + id);
        sjn_signal();
        usleep(50);
    }
}


int main() {
    Thread ta[N];
    for (int i = 0; i < N; i++)
        ta[i] = createThread(a, i);
    for (int i = 0; i < N; i++)
		join(ta[i]);
    return 0;
}
