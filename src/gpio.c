#include "cli.h"
#include "gpio.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <time.h>
#include <sys/time.h>
#include <linux/rtc.h>
#include <sys/mman.h>


// 포트의 데이타 구조
typedef struct
{
    // Configure Register
    uint32_t cfg[4];
    // Data Register
    uint32_t dat;
    // Multi_Driving Register
    uint32_t drv[4];
    // Pull Register
    uint32_t pull[3];
} port_t;

#define GPIO_OUTPUT	1
#define GPIO_INPUT	0

#define GPIO_MAX_PORT_COUNT	7
// 기본주소
#define BASE_ADDR 0x02000000
// 메핑할 메모리 크기
#define MEM_AREA_SIZE (GPIO_MAX_PORT_COUNT * sizeof(port_t))

// port_t 배열포인터
static port_t *ports = 0;

#define BYTEWRITE          1			/* Byte write command */
#define PAGEWRITE          2			/* Page write command */
#define RNDREAD            3			/* Random read command */
#define SEQREAD            4			/* Sequential read command */

#define I2C_RETRIES			0x0701
#define I2C_TIMEOUT			0x0702
#define I2C_RDWR			0x0707 

#define RTC_ADDR 			0x68

#define RTC_CLK				0x00		/* length 08-3F 56x8 */
#define RTC_RAM				0x08		/* length 08-3F 56x8 */

#define I2C_DEV 			"/dev/i2c-1"
#define RTC_DEV 			"/dev/rtc0"


struct i2c_rdwr_ioctl_data gmsg_rdwr;
struct i2c_msg             gi2cmsg;




// ADDRESS    BIT7  BIT6  BIT5  BIT4  BIT3  BIT2  BIT1  BIT0     FUNCTION    RANGE
// 00h        CH    10 Seconds        Seconds                    Seconds     00–59
// 01h        0     10 Minutes        Minutes                    Minutes     00–59
// 02h        0     12  10 Hour 10Hour Hours                     Hours       1-12
//                  24  PM/AM                                                +AM/PM 00-23
// 03h        0     0     0     0     0     DAY                  Day         01–07
// 04h        0     0     10 Date     Date                       Date        01–31
// 05h        0     0     0     10Month Month                    Month       01–12
// 06h        10 Year                 Year                       Year        00–99
// 07h        OUT   0     0     SQWE  0     0     RS1   RS0      Control     —
// 08h–3Fh RAM                                                   56 x 8      00h–FFh

#define RAM_COUNT				0x08		/* length 08-3F 56x8 */
#define RAM_MODE				0x14		/* length 08-3F 56x8 */

typedef struct _RamData
{
    uint32_t	total;			// 0 addr 8 9 10 11
    uint32_t	power;			// 4 addr 12 13 14 15
    uint32_t	lamp;			// 8 addr 16 17 18 19
	uint8_t		temp;			// 12 addr 20
	uint8_t		mode;			// 13 addr 21
    uint8_t     step[25];       // 14 addr 22 23 24 25 26, 27 28 29 30 31, 32 33 34 35 36, 37 38 39 40 41, 42 43 44 45 46
	uint8_t		nc[15];			// 39 addr 47 48 49 50 51, 52 53 54 55 56, 57 58 59 60 61, 62
	uint8_t		hp_mode;		// 54 addr 62
	uint8_t		check;			// 55 addr 63
} RamData;

typedef union _RAM1307
{
    RamData  data;
    uint8_t buff[56];	/* length 08-3F 56x8 */
} pRAM1307;

static pDS1307 rtc1307;
static pRAM1307 ram1307;



