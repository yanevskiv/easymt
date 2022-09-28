#include <thread.h>
#include <region.h>
struct AB : Region {
    char which = 'a';
} ab;
void a() {
    for ( ; ; ) {
        region (ab) {
            await(ab.which == 'a');
            print(RED "a" NONE);
            ab.which = 'b';
        }
    }
}

void b() {
    for ( ; ; ) {
        region (ab) {
            await(ab.which == 'b')
            print(GREEN "b" NONE);
            ab.which = 'a';
        }
    }
}

int main() {
    Thread ta = createThread(a);
    Thread tb = createThread(b);
    join(ta);
    join(tb);
    return 0;
}
