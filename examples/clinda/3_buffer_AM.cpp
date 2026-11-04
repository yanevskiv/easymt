/**
 * (c) Ivan Janevski
 */
#include <process.h>
#include <clinda.h>
#include <stdlib.h>
#define B 10

enum {
    PUT,
    GET
};

void server() {
    int buffer[B];
    int head = 0;
    int tail = 0;
    int count = 0;
    bool putDelayed = false;
    bool getDelayed = false;
    int putData = 0;
    while (true) {
        int action, data;
        in("request", &action, &data);
        if (action == PUT) {
            if (count < B) {
                buffer[head] = data;
                head = (head + 1) % B;
                count += 1;
                out("response", PUT, 0);
            } else {
                putDelayed = true;
                putData = data;
            }
            if (getDelayed && count > 0) {
                getDelayed = false;
                data = buffer[tail];
                tail = (tail + 1) % B;
                count -= 1;
                out("response", GET, data);
            }
        } else {
            if (count > 0) {
                data = buffer[tail];
                tail = (tail + 1) % B;
                count -= 1;
                out("response", GET, data);
            } else {
                getDelayed = true;
            }
            if (putDelayed && count < B) {
                putDelayed = false;
                buffer[head] = putData;
                head = (head + 1) % B;
                count += 1;
                out("response", PUT, 0);
            }
        }
    }
}

void producer() {
    while (true) {
        // produce data...
        int data = rand() % 1000;
        // put data into buffer
        int ignore;
        out("request", PUT, data);
        in("response", PUT, &ignore);
    }
}

void consumer() {
    while (true) {
        // get data from buffer
        int data;
        out("request", GET, 0);
        in("response", GET, &data);
        // consume data
        print("%d ", data);
    }
}

void init() {
    eval(server());
    eval(producer());
    eval(consumer());
}
