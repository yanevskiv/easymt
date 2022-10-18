#include <print.h>
#include <clinda.h>
#include <queue>
#define N 15
#define BARBER_ID (N + 1)
#define MAX_SEATS 5
enum {
    INVALID = -1,
    BARB_ENTER,
    CUST_ENTER,
};

void server() {
    std::queue<int> qCustomers, qBarbers;
    while (true) {
        int action, id;
        in("request", &action, &id);
        switch (action)  {
            case CUST_ENTER: {
                if (qCustomers.size() >= MAX_SEATS) {
                    out("response", id, INVALID);
                } else if (qCustomers.size() > 0 || qBarbers.size() == 0) {
                    qCustomers.push(id);
                } else {
                    int barbId = qBarbers.front();
                    qBarbers.pop();
                    out("response", id, barbId);
                    out("response", barbId, id);
                } 
            } break;
            case BARB_ENTER: {
                if (qCustomers.size() > 0 && qBarbers.size() == 0) {
                    int custId = qCustomers.front();
                    qCustomers.pop();
                    out("response", id, custId);
                    out("response", custId, id);
                } else {
                    qBarbers.push(id);
                }
            } break;
        }
    }
}

void barber(int id) {
    int custId = INVALID;
    while (true) {
        out("request", BARB_ENTER, id);
        in("response", id, &custId);
        println(GREEN "Cutting hair of %d", custId);
        /* cutting hair */
    }
}

void customer(int id) {
    int barbId = INVALID;
    while (true) {
        out("request", CUST_ENTER, id);
        in("response", id, &barbId);
        if (barbId != INVALID) {
            /* cutting hair */
            println(YELLOW "Getting hair cut from %d", barbId);
        }
    }
}


int main() {
    eval(barber(BARBER_ID));
    eval(server());
    for (int i = 0; i < N; i++) 
        eval(customer(i));
    return 0;
}
