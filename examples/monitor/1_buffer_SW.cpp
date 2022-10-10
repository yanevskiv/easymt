// (c) Ivan Janevski
#include <thread.h>
#include <stdlib.h>
#include <monitor.h>
#define B 10

class Buffer_Monitor : SW_Monitor {
    int m_data[B];
    int m_head = 0;
    int m_tail = 0;
    int m_count = 0;
    cond m_full;
    cond m_empty;
public:
    Buffer_Monitor () : m_full(this), m_empty(this) {
        // empty
    }
    void put_data(int data) {
        monitor {
            if (m_count == B)
                m_full.wait();
            m_data[m_head] = data;
            m_head = (m_head + 1) % B;
            m_count += 1;
            m_empty.signal();
        }
    }
    int get_data() {
        monitor {
            if (m_count == 0)
                m_empty.wait();
            int result = m_data[m_tail];
            m_tail = (m_tail + 1) % B;
            m_count -= 1;
            m_full.signal();
            return result;
        }
    }
} buffer;


void producer() {
    while (true) {
        // Produce data ...
        int data = rand() % 1000;
        // Put data into buffer
        buffer.put_data(data);
    }
}

void consumer() {
    while (true) {
        // Get data from buffer
        int data = buffer.get_data();
        // Consume data ...
        print("%d ", data);
    }
}

int main() {
    Thread tp = createThread(producer);
    Thread tc = createThread(consumer);
    join(tp);
    join(tc);
    return 0;
}
