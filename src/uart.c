#include "uart.h"
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
#include "ringbuf.h"
#include "ui_main.h"

#define COMM_UART "/dev/ttyS5"

struct umsg {
    int tid;
    int flag;
    int cnt;
    char msg[AT_CLI_FIFO_SIZE];
};
static struct umsg mcom;

static sem_t sem_comm;
static int msg_comm;
// Message queue
static key_t id_comm;

static pthread_t _uart_rx_thread;
static pthread_t _uart_tx_thread;

static int uart_fd;
static ringbuf_t rb;
static uint8_t _str[AT_CLI_FIFO_SIZE] = {0,};


int open_uart(char *dev_name, int baud, int vtime, int vmin)
{
    int fd;
    struct termios newtio;

    fd = open(dev_name, O_RDWR | O_NOCTTY);

    if(fd < 0) return -1;

    memset(&newtio, 0, sizeof(newtio));

    newtio.c_oflag = 0;
    newtio.c_cflag = CS8 | CLOCAL | CREAD | PARENB; 

    switch(baud)
    {
        case 230400:
            newtio.c_cflag |= B230400;
            break;
        case 115200:
            newtio.c_cflag |= B115200;
            break;
        default:
            newtio.c_cflag |= B9600;
            break;
    }

    newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    newtio.c_cc[VTIME] = vtime;         // timeout 0.1초 단위
    newtio.c_cc[VMIN]  = vmin;          // 최소 n 문자 받을 때까진 대기
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);

    return fd;
}

static uint8_t checksum(uint8_t packet[], int length)
{
    uint8_t ret = 0;
    for(int i = 0; i < length ; i++)
    {
        ret ^= packet[i];
    }
    return ret;
}

void make_qdelay(void)
{
    switch(config.wavelength)
    {
        case WAVE_1064S: case WAVE_532: 
            config.qdelay1 = calibrate.qdelay[0];
            config.qdelay2 = 0;
            break;

        case WAVE_1064D:
            config.qdelay1 = calibrate.qdelay[1];
            config.qdelay2 = calibrate.qdelay[2];
            break;

        case WAVE_MiiN:
            if(config.energy >= 1500){
                config.qdelay1 = calibrate.qdelay[3];
                config.qdelay2 = calibrate.qdelay[4];
            } else {
                config.qdelay1 = calibrate.qdelay[5];
                config.qdelay2 = calibrate.qdelay[6];
            }
            break;
        default:
            config.qdelay1 = config.qdelay2 = 0;
            break;
    }
}

