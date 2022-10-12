// Copyright (c) Ivan Janevski
#include <thread.h>
#include <region.h>
#include <stdlib.h>
#define N 5
struct Doors : Region {
    int count1 = 0;
    int count2 = 0;
} doors;

void a(int id) {
    while (true) {
        region (doors) {
            print("\e[%dm %d ", 31 + id, id);
            doors.count1 += 1;
            if (doors.count1 == N) {
                doors.count2 = 0;
            }
            await(doors.count1 == N);
        }

        region (doors) {
            print(REV "\e[%dm %d ", 31 + id, id);
            doors.count2 += 1;
            if (doors.count2 == N) {
                doors.count1 = 0;
            }
            await(doors.count2 == N);
        }
    }
}

int main() {
    Thread ta[N];
    for (int i = 0; i < N; i++) 
        ta[i] = createThread(a, i);
    for (int i = 0; i < N; i++) 
        join(ta[i]);
    return 0;
}
