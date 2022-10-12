#include <thread.h>
#include <sem.h>
#include <stdlib.h>
#define ITEM_COUNT  3
#define THIRD_ITEM(first, second) (1 - (((first) - 1) + ((second) -1)))
#define OTHER_ITEM_1(item) ((item) == 0 ? ITEM_COUNT - 1 : (item) - 1)
#define OTHER_ITEM_2(item) (((item) + 1) % ITEM_COUNT) 

enum {
    MATCHES = 0,
    TOBACCO = 1,
    PAPER   = 2
};


Sem mutex = 1;
Sem ok = 0;
Sem sem[ITEM_COUNT] = { 0, 0, 0 };
bool table[ITEM_COUNT] = { false, false, false };

void agent() {
    while (true) {
        // produce two items
        int item = rand() % 3;
        // put them on table
        wait(mutex);
        table[OTHER_ITEM_1(item)] = true;
        table[OTHER_ITEM_2(item)] = true;
        print("\e[%dm producing... ", 31 + item);
        signal(mutex);
        signal(sem[MATCHES]);
        signal(sem[TOBACCO]);
        signal(sem[PAPER]);
        wait(ok);
    }
}

void smoker(int item) {
    int item1 = OTHER_ITEM_1(item);
    int item2 = OTHER_ITEM_2(item);
    while (true) {
        // check items
        while (true) {
            wait(sem[item]);
            wait(mutex);
            if (table[item1] && table[item2]) {
                table[item1] = false;
                table[item2] = false;
                signal(mutex);
                break;
            } else {
                signal(mutex);
            }
        }
        println("\e[%dm smoking", 31 + item);
        signal(ok);
    }
    
}


int main() {
    Thread ta = createThread(agent);
    Thread ts1 = createThread(smoker, MATCHES);
    Thread ts2 = createThread(smoker, PAPER);
    Thread ts3 = createThread(smoker, TOBACCO);
    join(ta);
    join(ts1);
    join(ts2);
    join(ts3);
    return 0;
}
