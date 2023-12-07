#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include <stdbool.h>    // bool, true, false가 정의된 헤더 파일
#include "fb2bmp.h"
#include "cli.h"

#include "../lvgl/lvgl.h"
#include "lv_ui/ui.h"
#include "lv_drivers/display/sunxifb.h"


typedef struct __attribute__((__packed__)) {
    unsigned char bfType[2]; /* BM 표시 : "BM" (2 글자) 문자 */
    unsigned int bfSize; /* 파일의 크기 : 4 바이트 정수  */
    unsigned short bfReserved1; /* 추후의 확장을 위해서 필드(reserved) */
    unsigned short bfReserved2; /* 추후의 확장을 위해서 필드(reserved) */
    unsigned int bfOffBits; /* 실제 이미지까지의 오프셋(offset) : 바이트 */
} BITMAPFILEHEADER;

typedef struct {
    unsigned int biSize; /* 현 구조체의 크기 : 4바이트 */
    unsigned int biWidth; /* 이미지의 폭(픽셀단위) : 4바이트 */
    unsigned int biHeight; /* 이미지의 높이(픽셀단위) : 4바이트 */
    unsigned short biPlanes; /* 비트 플레인 수 (항상1) : 2바이트 */
    unsigned short biBitCount; /* 픽셀 당 비트 수 : 2바이트 */
    unsigned int biCompression; /* 압축 유형 : 4바이트 */
    unsigned int SizeImage; /* 이미지의 크기(압축 전의 바이트 단위) : 4바이트 */
    unsigned int biXPelsPerMeter; /* 가로 해상도  : 4바이트 */
    unsigned int biYPelsPerMeter; /* 세로 해상도 : 4바이트 */
    unsigned int biClrUsed;/* 실제 사용되는 색상수 : 4바이트 */
    unsigned int biClrImportant;/*중요한 색상 인덱스(0 인 경우 전체): 4바이트 */
} BITMAPINFOHEADER;

typedef struct {
    unsigned char rgbBlue;
    unsigned char rgbGreen;
    unsigned char rgbRed;
    unsigned char rgbReserved;
} RGBQUAD;

#define MEMZERO(x) memset(&(x), 0, sizeof(x)) /* 변수 초기화를 위한 매크로 */
#define NUMCOLOR 4

/////////////////////////////////////////////////////////////////////////////////////
/* Video4Linux에서 사용할 영상 저장을 위한 버퍼 */
struct buffer {
    void * start;
    size_t length;
};

static struct buffer *buff = NULL;

static void initRead(unsigned int buffer_size) 
{
    /* 메모리를 할당한다 */
    buff = (struct buffer*)calloc(1, sizeof(*buff));
    if(!buff) {
        perror("Out of memory");
        exit(EXIT_FAILURE);
    }

    /* 버퍼를 초기화 한다. */
    buff[0].length = buffer_size;
    buff[0].start = malloc(buffer_size);
    if(!buff[0].start) {
        perror("Out of memory");
        exit(EXIT_FAILURE);
    }
} 


static void change(uint32_t* Image)    // 상하 반전 함수
{
    uint32_t *tmp = malloc(614400 * 4);
	unsigned int i, j, ch;
	for (i = 0; i < 600 ; i++)
		for (j = 0; j < 1024; j++)
			tmp[i * 1024 + j] = Image[(600 - i - 1) * 1024 + j];

	for (i = 0; i < 614400 ; i++)
    {
        Image[i] = tmp[i];
	}
    free(tmp);
}

void _fb2bmp(char *argv[]) 
{
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;
    /* Refresh screen to screenshot callback */
    lv_disp_t * disp = lv_disp_get_default();
    FILE *infile, *outfile;       

    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";

    snprintf(buf, sizeof(buf), "%s",  argv[1]);

    system("cat /dev/fb0 > tmp.raw");

    infile = fopen("tmp.raw", "rb");
    if(infile) {
        printf("fb0 tmp.raw reading\n");
        initRead(614400 * 4);
        fread(buff->start, buff->length,1,infile);
        change(buff->start);
        system("rm -f *.raw");
    }
    else{
        printf("fb0 tmp.raw reading error\n");
        return;
    }

    MEMZERO(bmpFileHeader);
    bmpFileHeader.bfType[0] = 'B';
    bmpFileHeader.bfType[1] = 'M';
    bmpFileHeader.bfSize=sizeof(BITMAPFILEHEADER)+ sizeof(BITMAPINFOHEADER);
    bmpFileHeader.bfSize += disp->driver->hor_res*disp->driver->ver_res*NUMCOLOR;
    bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); // 54

    MEMZERO(bmpInfoHeader);
    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER); //40;
    bmpInfoHeader.biWidth = disp->driver->hor_res;
    bmpInfoHeader.biHeight = disp->driver->ver_res;
    bmpInfoHeader.biPlanes = 1;
    bmpInfoHeader.biBitCount = NUMCOLOR*8;
    bmpInfoHeader.SizeImage =  disp->driver->hor_res * disp->driver->ver_res*bmpInfoHeader.biBitCount/8;
    bmpInfoHeader.biXPelsPerMeter = 0x0;
    bmpInfoHeader.biYPelsPerMeter = 0x0;

    outfile = fopen(buf, "w");
    if(outfile) {
        /* BMP 파일(BITMAPFILEHEADER) 정보 저장 */ 
        fwrite((void*)&bmpFileHeader, sizeof(bmpFileHeader), 1, outfile);

        /* BMP 이미지(BITMAPINFOHEADER) 정보 저장 */ 
        fwrite((void*)&bmpInfoHeader, sizeof(bmpInfoHeader), 1, outfile);

        printf("bitmap header written\n");

        size_t res = fwrite((uint8_t *)buff->start, 1, buff->length, outfile);

        printf("bitmap data flushed to %s\n", buf);

        fclose(outfile);

    } else {
        printf("%s cannot be opened\n", buf);
    }

    free(buff[0].start);
    free(buff);

}
