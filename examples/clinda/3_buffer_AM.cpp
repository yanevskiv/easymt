#include <process.h>
#include <clinda.h>
#include <stdlib.h>
#include <queue>
#define B 10
enum {
    PUT_DATA = 0,
    GET_DATA = 1,
    IGNORE = -1
};
void server() {
    int buffer[B], saved;
    int head = 0;
    int tail = 0;
    int count = 0;
    int putDelay = 0;
    int getDelay = 0;
    while (true) {
        int action, data;
        in("request", &action, &data);
        // process action
        switch (action) {
            case PUT_DATA: {
                if (count < B && putDelay == 0) {
                    buffer[head] = data;
                    head = (head + 1) % B;
                    count += 1;
                    out("response", PUT_DATA, IGNORE);
                } else {
                    putDelay += 1;
                    saved = data;
                }
            } break;
            case GET_DATA: {
                if (count > 0 && getDelay == 0) {
                    data = buffer[tail];
                    tail = (tail + 1) % B;
                    count -= 1;
                    out("response", GET_DATA, data);
                } else {
                    getDelay += 1;
                }
            } break;
        }
        // try to wake up producer
        if (putDelay > 0 && count < B)  {
            putDelay -= 1;
            buffer[head] = saved;
            head = (head + 1) % B;
            count += 1;
            out("response", PUT_DATA, IGNORE);
        } 
        // try to wake up consumer
        if (getDelay > 0 && count > 0) {
            getDelay -= 1;
            buffer[head] = data;
            head = (head + 1) % B;
            count += 1;
            out("response", GET_DATA, data);
        }
    }
}
void producer() {
    int data;
    while (true) {
        // produce data...
        data = rand() % 1000;
        // put data into buffer
        out("request", PUT_DATA, data);
        in("response", PUT_DATA, IGNORE);
    }
}
void consumer() {
    int data;
    while (true) {
        // get data from buffer
        out("request", GET_DATA, IGNORE);
        in("response", GET_DATA, &data);
        // consume data
        print("%d ", data);
    }
}
void init() {
    eval(server());
    eval(producer());
    eval(consumer());
}
