/**
 * (c) Ivan Janevski
 */
#include <print.h>
#include <clinda.h>
#define N 5
#define LEFT_OF(n) (n)
#define RIGHT_OF(n) ((n + 1) % N)

enum {
    ACQUIRE,
    RELEASE
};

void server() {
    bool isAvail[N] = { true, true, true, true, true };
    bool isHungry[N] = { false, false, false, false, false };
    while (true) {
        int action, id, left, right;
        in("request", &action, &id, &left, &right);
        switch (action) {
            case ACQUIRE: {
                if (isAvail[left] && isAvail[right]) {
                    isAvail[left] = false;
                    isAvail[right] = false;
                    out("response", id);
                } else {
                    isHungry[id] = true;
                }
            } break;
            case RELEASE: {
                isAvail[left] = true;
                isAvail[right] = true;
                if (isHungry[left] && isAvail[LEFT_OF(left)] && isAvail[RIGHT_OF(left)]) {
                    isHungry[left] = false;
                    isAvail[LEFT_OF(left)] = false;
                    isAvail[RIGHT_OF(left)] = false;
                    out("response", left);
                } else if (isHungry[right] && isAvail[LEFT_OF(right)] && isAvail[RIGHT_OF(right)]) {
                    isHungry[right] = false;
                    isAvail[LEFT_OF(left)] = false;
                    isAvail[RIGHT_OF(left)] = false;
                    out("response", right);
                }
                out("response", id);
            } break;
        }
    }
}

void phil(int id) {
    int left = LEFT_OF(id);
    int right = RIGHT_OF(id);
    while (true) {
        // Think
        println("Thinking %d", id);
        usleep(10);
        // Acquire forks
        out("request", ACQUIRE, id, left, right);
        in("response", id);
        // Eating
        println("Eating %d", id);
        usleep(10);
        // Release forks
        out("request", RELEASE, id, left, right);
        in("response", id);
    }
}

void init() {
    eval(server());
    for (int i = 0; i < N; i++) 
        eval(phil(i));
}
