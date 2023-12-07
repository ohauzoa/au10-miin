#ifndef _MIIN_PROJECT_GPIO_H
#define _MIIN_PROJECT_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>

#define GPIO_PATH "/sys/class/gpio/gpio%d/value"
#define GPIO_FUNCTION_PATH "/sys/class/gpio/gpio%d/direction"
#define PWN_PATH "/sys/class/pwm/pwmchip0/pwm7/enable"   //enable

static int GPIO_Point_IN1_fd;
static int GPIO_Point_IN2_fd;
static int GPIO_Point_IN3_fd;
static int GPIO_Point_IN4_fd;
static int PWM_GPIO_fd;
//1 wjdqkdgid
static unsigned char phasecw[4] = {0x08, 0x04, 0x02, 0x01};
// 1 相反转
// u8 phaseccw[4] = {0x01, 0x02, 0x04, 0x08};

// 2向 
// u8 phasecw[4] = {0x0c, 0x06, 0x03,0x09};   //正转 电机导通相序  DC-CB-BA-AD
// u8 phaseccw[4] = {0x03, 0x06, 0x0c, 0x09}; //反转 电机导通相序  AB-BC-CD-DA

// 1-2向
// u8 phasecw[8] = {0x08, 0x0c, 0x04, 0x06, 0x02, 0x03, 0x01, 0x09}; //正转 电机导通相序  D-DC-C-CB-B-BA-A-AD
// u8 phaseccw[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x89}; //反转 电机导通相序  A-AB-B-BC-C-CD-D-D

//PWM 출력 normal or inversed
typedef enum PWM_Polarity
{
    NORMAL = 1,
    INVERSED = 0
}PWM_Polarity;

typedef struct PWMBaseInitTypeDef{
    long period;                 // 주기/  ns
    long duty_cycle;             // 듀티 / ns 
    PWM_Polarity polarity;       // 출력  NORMAL/INVERSED
}PWMBaseInitTypeDef;



#define GPIO_Point_IN1 2021
#define GPIO_Point_IN2 2022
#define GPIO_Point_IN3 2023
#define GPIO_Point_IN4 2024

#define GPIO_WRITEPIN(Point,value) (write(Point,value,1))

#define delay_ms(x)  (usleep(x*1000))

#define GPIO_Point_IN1_Set write(GPIO_Point_IN1_fd, "1", 1)
#define GPIO_Point_IN2_Set write(GPIO_Point_IN2_fd, "1", 1)
#define GPIO_Point_IN3_Set write(GPIO_Point_IN3_fd, "1", 1)
#define GPIO_Point_IN4_Set write(GPIO_Point_IN4_fd, "1", 1)

#define GPIO_Point_IN1_ReSet write(GPIO_Point_IN1_fd, "0", 1)
#define GPIO_Point_IN2_ReSet write(GPIO_Point_IN2_fd, "0", 1)
#define GPIO_Point_IN3_ReSet write(GPIO_Point_IN3_fd, "0", 1)
#define GPIO_Point_IN4_ReSet write(GPIO_Point_IN4_fd, "0", 1)

#define PWM_OutPut_ENABLE  write(PWM_GPIO_fd, "1", 1)
#define PWM_OutPut_DISABLE write(PWM_GPIO_fd, "0", 1)

int Motor_Init();   //스테퍼 모터 및 IO 초기화
int Motor_PWM_Init(PWMBaseInitTypeDef PWM_BaseStructure);  //PWM 초기화 기능
int Motor_PWM_Set(int enable);    //PWM 상태 활성화 설정 기능

int  GPIO_WrietPin(int GPIO_Point,int GPIO_Value);   // IO 수준 쓰기 기능
void MotorCW( void );
void MotorStop( void );



typedef struct _RtcData
{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
	uint8_t control;
} pRtcData;

typedef union _DS1307
{
    pRtcData  data;
    uint8_t buff[8];	/* length 08-3F 56x8 */
} pDS1307;




void gpio_power_off(void);
int read_power_switch(void);


//void ds1307_rtc(void);
void ds1307_write(void);
void ds1307_read(void);
void ds1307_clear(void);

void init_gpio();
int i2cTransfer(uint32_t uFd, uint8_t bCmd, uint8_t bAddr, uint8_t *pbData, uint32_t uLen);
uint8_t dec2bcd(uint8_t num);

void check_rtc_devicefile( void );
int set_systemtime( int year, int mon, int day, int hour, int min, int sec );
void get_systemtime(char * str);





#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
