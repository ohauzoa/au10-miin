#ifndef _MIIN_PROJECT_UART_H
#define _MIIN_PROJECT_UART_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    MSTATE_IDLE = 0,
    MSTATE_SAVE_DATA,
    MSTATE_BLUE_CHECK,
    MSTATE_PACKET_CHECK
} CommState_e;


#define PACKET_ID_HOST  0x00
#define PACKET_ID_APP   0x01
#define PACKET_STX      0x02
#define PACKET_ETX      0x03
#define PACKET_ACK      0x06
#define PACKET_NAK      0x15
#define PACKET_RET      0x0D

#define BUF_SIZE 512


#define PACKET_COMMAND_WAVELENGTH       0x40
#define PACKET_COMMAND_READY            0x41
#define PACKET_COMMAND_STANDBY          0x42
#define PACKET_COMMAND_AIMING           0x43
#define PACKET_COMMAND_HANDPIECE        0x44        // TxD

#define PACKET_COMMAND_SHOTCOUNT        0x45        // TxD
#define PACKET_COMMAND_SHOTCOUNT_ZERO   0x46        // RxD

#define PACKET_COMMAND_ERR_CODE         0x4E        // TxD
#define PACKET_COMMAND_STATUS           0x4F


#define PACKET_COMMAND_QDELAY           0x50
#define PACKET_COMMAND_V1064S           0x51
#define PACKET_COMMAND_V1064D           0x52
#define PACKET_COMMAND_V1064M           0x53
#define PACKET_COMMAND_V532             0x54
#define PACKET_COMMAND_VQUASI           0x55

#define PACKET_COMMAND_COUNT            0x56
#define PACKET_COMMAND_SERIAL           0x57
#define PACKET_COMMAND_VOLUME           0x58

/////////////////////////////////////////////
#define PACKET_RESPONSE_WAVELENGTH      0x70
#define PACKET_RESPONSE_READY           0x71
#define PACKET_RESPONSE_STANDBY         0x72
#define PACKET_RESPONSE_AIMING          0x73
#define PACKET_RESPONSE_HANDPIECE       0x74

#define PACKET_RESPONSE_SHOTCOUNT       0x75
#define PACKET_RESPONSE_SHOTCOUNT_ZERO  0x76

#define PACKET_RESPONSE_ERR_CODE        0x7E
#define PACKET_RESPONSE_STATUS          0x7F

#define PACKET_RESPONSE_QDELAY          0x80
#define PACKET_RESPONSE_V1064S          0x81
#define PACKET_RESPONSE_V1064D          0x82
#define PACKET_RESPONSE_V1064M          0x83
#define PACKET_RESPONSE_V532            0x84
#define PACKET_RESPONSE_VQUASI          0x85

#define PACKET_RESPONSE_COUNT           0x86
#define PACKET_RESPONSE_SERIAL          0x87
#define PACKET_RESPONSE_VOLUME          0x88


#define AT_CLI_FIFO_SIZE            128

void init_uart(void);
void make_qdelay(void);
void send_txd(int cmd);
void close_uart(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