void send_txd(int cmd)
{
    int cnt = 0;
    uint8_t chksum = 0;
    char tmp[10];

    memset(mcom.msg, 0, AT_CLI_FIFO_SIZE);

    mcom.msg[cnt++] = PACKET_STX;
    mcom.msg[cnt++] = PACKET_ID_APP;
    mcom.msg[cnt++] = cmd;

    switch(cmd)
    {
        case PACKET_RESPONSE_HANDPIECE:
            sprintf(tmp, "%02d", config.handpiece_rx);
            mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1];
            break;

        case PACKET_RESPONSE_ERR_CODE:
            sprintf(tmp, "%02d", config.err_code);
            mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1];
            break;

        case PACKET_RESPONSE_SHOTCOUNT:
            sprintf(tmp, "%08d", config.shotcount);
            for(int i = 0; i < 8; i++) mcom.msg[cnt++] = tmp[i];
            break;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case PACKET_COMMAND_QDELAY:
            for(int i = 0; i < 7; i++){
                sprintf(tmp, "%03d", calibrate.qdelay[i]);
                mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1];  mcom.msg[cnt++] = tmp[2];
            }
            break;

        case PACKET_COMMAND_V1064S:
            for(int i = 0; i < 14; i++){
                sprintf(tmp, "%04d", calibrate.voltage[WAVE_1064S][i]);
                mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1];  mcom.msg[cnt++] = tmp[2];  mcom.msg[cnt++] = tmp[3];
            }
            break;

        case PACKET_COMMAND_V1064D:
            for(int i = 0; i < 12; i++){
                sprintf(tmp, "%04d", calibrate.voltage[WAVE_1064D][i]);
                mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1];  mcom.msg[cnt++] = tmp[2];  mcom.msg[cnt++] = tmp[3];
            }
            break;

        case PACKET_COMMAND_V1064M:
            for(int i = 0; i < 13; i++){
                sprintf(tmp, "%04d", calibrate.voltage[WAVE_MiiN][i]);
                mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1];  mcom.msg[cnt++] = tmp[2];  mcom.msg[cnt++] = tmp[3];
            }
            break;

        case PACKET_COMMAND_V532:
            for(int i = 0; i < 15; i++){
                sprintf(tmp, "%04d", calibrate.voltage[WAVE_532][i]);
                mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1];  mcom.msg[cnt++] = tmp[2];  mcom.msg[cnt++] = tmp[3];
            }
            break;

        case PACKET_COMMAND_VQUASI:
            for(int i = 0; i < 11; i++){
                sprintf(tmp, "%04d", calibrate.voltage[WAVE_QUASI][i]);
                mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1];  mcom.msg[cnt++] = tmp[2];  mcom.msg[cnt++] = tmp[3];
            }
            break;

        case PACKET_COMMAND_READY:
            sprintf(tmp, "%01d", config.ready);
            mcom.msg[cnt++] = tmp[0];
            sprintf(tmp, "%02d", config.frequency);
            mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1];
            sprintf(tmp, "%02d", config.wavelength);
            mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1];

            make_qdelay();
            sprintf(tmp, "%03d", config.qdelay1);
            mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1]; mcom.msg[cnt++] = tmp[2];
            sprintf(tmp, "%03d", config.qdelay2);
            mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1]; mcom.msg[cnt++] = tmp[2];

            sprintf(tmp, "%04d", config.aiming);
            mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1]; mcom.msg[cnt++] = tmp[2];mcom.msg[cnt++] = tmp[3];

//            sprintf(tmp, "%02d", config.spotsize);
//            mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1];

            sprintf(tmp, "%04d", config.voltage);
            mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1]; mcom.msg[cnt++] = tmp[2];mcom.msg[cnt++] = tmp[3];
            sprintf(tmp, "%04d", config.energy);
            mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1]; mcom.msg[cnt++] = tmp[2];mcom.msg[cnt++] = tmp[3];
            break;

        case PACKET_COMMAND_AIMING:
            sprintf(tmp, "%04d", config.aiming);
            mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1]; mcom.msg[cnt++] = tmp[2];mcom.msg[cnt++] = tmp[3];
            break;

        case PACKET_COMMAND_VOLUME:
            sprintf(tmp, "%02d", config.volume);
            mcom.msg[cnt++] = tmp[0]; mcom.msg[cnt++] = tmp[1];
            break;

        case PACKET_COMMAND_STANDBY:
            sprintf(tmp, "%01d", config.ready);
            mcom.msg[cnt++] = tmp[0];
            break;

        case PACKET_COMMAND_SHOTCOUNT_ZERO:
            sprintf(tmp, "%01d", config.shotcount);
            mcom.msg[cnt++] = tmp[0];
            break;
    }

    chksum = checksum(mcom.msg, cnt);
    sprintf(tmp, "%02X", chksum);

    mcom.msg[cnt++] = tmp[0];
    mcom.msg[cnt++] = tmp[1];
    mcom.msg[cnt++] = PACKET_ETX;

    mcom.tid = cmd;
    mcom.cnt = cnt;
    mcom.flag = 0;

    if(msgsnd(id_comm, (void *)&mcom, cnt, IPC_NOWAIT) < 0) printf("msgsnd error %d\n", cmd);
}

