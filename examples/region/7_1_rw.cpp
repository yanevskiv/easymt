// Copyright (c) Ivan Janevski
#include <thread.h>
#include <region.h>
#include <stdlib.h>
#define NR 10
#define NW 5
struct RW : Region {
    int wCount = 0;
    int rCount = 0;
} rw;

void reader(int id)  {
    while (true) {
        region (rw) {
            await(rw.wCount == 0);
            rw.rCount += 1;
        }
        print(GREEN "r");
        uwork(500);
		region (rw) {
            rw.rCount -= 1;
        }
        usleep(10);
    }
}
void writer(int id)  {
    while (true) {
        region (rw) {
            await(rw.wCount == 0 && rw.rCount == 0);
            rw.wCount += 1;
        }
        print(RED "w");
        uwork(500);
		region (rw) {
            rw.wCount -= 1;
        }
        usleep(10);
    }
}
int main() {
    Thread tr[NR], tw[NW];
    for (int i = 0; i < NR; i++) 
        tr[i] = createThread(reader, i);
    for (int i = 0; i < NW; i++) 
        tw[i] = createThread(writer, i);
    for (int i = 0; i < NR; i++) 
        join(tr[i]);
    for (int i = 0; i < NW; i++) 
        join(tw[i]);
    return 0;
}
