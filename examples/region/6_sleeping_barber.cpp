#include <thread.h>
#include <region.h>
#include <queue>
using namespace std;
#define MAX_SEATS 5
#define N 10

struct BarberShop : Region {
    int custId = -1;
    bool paid = false;
    queue<int> qSeats;
} shop;

extern void cut_hair(int custId);

void barber() {
    int custId = -1;
    while (true) {
        region (shop) {
            await(shop.qSeats.size() > 0);
            shop.custId = shop.qSeats.front();
            shop.qSeats.pop();
        }
        /* cut hair */
        region (shop) {
            await(shop.paid);
            println(REV "\e[%dm %d ", 31 + shop.custId, shop.custId);
            shop.paid = false;
            shop.custId = -1;
        }
    }
}

void customer(int id) {
    bool isCutting;
    while (true) {
        isCutting = false;
        region (shop) {
            if (shop.qSeats.size() < MAX_SEATS)  {
                isCutting = true;
                shop.qSeats.push(id);
                await(shop.custId == id);
            }
        }
        if (isCutting) {
            /* cut hair */
            region (shop) {
                print(REV "\e[%dm %d ", 31 + shop.custId, shop.custId);
                shop.paid = true;
            }
        }
        usleep(100);
    }
}

int main() {
    Thread tb = createThread(barber);
    Thread tc[N];
    for (int i = 0; i < N; i++) 
        tc[i] = createThread(customer, i);
    join(tb);
    for (int i = 0; i < N; i++) 
        join(tc[i]);
    return 0;
}
