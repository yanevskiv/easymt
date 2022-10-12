#include <thread.h>
#include <region.h>
#include <stdlib.h>

enum {
    MATCHES = 0,
    TOBACCO = 1,
    PAPER   = 2
};

struct Table : Region {
    bool matches = false;
    bool paper   = false;
    bool tobacco = false;
    bool ok      = false;
} table; 

void agent() {
    while (true) {
        /* produce two items */
        int item = rand() % 3;
        switch (item) {
            case MATCHES: {
                region (table) {
                    table.paper = true;
                    table.tobacco = true;
                    print(GREEN "producing... ");
                    await(table.ok);
                    table.ok = false;
                }
            } break;
            case PAPER: {
                region (table) {
                    table.matches = true;
                    table.tobacco = true;
                    print(YELLOW "producing... ");
                    await(table.ok);
                    table.ok = false;
                }
            } break;
            case TOBACCO: {
                region (table) {
                    table.matches = true;
                    table.paper = true;
                    print(RED "producing... ");
                    await(table.ok);
                    table.ok = false;
                }
            } break;
        }
    }
}

void smoker(int id) {
    while (true) {
        switch (id) {
            case MATCHES: {
                region (table) {
                    await(table.paper && table.tobacco);
                    table.paper = false;
                    table.tobacco = false;
                }
                /* smoke */
                println(GREEN "smoking");
                region (table) {
                    table.ok = true;
                }
            } break;
            case PAPER: {
                region (table) {
                    await(table.matches && table.tobacco);
                    table.matches = false;
                    table.tobacco = false;
                }
                /* smoke */
                println(YELLOW "smoking");
                region (table) {
                    table.ok = true;
                }
            } break;
            case TOBACCO: {
                region (table) {
                    await(table.matches && table.paper);
                    table.matches = false;
                    table.paper = false;
                }
                /* smoke */
                println(RED "smoking");
                region (table) {
                    table.ok = true;
                }
            } break;
        }
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
