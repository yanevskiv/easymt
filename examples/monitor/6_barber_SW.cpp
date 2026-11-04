// (c) Ivan Janevski
#include <thread.h>
#include <monitor.h>
#include <queue>
#define MAX_SEATS 5
#define SHOP_PRICE 400
#define N 10

class BarberShop : SW_Monitor {
    std::queue<int> m_seats;
    int  m_called = -1;
    bool m_done = false;
    bool m_paid = false;
    int  m_money = 0;
    cond m_waitForCustomer;
    cond m_waitForBarber;
    cond m_waitForHaircut;
    cond m_waitForPayment;
public:
    BarberShop() : 
        m_waitForCustomer(this), 
        m_waitForBarber(this),
        m_waitForHaircut(this),
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
            m_called = custId;
            m_waitForBarber.signal();
            return custId;
        }
    }

    int barber_done() {
        monitor {
            m_done = true;
            m_waitForHaircut.signal();
            if (! m_paid) 
                m_waitForPayment.wait();
            int payment = m_money;
            m_called = -1;
            m_done = false;
            m_paid = false;
            m_money = 0;
            return payment;
        }
    }

    bool enter_shop(int id) {
        monitor {
            if (m_seats.size() >= MAX_SEATS)
                return false;
            m_seats.push(id);
            m_waitForCustomer.signal();
            if (m_called != id)
                m_waitForBarber.wait();
            return true;
        }
    }

    void exit_shop(int money) {
        monitor {
            if (! m_done)
                m_waitForHaircut.wait();
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
        money += shop.barber_done();
        println(" %d", money);
    }
}

void customer(int id) {
    while (true) {
        if (shop.enter_shop(id)) {
            print(GREEN "%d", id);
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
    for (int i = 0; i < N; i++) 
        join(tc[i]);
    return 0;
}
