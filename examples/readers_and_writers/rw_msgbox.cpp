#include <process.h>
#include <msgbox.h>
#include <queue>
using namespace std;
#define NR 20
#define NW 2 


mbx_t boxServer;
mbx_t boxReader[NR];
mbx_t boxWriter[NW];

struct msg_t {
    int m_id;
    int m_type;
    int m_action;
};

enum {
    READER,
    WRITER,
};
enum {
    LOCK,
    UNLOCK
};


void server() {
    msg_t rec, snd;
    int wCount = 0;
    int rCount = 0;
    typedef pair<int, int> PAIR;
    queue<PAIR> qWait;
    while (true) {
        msg_get(rec, boxServer, INF, 0);
        int id = rec.m_id;
        int type = rec.m_type;
        int action = rec.m_action;
        switch (action) {
            case LOCK: {
                int shouldDelay = ! qWait.empty()
                    || type == READER && wCount > 0
                    || type == WRITER && (wCount > 0 || rCount > 0);
                if (shouldDelay) {
                    qWait.push(PAIR(type, id));
                } else if (type == WRITER) {
                    wCount++;
                    mbx_put(snd, boxWriter[id]);
                } else if (type == READER) {
                    rCount++;
                    mbx_put(snd, boxReader[id]);
                }
            } break;
            case UNLOCK: {
                if (type == WRITER) {
                    wCount--;
                    mbx_put(snd, boxWriter[id]);
                } else if (type == READER) {
                    rCount--;
                    mbx_put(snd, boxReader[id]);
                }
            } break;
        }
        while (! qWait.empty())  {
            PAIR p = qWait.front();
            int pType = p.first;
            int pId = p.second;
            if (pType == WRITER && wCount == 0 && rCount == 0) {
                wCount++;
                qWait.pop();
                mbx_put(snd, boxWriter[pId]);
            } else if (pType == READER && wCount == 0) {
                rCount++;
                qWait.pop();
                mbx_put(snd, boxReader[pId]);
            } else break;
        }
    }
}

void reader(int id)
{
    msg_t rec, snd;
    while (true) {
        snd.m_id = id;
        snd.m_type = READER;
        snd.m_action = LOCK;
        msg_put(snd, boxServer);
        msg_get(rec, boxReader[id], INF, 0);

        print(GREEN "r" NONE);

        snd.m_id = id;
        snd.m_type = READER;
        snd.m_action = UNLOCK;
        msg_put(snd, boxServer);
        msg_get(rec, boxReader[id], INF, 0);
    }
}

void writer(int id)
{
    msg_t rec, snd;
    while (true) {
        snd.m_id = id;
        snd.m_type = WRITER;
        snd.m_action = LOCK;
        msg_put(snd, boxServer);
        msg_get(rec, boxWriter[id], INF, 0);

        print(RED "w" NONE);

        snd.m_id = id;
        snd.m_type = WRITER;
        snd.m_action = UNLOCK;
        msg_put(snd, boxServer);
        msg_get(rec, boxWriter[id], INF, 0);
    }
}

int main() {
    Process ps = createProcess(server);
    Process pr[NR], pw[NW];
    for (int i = 0; i < NR; i++)
        pr[i] = createProcess(reader, i);
    for (int i = 0; i < NW; i++) 
        pw[i] = createProcess(writer, i);
    for (int i = 0; i < NR; i++) 
        waitFor(pr[i]);
    for (int i = 0; i < NW; i++) 
        waitFor(pw[i]);
}
