#include <process.h>
#include <clinda.h>
#include <stdlib.h>
#define B 10
void producer() {
    int head, count, data;
    while (true) {
        // produce data...
        data = rand() % 1000;
        // put data into buffer
        in("full");
        in("mutex");
        in("head", &head);
        in("count", &count);
        out("data", head, data);
        out("head", head + 1);
        out("count", count + 1);
        out("mutex");
        out("empty");
    }
    
}

void consumer() {
    int tail, count, data;
    while (true) {
        // get data from buffer
        in("empty");
        in("mutex");
        in("tail", &tail);
        in("count", &count);
        in("data", tail, &data);
        out("tail", tail + 1);
        out("count", count - 1);
        out("mutex");
        out("full");
        // consume data
        print("%d ", data);
    }
}
void init() {
    out("mutex");
    for (int i = 0; i < B; i++)
        out("full");
    out("head", 0);
    out("tail", 0);
    out("count", 0);
    eval(producer());
    eval(consumer());
}
