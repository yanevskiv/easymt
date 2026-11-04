/**
 * (c) Ivan Janevski
 */
#include <print.h>
#include <clinda.h>
#include <queue>
#define N 15
#define BARBER_ID N
#define MAX_SEATS 5
#define SHOP_PRICE 400

enum {
    ENTER,
    NEXT,
    DONE,
    PAY
};

void server() {
    std::queue<int> seats;
    bool barberWaiting = false;
    while (true) {
        int id, action, arg;
        in("request", &id, &action, &arg);
        switch (action) {
            case ENTER: {
                if (seats.size() >= MAX_SEATS)
                    out("response", id, 0);
                else
                    seats.push(id);
            } break;
            case NEXT: {
                barberWaiting = true;
            } break;
            case DONE: {
                out("response", arg, 1);
            } break;
            case PAY: {
                out("response", id, 1);
                out("response", BARBER_ID, arg);
            } break;
        }
        if (barberWaiting && ! seats.empty()) {
            int custId = seats.front();
            seats.pop();
            barberWaiting = false;
            out("response", custId, 1);
            out("response", BARBER_ID, custId);
        }
    }
}

void barber() {
    int totalMoney = 0;
    while (true) {
        int custId, money;
        out("request", BARBER_ID, NEXT, 0);
        in("response", BARBER_ID, &custId);
        println(GREEN "Cutting hair of %d", custId);
        /* cutting hair */
        out("request", BARBER_ID, DONE, custId);
        in("response", BARBER_ID, &money);
        totalMoney += money;
        println(RED "Got paid %d (total %d)", money, totalMoney);
    }
}

void customer(int id) {
    while (true) {
        int ok;
        out("request", id, ENTER, 0);
        in("response", id, &ok);
        if (ok) {
            in("response", id, &ok);
            println(YELLOW "%d paying for the haircut", id);
            out("request", id, PAY, SHOP_PRICE);
            in("response", id, &ok);
        }
        usleep(100);
    }
}

void init() {
    eval(server());
    eval(barber());
    for (int i = 0; i < N; i++)
        eval(customer(i));
}