void ds1307_write(void)
{
	pRAM1307 tmp;
	int fd1307;

    if ((fd1307 = open (I2C_DEV, O_RDWR) ) < 0)
    {
        printf("can't open ds1307 i2c device\n");
		return;
    }

	// total, power, lamp counts 12byte write
	tmp.data.total = product.total;
	tmp.data.power = product.power;
	tmp.data.lamp  = product.lamp;

	tmp.data.temp = config.temp;
	tmp.data.mode = config.mode;
	tmp.data.hp_mode = config.hp_mode;
    
   	printf("write total [%d]\n ", product.total);	
   	printf("write power [%d]\n ", product.power);	
   	printf("write lamp  [%d]\n ", product.lamp);	

    for (int i = 0 ; i < 25 ; i++) tmp.data.step[i] = config.step[i];

    i2cTransfer(fd1307, PAGEWRITE, RAM_COUNT, &tmp.buff[0], 55);
//    for (int i = 0 ; i < 56 ; i++)
//    	printf("write data[%d] : 0x%02X\n ", i, tmp.buff[i]);	

    // 방금 write 한 data를 read
    i2cTransfer(fd1307, SEQREAD, RAM_COUNT, &tmp.buff[0], 55);

//    for (int i = 0 ; i < 25 ; i++)
//    	printf("ttt data : %02X %02X \n ", tmp.data.step[i], config.step[i]);	


    printf("read total : %d\n ", tmp.data.total);	
    printf("read power : %d\n ", tmp.data.power);	
    printf("read lamp : %d\n ", tmp.data.lamp);	

	close(fd1307);
}


void ds1307_read(void)
{
	pRAM1307 tmp;
	int fd1307;

    if ((fd1307 = open (I2C_DEV, O_RDWR) ) < 0)
    {
        printf("can't open ds1307 i2c device\n");
		return;
    }

    // data를 read
    i2cTransfer(fd1307, SEQREAD, RAM_COUNT, &tmp.buff[0], 55);

    printf("read total : %d\n ", tmp.data.total);	
    printf("read power : %d\n ", tmp.data.power);	
    printf("read lamp : %d\n ", tmp.data.lamp);	
	// total, power, lamp counts 12byte write
	product.total = tmp.data.total;
	product.power = tmp.data.power;
	product.lamp  = tmp.data.lamp;

	config.temp = tmp.data.temp;
	config.mode = tmp.data.mode;
	config.hp_mode = tmp.data.hp_mode;

    for (int i = 0 ; i < 25 ; i++)
		config.step[i] = tmp.data.step[i];

	close(fd1307);
}

void ds1307_clear(void)
{
	pRAM1307 tmp;
	int fd1307;

    if ((fd1307 = open (I2C_DEV, O_RDWR) ) < 0)
    {
        printf("can't open ds1307 i2c device\n");
		return;
    }

	// total, power, lamp counts 12byte write
	tmp.data.total = 0;
	tmp.data.power = 0;
	tmp.data.lamp  = 0;

    i2cTransfer(fd1307, PAGEWRITE, RAM_COUNT, &tmp.buff[0], 12);
//    i2cTransfer(fd1307, PAGEWRITE, RAM_COUNT+4, &tmp.buff[4], 4);
//    i2cTransfer(fd1307, PAGEWRITE, RAM_COUNT+8, &tmp.buff[8], 4);
    for (int i = 0 ; i < 12 ; i++)
    	printf("write data : 0x%02X\n ", tmp.buff[i]);	

    // 방금 write 한 data를 read
    i2cTransfer(fd1307, SEQREAD, RAM_COUNT, &tmp.buff[0], 12);
    printf("read total : %d\n ", tmp.data.total);	
    printf("read power : %d\n ", tmp.data.power);	
    printf("read lamp : %d\n ", tmp.data.lamp);	

	close(fd1307);
}

void ds1307_check(void)
{
	pRAM1307 tmp;
	int fd1307;

    if ((fd1307 = open (I2C_DEV, O_RDWR) ) < 0)
    {
        printf("can't open ds1307 i2c device\n");
		return;
    }

    // 방금 write 한 data를 read
    i2cTransfer(fd1307, SEQREAD, RAM_COUNT, &tmp.buff[0], 56);

	// total, power, lamp counts 12byte write
	if(tmp.data.check != 0xaa){
		printf("initial eeprom!\n");	
		memset(&tmp.buff[0], 0, 56);
		tmp.data.hp_mode = 1;
		tmp.buff[55] = 0xaa;
    	i2cTransfer(fd1307, PAGEWRITE, RAM_COUNT, &tmp.buff[0], 56);

		memset(&tmp.buff[0], 0xff, 56);
		// 방금 write 한 data를 read
		i2cTransfer(fd1307, SEQREAD, RAM_COUNT, &tmp.buff[0], 56);
		for (int i = 0 ; i < 56 ; i++)
			printf("read data : 0x%02X\n ", tmp.buff[i]);	
	}


	close(fd1307);
}



