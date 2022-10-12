#include <thread.h>
#include <sem.h>
#include <stdlib.h>

enum {
    MATCHES = 0,
    TOBACCO = 1,
    PAPER   = 2
};

Sem matches = 0;
Sem tobacco = 0;
Sem paper   = 0;
Sem ok      = 0;

struct Table {
    bool matches = false;
    bool paper   = false;
    bool tobacco = false;
} table; 

void agent() {
    while (true) {
        /* produce two items */
        int item = rand() % 3;
        switch (item) {
            case MATCHES: {
                table.paper = true;
                table.tobacco = true;
                print(GREEN "producing... ");
                signal(matches);
                wait(ok);
            } break;
            case PAPER: {
                table.matches = true;
                table.tobacco = true;
                print(YELLOW "producing... ");
                signal(paper);
                wait(ok);
            } break;
            case TOBACCO: {
                table.matches = true;
                table.paper = true;
                print(RED "producing... ");
                signal(tobacco);
                wait(ok);
            } break;
        }
    }
}

void smoker(int id) {
    while (true) {
        switch (id) {
            case MATCHES: {
                wait(matches);
                table.paper = false;
                table.tobacco = false;
                /* smoke */
                println(GREEN "smoking");
                signal(ok);
            } break;
            case PAPER: {
                wait(paper);
                table.matches = false;
                table.tobacco = false;
                /* smoke */
                println(YELLOW "smoking");
                signal(ok);
            } break;
            case TOBACCO: {
                wait(tobacco);
                table.matches = false;
                table.paper = false;
                /* smoke */
                println(RED "smoking");
                signal(ok);
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