static void analysis_packet(uint8_t buf[])
{
    char tmp[10];
    int val[20];
    int flag;
    int cnt = 3;

    switch(buf[2]){
        case PACKET_COMMAND_HANDPIECE:
            sprintf(tmp, "%c%c", buf[cnt+0], buf[cnt+1]);
            cnt += 2;
            config.handpiece_rx = atoi(tmp);
            //if(val[0] != config.handpiece) mcom.flag++;
            printf("handpiece_rx->%d\n", config.handpiece_rx);
            send_txd(PACKET_RESPONSE_HANDPIECE);
            break;

        case PACKET_COMMAND_ERR_CODE:
            sprintf(tmp, "%c%c", buf[cnt+0], buf[cnt+1]);
            cnt += 2;
            config.err_code = atoi(tmp);
            //if(val[0] != config.err_code) mcom.flag++;
            printf("ERR_CODE ->%d\n", config.err_code);
            send_txd(PACKET_RESPONSE_ERR_CODE);
            break;

        case PACKET_COMMAND_SHOTCOUNT:
            sprintf(tmp, "%c%c%c%c%c%c%c%c", buf[cnt+0], buf[cnt+1], buf[cnt+2], buf[cnt+3], buf[cnt+4], buf[cnt+5], buf[cnt+6], buf[cnt+7]);
            cnt += 8;
            config.shotcount = atoi(tmp);
            draw_txtCount();
            //if(val[0] != config.shotcount) mcom.flag++;
            printf("shot count ->%d\n", config.shotcount);
            product.total++;
            product.power++;
            product.lamp++;
            send_txd(PACKET_RESPONSE_SHOTCOUNT);
            break;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case PACKET_RESPONSE_QDELAY:
            for(int i = 0; i < 7; i++){
                sprintf(tmp, "%c%c%c", buf[(i*3)+3], buf[(i*3)+4], buf[(i*3)+5]);
                val[i] = atoi(tmp);
                printf("rx qdelay[%d]->%d\n", calibrate.qdelay[i], val[i]);
                if(val[i] != calibrate.qdelay[i]) mcom.flag++;
            }
            mcom.tid = PACKET_RESPONSE_QDELAY;
            mcom.cnt = 0;
            if(msgsnd(id_comm, (void *)&mcom, 0, IPC_NOWAIT) < 0) printf("msgsnd error %d\n", mcom.tid);
            break;

        case PACKET_RESPONSE_V1064S:
            for(int i = 0; i < 14; i++){
                sprintf(tmp, "%c%c%c%c", buf[(i*4)+3], buf[(i*4)+4], buf[(i*4)+5], buf[(i*4)+6]);
                val[i] = atoi(tmp);
                printf("rx v1064s[%d]->%d\n", calibrate.voltage[WAVE_1064S][i], val[i]);
                if(val[i] != calibrate.voltage[WAVE_1064S][i]) mcom.flag++;
            }
            mcom.tid = PACKET_RESPONSE_V1064S;
            mcom.cnt = 0;
            if(msgsnd(id_comm, (void *)&mcom, 0, IPC_NOWAIT) < 0) printf("msgsnd error %d\n", mcom.tid);
            break;

        case PACKET_RESPONSE_V1064D:
            for(int i = 0; i < 12; i++){
                sprintf(tmp, "%c%c%c%c", buf[(i*4)+3], buf[(i*4)+4], buf[(i*4)+5], buf[(i*4)+6]);
                val[i] = atoi(tmp);
                printf("rx v1064d[%d]->%d\n", calibrate.voltage[WAVE_1064D][i], val[i]);
                if(val[i] != calibrate.voltage[WAVE_1064D][i]) mcom.flag++;
            }
            mcom.tid = PACKET_RESPONSE_V1064D;
            mcom.cnt = 0;
            if(msgsnd(id_comm, (void *)&mcom, 0, IPC_NOWAIT) < 0) printf("msgsnd error %d\n", mcom.tid);
            break;

        case PACKET_RESPONSE_V1064M:
            for(int i = 0; i < 13; i++){
                sprintf(tmp, "%c%c%c%c", buf[(i*4)+3], buf[(i*4)+4], buf[(i*4)+5], buf[(i*4)+6]);
                val[i] = atoi(tmp);
                printf("rx v1064m[%d]->%d\n", calibrate.voltage[WAVE_MiiN][i], val[i]);
                if(val[i] != calibrate.voltage[WAVE_MiiN][i]) mcom.flag++;
            }
            mcom.tid = PACKET_RESPONSE_V1064M;
            mcom.cnt = 0;
            if(msgsnd(id_comm, (void *)&mcom, 0, IPC_NOWAIT) < 0) printf("msgsnd error %d\n", mcom.tid);
            break;

        case PACKET_RESPONSE_V532:
            for(int i = 0; i < 15; i++){
                sprintf(tmp, "%c%c%c%c", buf[(i*4)+3], buf[(i*4)+4], buf[(i*4)+5], buf[(i*4)+6]);
                val[i] = atoi(tmp);
                printf("rx v532[%d]->%d\n", calibrate.voltage[WAVE_532][i], val[i]);
                if(val[i] != calibrate.voltage[WAVE_532][i]) mcom.flag++;
            }
            mcom.tid = PACKET_RESPONSE_V532;
            mcom.cnt = 0;
            if(msgsnd(id_comm, (void *)&mcom, 0, IPC_NOWAIT) < 0) printf("msgsnd error %d\n", mcom.tid);
            break;

        case PACKET_RESPONSE_VQUASI:
            for(int i = 0; i < 11; i++){
                sprintf(tmp, "%c%c%c%c", buf[(i*4)+3], buf[(i*4)+4], buf[(i*4)+5], buf[(i*4)+6]);
                val[i] = atoi(tmp);
                printf("rx vquasi[%d]->%d\n", calibrate.voltage[WAVE_QUASI][i], val[i]);
                if(val[i] != calibrate.voltage[WAVE_QUASI][i]) mcom.flag++;
            }
            mcom.tid = PACKET_RESPONSE_VQUASI;
            mcom.cnt = 0;
            if(msgsnd(id_comm, (void *)&mcom, 0, IPC_NOWAIT) < 0) printf("msgsnd error %d\n", mcom.tid);
            break;

        case PACKET_RESPONSE_READY:
            // ready
            sprintf(tmp, "%c", buf[cnt+0]);
            cnt += 1;
            val[0] = atoi(tmp);
            if(val[0] != config.ready) mcom.flag++;
            printf("ready[%d]->%d\n", config.ready, val[0]);
            // frequency
            sprintf(tmp, "%c%c", buf[cnt+0], buf[cnt+1]);
            cnt += 2;
            val[0] = atoi(tmp);
            if(val[0] != config.frequency) mcom.flag++;
            printf("frequency[%d]->%d\n", config.frequency, val[0]);
            // wavelength
            sprintf(tmp, "%c%c", buf[cnt+0], buf[cnt+1]);
            cnt += 2;
            val[0] = atoi(tmp);
            if(val[0] != config.wavelength) mcom.flag++;
            printf("wavelength[%d]->%d\n", config.wavelength, val[0]);

            // Q delay 1
            sprintf(tmp, "%c%c%c", buf[cnt+0], buf[cnt+1], buf[cnt+2]);
            cnt += 3;
            val[0] = atoi(tmp);
            if(val[0] != config.qdelay1) mcom.flag++;
            printf("qdelay1 [%d]->%d\n", config.qdelay1, val[0]);

            // Q delay 2
            sprintf(tmp, "%c%c%c", buf[cnt+0], buf[cnt+1], buf[cnt+2]);
            cnt += 3;
            val[0] = atoi(tmp);
            if(val[0] != config.qdelay2) mcom.flag++;
            printf("qdelay2 [%d]->%d\n", config.qdelay1, val[0]);

            // aiming
            sprintf(tmp, "%c%c%c%c", buf[cnt+0], buf[cnt+1], buf[cnt+2], buf[cnt+3]);
            cnt += 4;
            val[0] = atoi(tmp);
            if(val[0] != config.aiming) mcom.flag++;
            printf("aiming [%d]->%d\n", config.aiming, val[0]);

            // handpeice
            sprintf(tmp, "%c%c", buf[cnt+0], buf[cnt+1]);
            cnt += 2;
            config.handpiece_rx = atoi(tmp);
            printf("handpiece_rx->%d\n", config.handpiece_rx);
            // val[0] = atoi(tmp);
           //if(val[0] != config.spotsize) mcom.flag++;
            //printf("spot size[%d]->%d\n", config.spotsize, val[0]);

            // voltage
            sprintf(tmp, "%c%c%c%c", buf[cnt+0], buf[cnt+1], buf[cnt+2], buf[cnt+3]);
            cnt += 4;
            val[0] = atoi(tmp);
            if(val[0] != config.voltage) mcom.flag++;
            printf("voltage[%d]->%d\n", config.voltage, val[0]);
            // energy
            sprintf(tmp, "%c%c%c%c", buf[cnt+0], buf[cnt+1], buf[cnt+2], buf[cnt+3]);
            cnt += 4;
            val[0] = atoi(tmp);
            if(val[0] != config.energy) mcom.flag++;
            printf("energy[%d]->%d\n", config.energy, val[0]);

            mcom.tid = PACKET_RESPONSE_READY;
            mcom.cnt = 0;
            if(msgsnd(id_comm, (void *)&mcom, 0, IPC_NOWAIT) < 0) printf("msgsnd error %d\n", mcom.tid);
            break;

        case PACKET_RESPONSE_AIMING:
            // aiming
            sprintf(tmp, "%c%c%c%c", buf[cnt+0], buf[cnt+1], buf[cnt+2], buf[cnt+3]);
            cnt += 4;
            val[0] = atoi(tmp);
            if(val[0] != config.aiming) mcom.flag++;
            printf("aiming[%d]->%d\n", config.aiming, val[0]);

            mcom.tid = PACKET_RESPONSE_AIMING;
            mcom.cnt = 0;
            if(msgsnd(id_comm, (void *)&mcom, 0, IPC_NOWAIT) < 0) printf("msgsnd error %d\n", mcom.tid);
            break;

        case PACKET_RESPONSE_VOLUME:
            // aiming
            sprintf(tmp, "%c%c", buf[cnt+0], buf[cnt+1]);
            cnt += 2;
            val[0] = atoi(tmp);
            if(val[0] != config.volume) mcom.flag++;
            printf("volume[%d]->%d\n", config.volume, val[0]);

            mcom.tid = PACKET_RESPONSE_VOLUME;
            mcom.cnt = 0;
            if(msgsnd(id_comm, (void *)&mcom, 0, IPC_NOWAIT) < 0) printf("msgsnd error %d\n", mcom.tid);
            break;

        case PACKET_RESPONSE_STANDBY:
            // ready
            sprintf(tmp, "%c", buf[3]);
            val[0] = atoi(tmp);
            if(val[0] != config.ready) mcom.flag++;
            printf("standby[%d]->%d\n", config.ready, val[0]);

            mcom.tid = PACKET_RESPONSE_STANDBY;
            mcom.cnt = 0;
            if(msgsnd(id_comm, (void *)&mcom, 0, IPC_NOWAIT) < 0) printf("msgsnd error %d\n", mcom.tid);
            break;

        case PACKET_RESPONSE_SHOTCOUNT_ZERO:
            // ready
            sprintf(tmp, "%c", buf[3]);
            val[0] = atoi(tmp);
            if(val[0] != config.shotcount) mcom.flag++;
            printf("shot count[%d]->%d\n", config.shotcount, val[0]);

            mcom.tid = PACKET_RESPONSE_STANDBY;
            mcom.cnt = 0;
            if(msgsnd(id_comm, (void *)&mcom, 0, IPC_NOWAIT) < 0) printf("msgsnd error %d\n", mcom.tid);
            break;

    }
}