int gpio_init(void)
{
    // 반복적인 초기화 방지
    if (ports != 0) {
        printf("inited already\n");
        return 1;
    }
    // 오픈 /dev/mem
    int fd = open("/dev/mem", O_RDWR);
    if (fd == -1) {
        perror("open '/dev/mem' failed\n");
        return 1;
    }
    // 페이지 크기 가져오기
    uint32_t page_size = sysconf(_SC_PAGESIZE);
    // mmap결과 위의 오프셋인 페이지크기를 기준으로 기본주소의 나머지 분분을 계산한다
    uint32_t offset = BASE_ADDR % page_size;
    // 실제주소에 매핑됨
    void *map_addr = mmap(0, MEM_AREA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BASE_ADDR - offset);
    // 매핑실패
    if (map_addr == MAP_FAILED) {
        perror("mmap failed!\n");
        close(fd);
        return 1;
    }
    // /dev/mem은 매핑관계에 영향을주지않고 닫힐수 있다
    close(fd);
    // 배열의 첫번째 주소를 가리킨다，이시점에서는 첫번째 포트의 첫번째 레지스터를 가리킨다
    ports = (port_t *)(map_addr + offset);

    return 0;
}

// 포트를 찾는다
static port_t *check_and_get_gpio(char port, int pin)
{
    // 초기화가 되지않았다면
    if (ports == 0) {
        printf("not inited yet!\n");
        return 0;
    }
    // 포트번호 얻기다
    int port_index = port - 'A';
    // 포트번호 확인
    if (port_index < 0 || port_index >= GPIO_MAX_PORT_COUNT) {
        printf("no such port '%c'!\n", port);
        return 0;
    }
    // 핀번호 확인
    if (pin < 0 || pin >= 32) {
        printf("no such pin '%d'!\n", pin);
        return 0;
    }
    // 번호에 해당하는 포트를 반환한
    return ports + port_index;
}

void gpio_set_mode(char port, int pin, int mode)
{
    // 포트가져오기
    port_t *pport = check_and_get_gpio(port, pin);
    if (pport == 0) {
        return;
    }

    // 해당 cfg 레지스터를 가리킨다. 각 cfg 레지스터는 8핀을 제어할수있다 그래서 pin/8번째 cfg 레지스터
    uint32_t *cfg = &(pport->cfg[pin / 8]);
    // 마스크의 왼쪽 시프트 수
    int shift = (pin % 8) * 4;
    // cfg 레지스터에 해당하는 4비트 클리어
    *cfg &= ~(((uint32_t)0x0f) << shift);
    // cfg 레지스터에 해당하는 4비트 클리어하고 mode의 하위 4비트 설정
    *cfg |= ((uint32_t)(mode & 0x0f)) << shift;
}

void gpio_set_value(char port, int pin, int value)
{
    // 포트가져오기
    port_t *pport = check_and_get_gpio(port, pin);
    if (pport == 0) {
        return;
    }

    // 1이면 해당비트를 1로 설정하고 0이면 지운다
    if (value == 1) {
        pport->dat |= ((uint32_t)1) << pin;
    } else {
        pport->dat &= ~(((uint32_t)1) << pin);
    }
}

int gpio_get_value(char port, int pin)
{
    // 포트가져오기
    port_t *pport = check_and_get_gpio(port, pin);
    if (pport == 0) {
        return -1;
    }
    if ((pport->dat & (((uint32_t)1) << pin)) != 0)
        return 1 ;
    else
        return 0 ;
}

void gpio_power_on(void)
{
	gpio_set_value('G', 14, 1);	
}

