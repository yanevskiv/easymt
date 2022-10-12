#include <thread.h>
#include <sem.h>
#include <queue>
using namespace std;
#define MAX_SEATS 5
#define N 10

Sem privb = 0; // privatni semafor berberina
Sem privc[N];  // privatni semafori musterija
Sem mutex = 1; // mutex koji stiti qSeats

queue<int> qSeats;

extern void cut_hair(int custId);

void barber() {
    while (true) {
        wait(privb);
        wait(mutex);
        int custId = qSeats.front();
        qSeats.pop();
        signal(mutex);
        signal(privc[custId]);
        cut_hair(custId);
    }
}

void customer(int id) {
    while (true) {
        wait(mutex);
        if (qSeats.size() < MAX_SEATS)  {
            qSeats.push(id);
            signal(mutex);
            wait(privc[id]);
            cut_hair(id);
        } else {
            signal(mutex);
        }
        usleep(100);
    }
}

int main() {
    for (int i = 0; i < N; i++) 
        init(privc[i], 0);
    Thread tb = createThread(barber);
    Thread tc[N];
    for (int i = 0; i < N; i++) 
        tc[i] = createThread(customer, i);
    join(tb);
    join(tb);
    return 0;
}
