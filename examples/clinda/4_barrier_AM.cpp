/**
 * (c) Ivan Janevski
 */
#include <thread.h>
#include <clinda.h>
#include <map>
#include <string>
#include <queue>
#define N 5
enum {
    BAR_1,
    BAR_2
};

void server() {
    std::map<int, std::queue<int>> qDelay;
    while (true) {
        int id, bar;
        in("request", &id, &bar);
        qDelay[bar].push(id);
        if (qDelay[bar].size() == N)  {
            while (! qDelay[bar].empty()) {
                int pId = qDelay[bar].front();
                qDelay[bar].pop();
                out("response", pId);
            }
        }
    }
}

void a(int id) {
    while (true) {
        out("request", id, BAR_1);
        in("response", id);
        print("\e[%dm %d ", 31 + id, id);

        out("request", id, BAR_2);
        in("response", id);
        print("\e[7m\e[%dm %d ", 31 + id, id);
    }
}


int main() {
    eval(server());
    for (int i = 0; i < N; i++) 
        eval(a(i));
    return 0;
}