void gpio_power_off(void)
{
	gpio_set_value('G', 14, 0);	
}

int read_power_switch(void)
{
	return gpio_get_value('B', 3);	
}

void init_gpio()
{
	gpio_init();
	// Power On FET OUTPUT
	gpio_set_mode('G', 14, GPIO_OUTPUT);
	// Audio ShotDown OUTPUT
	gpio_set_mode('B', 2, GPIO_OUTPUT);
	// Power Detect Input
	gpio_set_mode('E', 11, GPIO_INPUT);
	// Power Switch Input
	gpio_set_mode('B', 3, GPIO_INPUT);
	// Emergency SWITCH INPUT
	gpio_set_mode('E', 8, GPIO_INPUT);


    //// open i2c eeprom ///////////////////////////////////////////////////////   
	ds1307_check();
//    ds1307_read();
}

void close_gpio()
{
}












/////////////////////////////  static function /////////////////////////////////

int i2cTransfer(uint32_t uFd, uint8_t bCmd, uint8_t bAddr, uint8_t *pbData, uint32_t uLen)
{
	uint8_t bi;
	int i;
	uint32_t ui;
	uint8_t bRxbuff[256];
	uint8_t bTxbuff[9];
	  
	gmsg_rdwr.msgs = &gi2cmsg;
	gmsg_rdwr.nmsgs = 1;
	gi2cmsg.addr  = RTC_ADDR;
	  	
    if (bCmd == BYTEWRITE)
    {        	      
        bTxbuff[0] = bAddr; // set word address 
        bTxbuff[1] = *pbData; // write data  
        
        gi2cmsg.flags = 0;  // write flag
        gi2cmsg.len   = 2;
        gi2cmsg.buf   = bTxbuff;        

        if((i = ioctl(uFd, I2C_RDWR, &gmsg_rdwr)) < 0)
        {
			printf("can't write data \n");
			return -1;
	    }	              
	}
    else if (bCmd == PAGEWRITE)
    {
        if (uLen > 56)
		{   
			printf("invaild length\n"); 
			return -1;
		}
 	      
        bTxbuff[0] = bAddr; // set word address 
        for (bi = 0 ; bi < uLen ; bi++)
        {
            bTxbuff[bi + 1] = *(pbData + bi); 
        }    
        
        gi2cmsg.flags = 0;  // write flag
        gi2cmsg.len   = uLen + 1;
        gi2cmsg.buf   = bTxbuff;   
        
        if((i = ioctl(uFd, I2C_RDWR, &gmsg_rdwr)) < 0)
        {
			printf("can't write data \n");
			return -1;
		}	  	
    }   
    else if (bCmd == RNDREAD)
    {
		bTxbuff[0] = bAddr; // word address

		gi2cmsg.flags = 0;  // write flag
		gi2cmsg.len   = 1;
		gi2cmsg.buf   = bTxbuff;

        if((i = ioctl(uFd, I2C_RDWR, &gmsg_rdwr)) < 0)
        {
			printf("can't write word address \n");
			return -1;
		}	
        
        gi2cmsg.flags = I2C_M_RD;
	    gi2cmsg.len   = 1;
	    gi2cmsg.buf   = pbData;

        if((i = ioctl(uFd, I2C_RDWR, &gmsg_rdwr)) < 0)
        {
            printf("can't random read data \n");
	        return -1;
	    }    
    }
    else if (bCmd == SEQREAD)
    {
        bTxbuff[0] = bAddr; // word address
        
	    gi2cmsg.flags = 0;  // write flag
        gi2cmsg.len   = 1;
        gi2cmsg.buf   = bTxbuff;

        if((i = ioctl(uFd, I2C_RDWR, &gmsg_rdwr)) < 0)
        {
            printf("can't write word address \n");
	        return -1;
	    }	       
        
        gi2cmsg.flags = I2C_M_RD;
	    gi2cmsg.len   = uLen;
	    gi2cmsg.buf   = pbData;

        if((i = ioctl(uFd, I2C_RDWR, &gmsg_rdwr)) < 0)
        {
            printf("can't sequential read data \n");
	        return -1;
	    } 	      
    }	
    else
    {
        printf("invaild command\n"); 	
        return -1;
    }
    
    return 0;
}

