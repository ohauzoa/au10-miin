
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <time.h>

#include "functions.h"



static int abs_int(int *n)
{/* n is minus then return 1, not then return 0 */
    int sign;
 
    sign = ((*n < 0)? 1 : 0);
    *n = ((*n < 0)? -*n : *n);
    
    return sign;
}
 
static int check_width(int num, int width)
{/* too narrow return 1 not 0, no count null size */
    int sign, i;
    
    sign = abs_int(&num);
    
    for (i = 1; num > 1; num /= 10, i++);
    i += ((i-1) / 3) + sign;
 
    return (width < i)? 1 : 0;
}
 
int int2comma(char *str, int num, int width)
{/* on success return 0 , not return -1 */
    int sign, i;
    char num_str[15] = "\0";
    char *ptr = num_str;
//    memset(ptr, 0, 15);
    if (check_width(num, width)) {
        sprintf(str, "narrow");
        return -1;
    }
    
    sign = abs_int(&num);
 
    sprintf(num_str,    "%02d,", (num - (num - ( num / SIBEOK) * SIBEOK  )) / SIBEOK);
    sprintf(num_str+3,    "%03d,", (num - num / SIBEOK * SIBEOK) / BAEKMAN);
    sprintf(num_str+7,    "%03d,", (num - num / BAEKMAN * BAEKMAN) / CHEON);
    sprintf(num_str+11,    "%03d", (num - num / CHEON * CHEON) / ILL);
 
    for (;*ptr == '0' || *ptr == ','; ptr++) *ptr = ' ';
    if (sign == 1) *(--ptr) = '-';
    ptr = num_str + 14 - width;
    for (i = 0; i < width; i++, ptr++) str[i] = *ptr;
    return 0;
}

int create_timer( timer_t *timerID, void (*func)(int, siginfo_t *, void *), int interval, int mode )  
{  
    struct sigevent         te;  
    struct itimerspec       its;  
    struct sigaction        sa;  
    int                     sigNo = SIGRTMIN;  
    int sec, msec;
    // tv_sec는 값이 1,000,000 넘으면 에러발생.
    sec = interval / 1000;
    msec = (interval % 1000) * 1000000;

    sa.sa_flags = SA_SIGINFO;  
    sa.sa_sigaction = func;     // 타이머 호출시 호출할 함수 
    sigemptyset(&sa.sa_mask);  
  
    if (sigaction(sigNo, &sa, NULL) == -1)  
    {  
        printf("sigaction error\n");
        return -1;  
    }  
   
    /* Set and enable alarm */  
    te.sigev_notify = SIGEV_SIGNAL;  
    te.sigev_signo = sigNo;  
    te.sigev_value.sival_ptr = timerID;  
    timer_create(CLOCK_REALTIME, &te, timerID);  
   
    its.it_value.tv_sec = sec;
    its.it_value.tv_nsec = msec;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    // mode 0 : oneshot, 1 :  periodic
    if(mode == 1){
        its.it_interval.tv_sec = sec;
        its.it_interval.tv_nsec = msec; 
    }

    timer_settime(*timerID, 0, &its, NULL);  
   
    return 0;  
}