static void analysis_rb(void)
{
    uint8_t chksum = 0;
    int nn = 0;
    uint8_t tmp[4];
    int value = 0;
    int result = ringbuf_bytes_used(rb);
    ringbuf_memcpy_from(_str, rb, result);

    for(int i = 0; i < result; i++){
        if(_str[i] == PACKET_ETX){
            sprintf(tmp, "%c%c", _str[i-2], _str[i-1]);
            nn = (int)strtol(tmp, NULL, 16);
            chksum = checksum(_str, i - 2);
            if(nn == chksum){
                analysis_packet(_str);
            }
            else{
                printf("check sum error !\n");
            }
//            printf( "crc=%02x, %02x, %d\n", nn, chksum, i);
        }
    }
}

static void *rx_thread(void *arg)
{
    fd_set read_fds;
    int16_t res;
    uint8_t buf[AT_CLI_FIFO_SIZE];
    uint8_t tmp[3];
    int result = 0;
    int step = MSTATE_IDLE;
    int value = 0;
    uint16_t crc;
    uint16_t crc_rx;
    rb = ringbuf_new(AT_CLI_FIFO_SIZE);

	while(1) {
        FD_ZERO(&read_fds);
        FD_SET(uart_fd, &read_fds);                 
        res = select(uart_fd+1, &read_fds, NULL, NULL, NULL);
        result = read(uart_fd,  buf, sizeof(buf) );  //uart에 rx 발생 시 호출
        if(result < 1) continue;

        ringbuf_memcpy_into(rb, buf, result);

        for(int i = 0; i < result; i++){
//          printf("%c\n", buf[i]);
            if(buf[i] == PACKET_ETX){
                analysis_rb();
            }
        }
	} // while
}