void check_rtc_devicefile( void )
{
	if( access( RTC_DEV , F_OK ) == 0 ) return;

	mknod( RTC_DEV , S_IRWXU|S_IRWXG|S_IFCHR,(10<<8|(135)));
}

int set_systemtime( int year, int mon, int day, int hour, int min, int sec )
{
	int              rtc;
	struct rtc_time  rtc_time_data;
	struct tm        tm_src;
	struct timeval   tv = { 0, 0 };

	tm_src.tm_year = rtc_time_data.tm_year = year - 1900;
	tm_src.tm_mon  = rtc_time_data.tm_mon  = mon  - 1;
	tm_src.tm_mday = rtc_time_data.tm_mday = day;
	tm_src.tm_hour = rtc_time_data.tm_hour = hour;
	tm_src.tm_min  = rtc_time_data.tm_min  = min;
	tm_src.tm_sec  = rtc_time_data.tm_sec  = sec;

	tv.tv_sec = mktime( &tm_src );

	settimeofday ( &tv, NULL );
	
	check_rtc_devicefile();

	rtc = open ( RTC_DEV, O_WRONLY );
	if( rtc < 0 )
	{
		perror( "/dev/rtc0 open error" );
		return -1;
	}

	if ( ioctl ( rtc, RTC_SET_TIME, &rtc_time_data ) < 0 )
	{
		perror( "/dev/rtc0 rtc write time error" );
		close( rtc );
		return -1;
	}

	close( rtc );

	return 0;
}

void get_systemtime(char * str)
{
	int              rtc;
	struct rtc_time  time, tmp;
	struct tm        tm_src;
	struct timeval   tv = { 0, 0 };

	check_rtc_devicefile();

	rtc = open ( RTC_DEV, O_RDONLY );
	if( rtc < 0 )
	{
		perror( "/dev/rtc0 open error" );
		return;
	}

	if ( ioctl ( rtc, RTC_RD_TIME, &time ) < 0 )
	{
		perror( "/dev/rtc0 rtc read time error" );
		close( rtc );
		return;
	}

	sprintf(str, "%04d-%02d-%02d %02d:%02d",
		1900+time.tm_year, time.tm_mon+1, time.tm_mday,
		time.tm_hour, time.tm_min);

	close( rtc );
	return;
}


uint8_t dec2bcd(uint8_t num)
{
    uint8_t ones = 0;
    uint8_t tens = 0;
    uint8_t temp = 0;

    ones = num%10;
    temp = num/10;
    tens = temp<<4;
    return (tens + ones);
}

int test_ds13072(void)
{
	int uFd;
	uint8_t brndNum;
	uint8_t breadData;

    //// open i2c eeprom ///////////////////////////////////////////////////////   
    if ((uFd = open (I2C_DEV, O_RDWR) ) < 0)
    {
        printf("can't open i2c device\n");
        return -1 ;       
    }		

    printf("==== random write/read test start ====\n"); 
	rtc1307.data.sec   = 0x50;
	rtc1307.data.min   = 0x24;
	rtc1307.data.hour  = 0x15;
	rtc1307.data.day   = 0x07;
	rtc1307.data.date  = 0x04;
	rtc1307.data.month = 0x11;
	rtc1307.data.year  = 0x23;
	
    // 생성한 random number 1byte write	
    i2cTransfer(uFd, PAGEWRITE, 0x0, &rtc1307.buff[0], 7); 
    for (int i = 0 ; i < 7 ; i++)
    printf("write data : 0x%.2X\n ", rtc1307.buff[i]);	

    // 방금 write 한 data를 read
    i2cTransfer(uFd, SEQREAD, 0x0, &rtc1307.buff[0], 7);
    for (int i = 0 ; i < 7 ; i++)
    printf("read data : 0x%.2X\n ", rtc1307.buff[i]);	
	      
    // write 한 data와 read data를 비교한다.
    printf("==== random write/read test ok ====\n\n");

}
