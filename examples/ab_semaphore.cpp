#include <thread.h>
#include <semfor.h>

Sem sa, sb;

void a() {
    for ( ; ; ) {
        wait(sa);
        print(RED "a" NONE);
        signal(sb);
    }
}

void b() 
{
    for ( ; ; ) {
        wait(sb);
        print(GREEN "b" NONE);
        signal(sa);
    }
}

int main() {
    init(sa, 1);
    init(sb, 0);
    Thread ta = createThread(a);
    Thread tb = createThread(b);
    join(ta);
    join(tb);
    destroy(sa);
    destroy(sb);
    return 0;
}
