// (c) Ivan Janevski
#include <thread.h>
#include <monitor.h>
#include <queue>
#define MAX_SEATS 5
#define SHOP_PRICE 400
#define N 10

class BarberShop : SW_Monitor {
    std::queue<int> m_seats;
    int  m_money = 0;
    bool m_paid = false;
    cond m_waitForCustomer;
    cond m_waitForBarber;
    cond m_waitForPayment;
public:
    BarberShop() : 
        m_waitForCustomer(this), 
        m_waitForBarber(this),
        m_waitForPayment(this) 
    {
        // empty
    }
    int barber_wait() {
        monitor {
            if (m_seats.empty())
                m_waitForCustomer.wait();
            int custId = m_seats.front();
            m_seats.pop();
            m_waitForBarber.signal();
            return custId;
        }
    }

    int barber_done(int id) {
        monitor {
            if (! m_paid) 
                m_waitForPayment.wait();
            int payment = m_money;
            m_paid = false;
            m_money = 0;
            return payment;
        }
    }

    bool enter_shop(int id) {
        monitor {
            if (m_seats.size() < MAX_SEATS) {
                m_seats.push(id);
                m_waitForCustomer.signal();
                m_waitForBarber.wait();
                return true;
            }
            return false;
        }
    }

    void exit_shop(int money) {
        monitor {
            m_paid = true;
            m_money += money;
            m_waitForPayment.signal();
        }
    }
} shop;

void barber() {
    int money = 0;
    while (true) {
        int custId = shop.barber_wait();
        /* cut hair */
        print(RED "%d", custId);
        money += shop.barber_done(custId);
        println(" %d", money);
    }
}

void customer(int id) {
    while (true) {
        if (shop.enter_shop(id)) {
            print(GREEN "%d", id);
            /* cut hair */
            shop.exit_shop(SHOP_PRICE);
            print(YELLOW "%d", id);
        }
        usleep(50);
    }
}

int main() {
    Thread tb = createThread(barber);
    Thread tc[N];
    for (int i = 0; i < N; i++) 
        tc[i] = createThread(customer, i);
    join(tb);
    join(tb);
    return 0;
}
