#include <process.h>
#include <msgbox.h>
#include <cstdlib>
#define B 10

struct msg_t {
    int data;
};

mbx_t buffer;
mbx_t slots;

void producer() {
    int st;
    msg_t slot;
    while (true) {
        // Produce data...
        int data = rand() % 1000;

        // Put data into buffer
        mbx_get(slot, slots, INF, &st);
        msg_t msg;
        msg.data = data;
        mbx_put(msg, buffer);
    }
}

void consumer() {
    int st;
    msg_t msg;
    while (true) {
        // Get data from buffer
        msg_get(msg, buffer, INF, &st);
        int data = msg.data;
        mbx_put(msg, slots);

        // Consume data...
        print("%d ", data);
    }
}

int main() {
    msg_t slot;
    for (int i = 0; i < B; i++)
        mbx_put(slot, slots);
    Process tp = createProcess(producer);
    Process tc = createProcess(consumer);
    waitFor(tp);
    waitFor(tc);
    return 0;
}
