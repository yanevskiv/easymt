// Copyright (c) Ivan Janevski
#include <thread.h>
#include <region.h>
#include <stdlib.h>
#define B 10
struct Buffer : Region {
    int data[B];
    int head = 0;
    int tail = 0;
    int count = 0;
} buffer;

void producer() {
    while (true) {
		/* Producing data... */
        int data = rand() % 1000;		
		/* Put data into buffer */
        region (buffer) {
            await(buffer.count < B);
            buffer.data[buffer.head] = data;
            buffer.head = (buffer.head + 1) % B;
            buffer.count += 1;
        }
    }
}

void consumer() {
    while (true) {
		/* Get data from buffer */
        int data;
        region (buffer) {
            await(buffer.count > 0);
            data = buffer.data[buffer.tail];
            buffer.tail = (buffer.tail + 1) % B;
            buffer.count -= 1;
        }
		/* Consuming data... */
        print("%d ", data);
    }
}
int main() {
    Thread tp = createThread(producer);
    Thread tc = createThread(consumer);
    join(tp);
    join(tc);
    return 0;
}

