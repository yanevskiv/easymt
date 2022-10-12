// Copyright (c) Ivan Janevski
#include <thread.h>
#include <sem.h>
#include <queue>
#define NR 10
#define NW 5
#define LOCK true
#define UNLOCK false
enum {
    READER,
    WRITER,
};

int wCount = 0; // nw
int rCount = 0; // nr
int wDelay = 0; // dw
int rDelay = 0; // dr
Sem mutex = 1;  // e
Sem wSem = 0;   // w
Sem rSem = 0;   // r

void rw_lock(int type, int lock) {
    // WAIT()
    wait(mutex);
    if (lock) {
        if (type == WRITER) {
            // DELAY WRITER
            if (wCount > 0 || rCount > 0) {
                wDelay += 1;
                signal(mutex);
                wait(wSem);
            }
            wCount += 1;
        } else if (type == READER) {
            // DELAY READER
            if (wCount > 0) {
                rDelay += 1;
                signal(mutex);
                wait(rSem);
            }
            rCount += 1;
        }
    } else {
        if (type == WRITER) {
            wCount -= 1;
        } else if (type == READER) {
            rCount -= 1;
        }

    }
    // SIGNAL()
    if (wDelay > 0 && wCount == 0 && rCount == 0) {
        wDelay -= 1;
        signal(wSem);
    } else if (rDelay > 0 && wCount == 0) {
        rDelay -= 1;
        signal(rSem);
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
