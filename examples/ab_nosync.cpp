#include <thread.h>

void a() {
    for ( ; ; ) {
        print(RED "a" NONE);
    }
}

void b() 
{
    for ( ; ; ) {
        print(GREEN "b" NONE);
    }
}

int main() {
    Thread ta = createThread(a);
    Thread tb = createThread(b);
    join(ta);
    join(tb);
    return 0;
}
