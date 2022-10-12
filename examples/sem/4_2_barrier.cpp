// Copyright (c) Ivan Janevski
#include <thread.h>
#include <sem.h>
#define N 5

struct Barrier {
    Barrier(int needed) : m_needed(needed) {
        // nothing
    }
	void bar_wait() {
		wait(m_mutex);
		m_count += 1;
		if (m_count == N) {
			m_count = 0;
			for (int i = 0; i < N; i++)
				signal(m_barrier);
		}
		signal(m_mutex);
		wait(m_barrier);
	}
private:
    int m_needed;
	int m_count = 0;
	Sem m_mutex = 1;
	Sem m_barrier = 0;
} bar1(N), bar2(N);

Sem mutex = 1;

void a(int id) {
    while (true) {
		/* SECTION 1 */
		wait(mutex);
        print("\e[%dm %d ", 31 + id, id);
		signal(mutex);
		bar1.bar_wait();
		
		/* SECTION 2 */
		wait(mutex);
        print(REV "\e[%dm %d ", 31 + id, id);
		signal(mutex);
		bar2.bar_wait();
    }
}

int main() {
    Thread t[N];
    for (int i = 0; i < N; i++) 
        t[i] = createThread(a, i);
    for (int i = 0; i < N; i++)
        join(t[i]);
    return 0;
}
