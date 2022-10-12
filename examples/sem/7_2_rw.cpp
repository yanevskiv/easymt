// Copyright (c) Ivan Janevski
#include <thread.h>
#include <sem.h>
#include <queue>
#include <stdio.h>
#define NR 10
#define NW 5
#define LOCK true
#define UNLOCK false
enum {
    WRITER = 0,
    READER = 1
};

Sem mutex = 1;
int count[2] = {0, 0};
typedef std::pair<int, Sem&> PAIR;
std::queue<PAIR> qWait;

void rw_lock(int type, int lock)
{
	// WAIT()
    wait(mutex);
    if (lock) {
        // DELAY()
        int shouldDelay = ! qWait.empty()
            || type == READER && count[WRITER] > 0
            || type == WRITER && (count[WRITER] > 0 || count[READER] > 0);
        if (shouldDelay) {
            Sem sem = 0; // privatni semafor
            qWait.push(PAIR(type, sem));
            signal(mutex);
            wait(sem);
        }
        count[type] += 1;
    } else {
        count[type] -= 1;
    }
    // SIGNAL()
    if (! qWait.empty()) {
        PAIR p = qWait.front();
        int pType = p.first;
        Sem& pSem = p.second;
        int shouldUnblock =
            pType == READER && count[WRITER] == 0
            || pType == WRITER && count[WRITER] == 0 && count[READER] == 0;
        if (shouldUnblock) {
            qWait.pop();
            signal(pSem);
        } else {
            signal(mutex);
        }
    } else {
        signal(mutex);
    }
}

void reader(int id) {
    while (true) {
        rw_lock(READER, LOCK);
        print(GREEN "r");
        rw_lock(READER, UNLOCK);
        usleep(10);
    }
}

void writer(int id) {
    while (true) {
        rw_lock(WRITER, LOCK);
        print(RED "w");
		rw_lock(WRITER, UNLOCK);
        usleep(10);
    }
}

int main() {
    Thread tr[NR];
    Thread tw[NW];
    for (int i = 0; i < NR; i++)
        tr[i] = createThread(reader, i);
    for (int i = 0; i < NW; i++)
        tw[i] = createThread(writer, i);
    for (int i = 0; i < NR; i++)
        join(tr[i]);
    for (int i = 0; i < NW; i++)
        join(tw[i]);
    return 0;
}
