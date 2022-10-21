/**
 * (c) Ivan Janevski
 */
#include <process.h>
#include <clinda.h>
#define OTHER_ID(id) (id == ID_A ? ID_B : ID_A)
enum {
    ENTER = 0,
    EXIT  = 1,
    OK    = 2
};
enum {
    ID_A = 0,
    ID_B = 1
};
void server() {
    int current = ID_A;
    bool delayed[2] = { false, false };
    while (true) {
        int id, action;
        in("request", &id, &action);
        switch (action) {
            case ENTER: {
                if (current == id) {
                    out("response", id, OK);
                } else {
                    delayed[id] = true;
                }
            } break;
            case EXIT: {
                current = OTHER_ID(id);
                if (delayed[OTHER_ID(id)]) {
                    delayed[OTHER_ID(id)] = false;
                    out("response", OTHER_ID(id), OK);
                }
                out("response", id, OK);
            } break;
        }
    }
}

void a() {
    while (true) {
        out("request", ID_A, ENTER);
        in("response", ID_A, OK);
        print(GREEN "a");
        out("request", ID_A, EXIT);
        in("response", ID_A, OK);
    }
}
void b() {
    while (true) {
        out("request", ID_B, ENTER);
        in("response", ID_B, OK);
        print(RED "b");
        out("request", ID_B, EXIT);
        in("response", ID_B, OK);
    }
}
void init() {
    eval(a());
    eval(b());
    eval(server());
}
