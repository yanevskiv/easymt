// Copyright (c) Ivan Janevski
#include <thread.h>
#include <region.h>
#define N 10
struct AB : Region {
    int c = 'a';
} ab;

void a() {
    while (true) {
        region (ab) {
            await(ab.c == 'a');
        }
        print(RED "a");
        region (ab) {
            ab.c = 'b';
        }
    }
}
void b() {
    while (true) {
        region (ab) {
            await(ab.c == 'b');
        }
        print(GREEN "b");
        region (ab) {
            ab.c = 'a';
        }
    }
}
int main() 
{
    Thread ta = createThread(a);
    Thread tb = createThread(b);
    join(ta);
    join(tb);
    return 0;
}
