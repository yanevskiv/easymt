/**
 * (c) Ivan Janevski
 */
#include <print.h>
#include <clinda.h>
#include <stdlib.h>
enum {
    PAPER,
    TOBACCO,
    MATCHES,
};

void agent() {
    while (true) {
        int item = rand() % 3;
        switch (item) {
            case PAPER: {
                println(RED "Agent putting TOBACCO and MATCHES");
                out("tobacco");
                out("matches");
                out("wait(Matches, Tobacco)");
                in("ok");
            } break;
            case TOBACCO: {
                println(YELLOW "Agent putting PAPER and MATCHES");
                out("paper");
                out("matches");
                out("wait(Paper, Matches)");
                in("ok");
            } break;
            case MATCHES: {
                println(GREEN "Agent putting TOBACCO and PAPER");
                out("tobacco");
                out("matches");
                out("wait(Tobacco, Paper)");
                in("ok");
            } break;
        }
    }
}

void smoker(int item)  {
    while (true) {
        switch (item) {
            case PAPER: {
                out("wait(Matches, Tobacco)");
                in("matches");
                in("tobacco");
                println(RED "Smoker taking TOBACCO and MATCHES");
                out("ok");
            } break;
            case TOBACCO: {
                out("wait(Paper, Matches)");
                in("paper");
                in("matches");
                println(YELLOW "Smoker taking PAPER and MATCHES");
                out("ok");
            } break;
            case MATCHES: {
                out("wait(Tobacco, Paper)");
                in("tobacco");
                in("paper");
                println(GREEN "Smoker taking TOBACCO and PAPER");
                out("ok");
            } break;
        }
    }
}

int main() {
    eval(agent());
    eval(smoker(PAPER));
    eval(smoker(TOBACCO));
    eval(smoker(MATCHES));
    return 0;
}
