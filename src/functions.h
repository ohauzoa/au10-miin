#ifndef _MIIN_PROJECT_FUNCTIONS_H
#define _MIIN_PROJECT_FUNCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <signal.h>
#include <time.h>

#define SIBEOK 1000000000
#define BAEKMAN 1000000
#define CHEON 1000
#define ILL 1
#define COUNT_WIDTH 14


int int2comma(char *str, int num, int width);
int create_timer( timer_t *timerID, void (*func)(int, siginfo_t *, void *), int interval, int mode );



#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
