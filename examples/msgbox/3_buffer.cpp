#include <process.h>
#include <msgbox.h>
#include <cstdlib>

struct msg_t {
    int data;
};

mbx_t buffer;

void producer() {
    while (true) {
        // Produce data...
        int data = rand() % 1000;

        // Put data into buffer
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

        // Consume data...
        print("%d ", data);
    }
}

int main() {
    Process tp = createProcess(producer);
    Process tc = createProcess(consumer);
    waitFor(tp);
    waitFor(tc);
    return 0;
}
