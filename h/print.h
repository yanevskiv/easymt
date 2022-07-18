#ifndef _PRINT_H_
#define _PRINT_H_
#include <stdio.h>
#define RED       "\e[31m"
#define GREEN     "\e[32m"
#define YELLOW    "\e[33m"
#define BLUE      "\e[34m"
#define MAGENTA   "\e[35m"
#define BGRED     "\e[41m"
#define BGGREEN   "\e[42m"
#define BGYELLOW  "\e[43m"
#define BGBLUE    "\e[44m"
#define BGMAGENTA "\e[45m"
#define REV       "\e[7m"
#define NONE      "\e[0m"

#define println(...) \
    do { \
        printf(__VA_ARGS__); \
        printf("\n" NONE); \
        fflush(stdout); \
    } while (0)
//#define debug(...)
#define debug(...) \
    do { \
        printf(MAGENTA); \
        printf(__VA_ARGS__); \
        printf("\n" NONE); \
        fflush(stdout); \
    } while (0)

#define print(...) \
    do { \
        printf(__VA_ARGS__); \
        printf(NONE); \
        fflush(stdout); \
    } while (0)

#endif
