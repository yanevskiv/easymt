// Copyright (c) Ivan Janevski
#include <thread.h>
#include <sem.h>
#include <stdlib.h>
#define B 10
int buffer[B];
int head = 0;
int tail = 0;
int count = 0;
Sem mutex = 1;
Sem full = B;
Sem empty = 0;

void producer() {
    while (true) {
        // Produce data...
        int data = rand() % 1000;

        // Put data into buffer
        wait(full);
        wait(mutex);
        buffer[head] = data;
        head = (head + 1) % B;
        count += 1;
        signal(mutex);
        signal(empty);
    }
}
void consumer() {
    while (true) {
        // Fetch data from buffer
        wait(empty);
        wait(mutex);
        int data  = buffer[tail];
        tail = (tail + 1) % B;
        count -= 1;
        signal(mutex);
        signal(full);

        // Consume data... 
        print("%d ", data);
    }
}

int main() {
    Thread ta = createThread(producer);
    Thread tb = createThread(consumer);
    join(ta);
    join(tb);
    return 0;
}
