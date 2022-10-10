// (c) Ivan Janevski
#include <thread.h>
#include <monitor.h>
#include <cstdlib>
enum {
    PAPER = 0,
    MATCHES = 1,
    TOBACCO = 2
};

#define ITEM_COUNT 3
#define OTHER_ITEM_1(item) ((item) == 0 ? ITEM_COUNT - 1 : ((item) - 1))
#define OTHER_ITEM_2(item) (((item) + 1) % ITEM_COUNT)
#define THIRD_ITEM(item1, item2) (1 - (((item1) - 1) + ((item2) - 1)))

class Table : SW_Monitor {
    bool m_items[ITEM_COUNT] = { false, false, false };
    cond m_waitForItems[ITEM_COUNT];
    bool m_ok = false;
    cond m_waitForOk;
public:
    Table() : m_waitForItems((cond[]){this, this, this}), m_waitForOk(this) {

    }
    void put_items(int item1, int item2) {
        monitor {
            int third = THIRD_ITEM(item1, item2);
            m_items[item1] = true;
            m_items[item2] = true;
            m_waitForItems[third].signal();
        }
    }
    void get_items(int item1, int item2) {
        monitor {
            int third = THIRD_ITEM(item1, item2);
            if (! m_items[item1] || ! m_items[item2]) 
                m_waitForItems[third].wait();
            m_items[item1] = false;
            m_items[item2] = false;
        }
    }
    void wait_ok() {
        monitor {
            if (! m_ok)
                m_waitForOk.wait();
            m_ok = false;
        }
    }
    void signal_ok() {
        monitor {
            m_ok = true;
            m_waitForOk.signal();
        }
    }
} table;

void agent() {
    while (true) {
        int notItem = rand() % ITEM_COUNT;
        switch (notItem) {
            case MATCHES: {
                // produce items...
                // put items on the table
                print(GREEN "creating... ");
                table.put_items(PAPER, TOBACCO);
                table.wait_ok();
                println(GREEN "done.");
            } break;
            case PAPER: {
                // produce items...
                // put items on the table
                print(YELLOW "creating... ");
                table.put_items(MATCHES, TOBACCO);
                table.wait_ok();
                println(YELLOW "done.");
            } break;
            case TOBACCO: {
                // produce items...
                // put items on the table
                print(RED "creating... ");
                table.put_items(MATCHES, PAPER);
                table.wait_ok();
                println(RED "done.");
            } break;
        }
    }
}

void smoker(int hasItem) {
    while (true) {
        switch (hasItem) {
            case MATCHES: {
                table.get_items(PAPER, TOBACCO);
                // create a cigarette with the three items
                // smoke...
                print(GREEN "smoking... ");
                table.signal_ok();
            } break;
            case PAPER: {
                table.get_items(MATCHES, TOBACCO);
                // create a cigarette with the three items
                // smoke...
                print(YELLOW "smoking... ");
                table.signal_ok();
            } break;
            case TOBACCO: {
                table.get_items(PAPER, MATCHES);
                // create a cigarette with the three items
                // smoke...
                print(RED "smoking... ");
                table.signal_ok();
            } break;
        }
    }
}

int main() {
    Thread ta = createThread(agent);
    Thread ts1 = createThread(smoker, MATCHES);
    Thread ts2 = createThread(smoker, PAPER);
    Thread ts3 = createThread(smoker, TOBACCO);
    join(ta);
    join(ts1);
    join(ts2);
    join(ts3);
    return 0;
}