static void *tx_thread(void *arg)
{
    int count = 0;
    int result = 0;
    uint8_t str[AT_CLI_FIFO_SIZE]={0,}; // {PACKET_STX, cmd1, cmd2, data, chksum1, chksum2, PACKET_ETX};
    struct umsg tmp;

	while(1) {
        result = msgrcv(id_comm, (void *)&mcom, (size_t)AT_CLI_FIFO_SIZE, (1 >> 1), 0);
        if(result == -1) continue;

        switch(mcom.tid){

            case PACKET_RESPONSE_HANDPIECE:
                memcpy((void*)&tmp, (void*)&mcom, sizeof(struct umsg));
                write(uart_fd, tmp.msg, tmp.cnt);
                break;

            case PACKET_RESPONSE_ERR_CODE:
                memcpy((void*)&tmp, (void*)&mcom, sizeof(struct umsg));
                write(uart_fd, tmp.msg, tmp.cnt);
                break;

            case PACKET_RESPONSE_SHOTCOUNT:
                memcpy((void*)&tmp, (void*)&mcom, sizeof(struct umsg));
                write(uart_fd, tmp.msg, tmp.cnt);
                break;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            case PACKET_COMMAND_QDELAY:
                memcpy((void*)&tmp, (void*)&mcom, sizeof(struct umsg));
                write(uart_fd, tmp.msg, tmp.cnt);
                break;

            case PACKET_RESPONSE_QDELAY:
                if((mcom.flag > 0) && (count < 3)){
                    write(uart_fd, tmp.msg, tmp.cnt);
                    printf( "PACKET_RESPONSE_QDELAY error! retry(%d)\n", count++);
                }
                else {
                    count = 0;
                    if(mcom.flag > 0) printf( "PACKET_RESPONSE_QDELAY error!\n");
                    else              printf( "PACKET_RESPONSE_QDELAY OK\n");
                }
                break;

            case PACKET_COMMAND_V1064S:
                memcpy((void*)&tmp, (void*)&mcom, sizeof(struct umsg));
                write(uart_fd, tmp.msg, tmp.cnt);
                break;

            case PACKET_RESPONSE_V1064S:
                if((mcom.flag > 0) && (count < 3)){
                    write(uart_fd, tmp.msg, tmp.cnt);
                    printf( "PACKET_RESPONSE_V1064S error! retry(%d)\n", count++);
                }
                else {
                    count = 0;
                    if(mcom.flag > 0) printf( "PACKET_RESPONSE_V1064S error!\n");
                    else              printf( "PACKET_RESPONSE_V1064S OK\n");
                }
                break;

            case PACKET_COMMAND_V1064D:
                memcpy((void*)&tmp, (void*)&mcom, sizeof(struct umsg));
                write(uart_fd, tmp.msg, tmp.cnt);
                break;

            case PACKET_RESPONSE_V1064D:
                if((mcom.flag > 0) && (count < 3)){
                    write(uart_fd, tmp.msg, tmp.cnt);
                    printf( "PACKET_RESPONSE_V1064D error! retry(%d)\n", count++);
                }
                else {
                    count = 0;
                    if(mcom.flag > 0) printf( "PACKET_RESPONSE_V1064D error!\n");
                    else              printf( "PACKET_RESPONSE_V1064D OK\n");
                }
                break;

            case PACKET_COMMAND_V1064M:
                memcpy((void*)&tmp, (void*)&mcom, sizeof(struct umsg));
                write(uart_fd, tmp.msg, tmp.cnt);
                break;

            case PACKET_RESPONSE_V1064M:
                if((mcom.flag > 0) && (count < 3)){
                    write(uart_fd, tmp.msg, tmp.cnt);
                    printf( "PACKET_RESPONSE_V1064M error! retry(%d)\n", count++);
                }
                else {
                    count = 0;
                    if(mcom.flag > 0) printf( "PACKET_RESPONSE_V1064M error!\n");
                    else              printf( "PACKET_RESPONSE_V1064M OK\n");
                }
                break;

            case PACKET_COMMAND_V532:
                memcpy((void*)&tmp, (void*)&mcom, sizeof(struct umsg));
                write(uart_fd, tmp.msg, tmp.cnt);
                break;

            case PACKET_RESPONSE_V532:
                if((mcom.flag > 0) && (count < 3)){
                    write(uart_fd, tmp.msg, tmp.cnt);
                    printf( "PACKET_RESPONSE_V532 error! retry(%d)\n", count++);
                }
                else {
                    count = 0;
                    if(mcom.flag > 0) printf( "PACKET_RESPONSE_V532 error!\n");
                    else              printf( "PACKET_RESPONSE_V532 OK\n");
                }
                break;

            case PACKET_COMMAND_VQUASI:
                memcpy((void*)&tmp, (void*)&mcom, sizeof(struct umsg));
                write(uart_fd, tmp.msg, tmp.cnt);
                break;

            case PACKET_RESPONSE_VQUASI:
                if((mcom.flag > 0) && (count < 3)){
                    write(uart_fd, tmp.msg, tmp.cnt);
                    printf( "PACKET_RESPONSE_VQUASI error! retry(%d)\n", count++);
                }
                else {
                    count = 0;
                    if(mcom.flag > 0) printf( "PACKET_RESPONSE_VQUASI error!\n");
                    else              printf( "PACKET_RESPONSE_VQUASI OK\n");
                }
                break;

            case PACKET_COMMAND_READY:
                memcpy((void*)&tmp, (void*)&mcom, sizeof(struct umsg));
                write(uart_fd, tmp.msg, tmp.cnt);
                break;

            case PACKET_RESPONSE_READY:
                if((mcom.flag > 0) && (count < 3)){
                    write(uart_fd, tmp.msg, tmp.cnt);
                    printf( "PACKET_COMMAND_READY error! retry(%d)\n", count++);
                }
                else {
                    count = 0;
                    if(mcom.flag > 0){
                        printf( "PACKET_COMMAND_READY error!\n");
                    }
                    else{
                        printf( "PACKET_COMMAND_READY OK\n");
                    }
                }
                break;

            case PACKET_COMMAND_AIMING:
                memcpy((void*)&tmp, (void*)&mcom, sizeof(struct umsg));
                write(uart_fd, tmp.msg, tmp.cnt);
                break;

            case PACKET_RESPONSE_AIMING:
                if((mcom.flag > 0) && (count < 3)){
                    write(uart_fd, tmp.msg, tmp.cnt);
                    printf( "PACKET_COMMAND_AIMMING error! retry(%d)\n", count++);
                }
                else {
                    count = 0;
                    if(mcom.flag > 0){
                        printf( "PACKET_COMMAND_AIMMING error!\n");
                    }
                    else{
                        printf( "PACKET_COMMAND_AIMMING OK\n");
                    }
                }
                break;

            case PACKET_COMMAND_VOLUME:
                memcpy((void*)&tmp, (void*)&mcom, sizeof(struct umsg));
                write(uart_fd, tmp.msg, tmp.cnt);
                break;

            case PACKET_RESPONSE_VOLUME:
                if((mcom.flag > 0) && (count < 3)){
                    write(uart_fd, tmp.msg, tmp.cnt);
                    printf( "PACKET_COMMAND_VOLUME error! retry(%d)\n", count++);
                }
                else {
                    count = 0;
                    if(mcom.flag > 0){
                        printf( "PACKET_COMMAND_VOLUME error!\n");
                    }
                    else{
                        printf( "PACKET_COMMAND_VOLUME OK\n");
                    }
                }
                break;

            case PACKET_COMMAND_STANDBY:
                memcpy((void*)&tmp, (void*)&mcom, sizeof(struct umsg));
                write(uart_fd, tmp.msg, tmp.cnt);
                break;

            case PACKET_RESPONSE_STANDBY:
                if((mcom.flag > 0) && (count < 3)){
                    write(uart_fd, tmp.msg, tmp.cnt);
                    printf( "PACKET_COMMAND_STANDBY error! retry(%d)\n", count++);
                }
                else {
                    count = 0;
                    if(mcom.flag > 0){
                        printf( "PACKET_COMMAND_STANDBY error!\n");
                    }
                    else{
                        printf( "PACKET_COMMAND_STANDBY OK\n");
                    }
                }
                break;

            case PACKET_COMMAND_SHOTCOUNT_ZERO:
                memcpy((void*)&tmp, (void*)&mcom, sizeof(struct umsg));
                write(uart_fd, tmp.msg, tmp.cnt);
                break;

            case PACKET_RESPONSE_SHOTCOUNT_ZERO:
                if((mcom.flag > 0) && (count < 3)){
                    write(uart_fd, tmp.msg, tmp.cnt);
                    printf( "PACKET_RESPONSE_SHOTCOUNT_ZERO error! retry(%d)\n", count++);
                }
                else {
                    count = 0;
                    if(mcom.flag > 0){
                        printf( "PACKET_RESPONSE_SHOTCOUNT_ZERO error!\n");
                    }
                    else{
                        printf( "PACKET_RESPONSE_SHOTCOUNT_ZERO OK\n");
                    }
                }
                break;
        }
        
	} // while
}



void init_uart(void)
{
    int res;

    uart_fd = open_uart(COMM_UART, 115200, 0, 1);
	res = sem_init(&sem_comm, 0, 0);
	if(res != 0)  printf("sem_comm error!\n");

    // 메시지큐 생성
    id_comm = msgget((key_t)8888, IPC_CREAT|0666);

    if (id_comm == -1)
    {
        printf("msgget error : id_comm\n");
    }
    memset(mcom.msg, 0, AT_CLI_FIFO_SIZE);


    pthread_create(&_uart_rx_thread, NULL, rx_thread, NULL);
    pthread_create(&_uart_tx_thread, NULL, tx_thread, NULL);
}


void close_uart(void)
{
    ringbuf_free(&rb);
    sem_destroy(&sem_comm);
    msgctl( id_comm, IPC_RMID, 0);
//    pthread_join(_uart_tx_thread, (void**)NULL);	// 해당 스레드가 종료되길 기다린다.
//    pthread_join(_uart_rx_thread, (void**)NULL);	// 해당 스레드가 종료되길 기다린다.
}
