#include "sound.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "ui_main.h"

#define SOUND_FIFO_SIZE 12

struct sound_umsg {
    int tid;
    int flag;
    int cnt;
    char msg[SOUND_FIFO_SIZE];
};
static struct sound_umsg msound;

//static int msg_sound;
// Message queue
static key_t id_sound;

static pthread_t _sound_thread;

void send_sound(int cmd)
{
    int cnt = 0;
    char tmp[10];

    memset(msound.msg, 0, SOUND_FIFO_SIZE);
    switch(cmd)
    {
        case 0:
            break;
    }

//    msound.msg[cnt++] = PACKET_STX;
//    msound.msg[cnt++] = PACKET_ID_APP;
//    msound.msg[cnt++] = cmd;

    msound.tid = cmd;
    msound.cnt = cnt;
    msound.flag = 0;

    if(msgsnd(id_sound, (void *)&msound, cnt, IPC_NOWAIT) < 0) printf("msgsnd error %d\n", cmd);
}

static void *sound_thread(void *arg)
{
    int count = 0;
    int result = 0;
    uint8_t str[SOUND_FIFO_SIZE]={0,}; // {PACKET_STX, cmd1, cmd2, data, chksum1, chksum2, PACKET_ETX};
    struct sound_umsg tmp;

	while(1) {
        result = msgrcv(id_sound, (void *)&msound, (size_t)0, (1 >> 1), 0);
        if(result == -1) continue;

        switch(msound.tid)
        {
            case SOUND_MIIN:
                system("tinyplayer tts/miin.mp3 &");
                break;
            case SOUND_BRIGHTONE:
                system("tinyplayer tts/brightone.mp3 &");
                break;
            case SOUND_PEAKLIGHT:
                system("tinyplayer tts/peaklight.mp3 &");
                break;
            case SOUND_AUTO:
                system("tinyplayer tts/auto.mp3 &");
                break;
            case SOUND_MANUAL:
                system("tinyplayer tts/manual.mp3 &");
                break;
            case SOUND_READY:
                system("tinyplayer tts/ready.mp3 &");
                break;
            case SOUND_STANDBY:
                system("tinyplayer tts/standby.mp3 &");
                break;
            case SOUND_WARNING_HANDPIECE:
                system("tinyplayer tts/warning_handpiece.mp3 &");
                break;
            case SOUND_SHOTCOUNT_TOTAL:
                system("tinyplayer tts/shotcount_total.mp3 &");
                break;
            case SOUND_SHOTCOUNT_CLEAR:
                system("tinyplayer tts/shotcount_clear.mp3 &");
                break;
            case SOUND_LOAD_MEMORY1:
                system("tinyplayer tts/load_memory1.mp3 &");
                break;
            case SOUND_LOAD_MEMORY2:
                system("tinyplayer tts/load_memory2.mp3 &");
                break;
            case SOUND_LOAD_MEMORY3:
                system("tinyplayer tts/load_memory3.mp3 &");
                break;
            case SOUND_SAVE_MEMORY1:
                system("tinyplayer tts/save_memory1.mp3 &");
                break;
            case SOUND_SAVE_MEMORY2:
                system("tinyplayer tts/save_memory2.mp3 &");
                break;
            case SOUND_SAVE_MEMORY3:
                system("tinyplayer tts/save_memory3.mp3 &");
                break;
            case SOUND_MEMORY_MODE:
                system("tinyplayer tts/memory_mode.mp3 &");
                break;
            case SOUND_EASY_MODE:
                system("tinyplayer tts/easy_mode.mp3 &");
                break;
            case SOUND_0SEC:
                system("tinyplayer tts/0sec.mp3 &");
                break;
            case SOUND_30SEC:
                system("tinyplayer tts/30sec.mp3 &");
                break;
            case SOUND_60SEC:
                system("tinyplayer tts/1min.mp3 &");
                break;
            case SOUND_90SEC:
                system("tinyplayer tts/1min30sec.mp3 &");
                break;
            case SOUND_120SEC:
                system("tinyplayer tts/2min.mp3 &");
                break;
            case SOUND_THIS_VOICE:
                system("tinyplayer tts/this_voice.mp3 &");
                break;
        }



        
	} // while
}



void init_sound(void)
{
    // 메시지큐 생성
    id_sound = msgget((key_t)8888, IPC_CREAT|0666);

    if (id_sound == -1)
    {
        printf("msgget error : id_sound\n");
    }
    memset(msound.msg, 0, SOUND_FIFO_SIZE);


    pthread_create(&_sound_thread, NULL, sound_thread, NULL);
}


void close_sound(void)
{
    msgctl( id_sound, IPC_RMID, 0);
//    pthread_join(_uart_tx_thread, (void**)NULL);	// 해당 스레드가 종료되길 기다린다.
//    pthread_join(_uart_rx_thread, (void**)NULL);	// 해당 스레드가 종료되길 기다린다.
}
