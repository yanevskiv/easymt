#include <thread.h>
#include <semfor.h>
#include <queue>
using namespace std;
#define NR 9
#define NW 1

Sem mutex = 1;
Sem rSem = 0;
Sem wSem = 0;
int wCount = 0;
int rCount = 0;
int wDelay = 0;
int rDelay = 0;
enum {
    RW_LOCK   = 1,
    RW_UNLOCK = 2,
    RW_READER = 4,
    RW_WRITER = 8 

};
enum {
    WRITER = 0,
    READER = 1,
    LOCK = 1,
    UNLOCK = 0
};

int count[2] = {0, 0};
typedef pair<int, Sem*> PAIR;
queue<PAIR> qWait;

void rw_lock(int type, int lock)
{
    wait(mutex);
    if (lock) {
        int shouldDelay = ! qWait.empty()
            || type == READER && count[WRITER] > 0
            || type == WRITER && (count[WRITER] > 0 || count[READER] > 0);
        if (shouldDelay) {
            Sem sem = 0;
            qWait.push(PAIR(type, &sem));
            signal(mutex);
            wait(sem);
        }
        count[type] += 1;
    } else {
        count[type] -= 1;
    }

    /*
     * SIGNAL();
     */
    if (! qWait.empty()) {
        PAIR p = qWait.front();
        int pType = p.first;
        Sem* pSem = p.second;
        int shouldUnblock =
            pType == READER && count[WRITER] == 0
            || pType == WRITER && count[WRITER] == 0 && count[READER] == 0;
        if (shouldUnblock) {
            qWait.pop();
            pSem->signal();
        } else {
            signal(mutex);
        }
    } else {
        signal(mutex);
    }

}

void rw_lock2(int flag) {
    wait(mutex);
    if (flag & RW_LOCK) {
        if (flag & RW_READER) {
            if (wCount > 0) {
                rDelay++;
                signal(mutex);
                wait(rSem);
            }
            rCount++;
        } else if (flag & RW_WRITER) {
            if (rCount > 0 || wCount > 0) {
                wDelay++;
                signal(mutex);
                wait(wSem);
            }
            wCount++;
        }
    } else if (flag & RW_UNLOCK) {
        if (flag & RW_READER) {
            rCount--;
        } else if (flag & RW_WRITER) {
            wCount--;
        }
    }
    
    if (wDelay > 0 && rCount == 0 && wCount == 0) {
        wDelay--;
        signal(wSem);
    } else if (rDelay > 0 && wCount == 0) {
        rDelay--;
        signal(rSem);
    } else {
        signal(mutex);
    }
}

void reader(int id) {
    while (true) {
        rw_lock(READER, LOCK);
        nanowork(1000);
        print(GREEN "%d" NONE, count[READER]);
        rw_lock(READER, UNLOCK);
        usleep(10);
    }
}

void writer(int id) {
    while (true) {
        rw_lock(WRITER, LOCK);
        nanowork(1000);
        print(RED "%d" NONE, count[WRITER]);
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
