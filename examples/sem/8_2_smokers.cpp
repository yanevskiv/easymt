#include <thread.h>
#include <sem.h>
#include <stdlib.h>

enum {
    MATCHES = 0,
    TOBACCO = 1,
    PAPER   = 2
};

Sem sem[] = { 0, 0, 0 };
Sem ok = 0;
Sem mutex = 1;
bool table[] = { false, false, false };
bool agree[] = { false, false, false };

#define ITEM_COUNT 3
#define OTHER_ITEM_1(item) (((item) + 1) % ITEM_COUNT)
#define OTHER_ITEM_2(item) ((item) == 0 ? (ITEM_COUNT - 1) : ((item) - 1))
    

void agent() {
    while (true) {
        /* produce two items */
        int item = rand() % 3;
        table[OTHER_ITEM_1(item)] = true;
        table[OTHER_ITEM_2(item)] = true;
        print("\e[%dmproducing... ", 31 + item);
        signal(sem[OTHER_ITEM_1(item)]);
        signal(sem[OTHER_ITEM_2(item)]);
        wait(ok);
    }
}

void smoker(int id) {
    while (true) {
        while (true) {
            wait(sem[id]);
            wait(mutex);
            agree[id] = true;
            if (agree[PAPER] && agree[MATCHES] && agree[TOBACCO]) {
                agree[PAPER] = false;
                agree[MATCHES] = false;
                agree[TOBACCO] = false;
                signal(mutex);
                break;
            } else if (agree[id] && agree[OTHER_ITEM_1(id)]) {
                signal(sem[OTHER_ITEM_2(id)]);
            } else if (agree[id] && agree[OTHER_ITEM_2(id)]) {
                signal(sem[OTHER_ITEM_1(id)]);
            }
            signal(mutex);
        }
        table[OTHER_ITEM_1(id)] = false;
        table[OTHER_ITEM_2(id)] = false;
        /* smoke */
        println("\e[%dm consuming", 31 + id);
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
