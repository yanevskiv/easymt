/**
 * (c) Ivan Janevski
 */
#include <print.h>
#include <clinda.h>
#include <stdlib.h>
#define ITEM_COUNT 3

enum {
    PAPER,
    TOBACCO,
    MATCHES,
};

const char *NAMES[ITEM_COUNT] = { "PAPER", "TOBACCO", "MATCHES" };

void agent() {
    while (true) {
        int notItem = rand() % ITEM_COUNT;
        int item1 = (notItem + 1) % ITEM_COUNT;
        int item2 = (notItem + 2) % ITEM_COUNT;
        println(RED "Agent putting %s and %s", NAMES[item1], NAMES[item2]);
        out("table", item1, item2);
        in("ok");
    }
}

void smoker(int hasItem) {
    int item1 = (hasItem + 1) % ITEM_COUNT;
    int item2 = (hasItem + 2) % ITEM_COUNT;
    while (true) {
        in("table", item1, item2);
        println(GREEN "Smoker with %s taking %s and %s", NAMES[hasItem], NAMES[item1], NAMES[item2]);
        out("ok");
    }
}

void init() {
    eval(agent());
    eval(smoker(PAPER));
    eval(smoker(TOBACCO));
    eval(smoker(MATCHES));
}
