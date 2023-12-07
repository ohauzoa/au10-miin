#ifndef _MIIN_PROJECT_CONFIG_TASK_H
#define _MIIN_PROJECT_CONFIG_TASK_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>

#include "main.h"
#include "functions.h"

#define HP_AUTO                     0
#define HP_MANUAL                   1

#define FREQ_MIN                    0
#define FREQ_MAX                    9
#define TRANSITION_TIME             400

typedef enum logo_e
{
    LOGO_MiiN = 0,
    LOGO_BRIGHTONE,
    LOGO_PEAKLIGHT
} logo_e;

typedef enum mode_e
{
    MODE_MEMORY = 0,
    MODE_PRESET
} mode_e;

typedef enum wavelength_e
{
    WAVE_1064S = 0,
    WAVE_1064D,
    WAVE_MiiN,
    WAVE_532,
    WAVE_QUASI
} wavelength_e;

typedef enum Error_Code
{
    ERR_NONE = 0,
    ERR_INTERLOCK,
    ERR_SIMMER2,
    ERR_SIMMER1,
    ERR_OVP,
    ERR_OCP,
    ERR_FOOT_DET,
} ERR_CODE;

typedef enum HP_Type
{
    ZOOM_1064 = 0,
    ZOOM_532,
    MLA,
    DOE
} HP_TYPE;



typedef enum ds1307_e
{
    DS_RTC = 0,
    DS_COUNT,
    DS_SERIAL,
    DS_TEMP
} DS1307_e;

// eeprom save
typedef struct product_t
{
// Count NO.
    int total;
    int power;
    int lamp;

// Serial NO.
    int seriallaser;
    int serialpower;
    int serialproduct;
} product_t;

typedef struct config_t
{
    char    password[20];   /*!< A NULL terminated string */
//    char    calibrate[20];  /*!< A NULL terminated string */
    int     volume;         //
    int     standbytime;    //
    int     logo;           // 0:miin 1:brightone 2:peaklight
    int     aiming;         // 0~7

    char    SSID[20];
    char    PSK[20];


// 저장 안하는 변수
    int     standby_count;
    int     flag_shotcount_count;
    int     flag_shotcount;
    int     flag_rtc_write;
    int     flag_aim_write;
    int     flag_handpiece;

    unsigned char     temp;           // 0~23:easy save
    unsigned char     mode;           // easy step 0~15, memory (0~2:1064S) (3~5:1064D)(6~8:MiiN)(9~11:532)(12~14:Quasi)(15---->user) 
    unsigned char     step[25];       // 0~24
    unsigned char     hp_mode;

    int     ready;
    int     energy;
    int     voltage;
    int     wavelength;
    int     frequency;
    int     qdelay1;
    int     qdelay2;
    int     handpiece_rx;
    int     handpiece;
    int     spotsize;
    int     tmp_handpiece;
    int     tmp_spotsize;

    int     shotcount;

    int     err_code;
} config_t;


typedef struct compute_t
{
    int voltage[15];
    int energy[15];
} compute_t;

typedef struct calibrate_t
{
// Q delay 1064S Q1, 1064D Q1, 1064D Q2, MiiN High Q1, Q2, Miin Low Q1, Q2
    int     qdelay[15];
// Voltage
    int     energy[5][15];  // Voltage 계산시 여분필요함.
    int     voltage[5][15];
} calibrate_t;


typedef struct _preset_dat_t
{
    char mark[30];
    int wavelength;
    int handpiece;
    int spot;
    int fluence;
    int energy;
    int frequency;
} _preset_dat_t;

typedef struct preset_t
{
    char   name[30];
    _preset_dat_t dat[20];
    int    count;
} preset_t;



extern const int number_map[5];
extern const int energy_map[5][15];
extern calibrate_t calibrate;
extern product_t product;
extern config_t config;
//extern memory_t memory[5];
extern preset_t preset[30];

int make_calibrate_modify(void);
int make_calibrate_default(void);
int make_preset_modify(int number);

int init_configuration(void);

int modify_product(const char *iniFile, const char *section, const char *key, const char *value);
int modify_config(const char *iniFile, const char *section, const char *key, const char *value);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
