
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#include "sound.h"
#include "ezini.h"
#include "config.h"

//#define CUSTOM
const int qdelay_default[7] = { 190, 140, 90, 100, 25 , 90 , 25 }; 

const int number_map[5] = { 14, 12, 13, 15, 11 }; 
const int energy_map[5][15] = {
                                { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 0} ,
                                { 300, 500, 700, 900, 1100, 1300, 1500, 1700, 1900, 2100, 2300, 2500, 0, 0, 0 },
                                { 500, 700, 900, 1100, 1300, 1500, 1700, 1900, 2100, 2300, 2500, 2700, 3000, 0, 0 },
                                { 20, 30, 40, 50, 60, 70, 80, 90, 100, 120, 150, 200, 300, 400, 500 },
                                { 500, 800, 1100, 1400, 1700, 2000, 2300, 2600, 2900, 3200, 3500, 0, 0, 0, 0 }
                               };

static const char * const label_calwave[5] = { "1064S", "1064D", "MiiN", "532", "Quasi" };

calibrate_t calibrate;
product_t product;
config_t config;
// memory 1064S[0~3], 1064D[4~7], MiiN[8~11], 532[12~15], Quasi[16~19]
//memory_t memory[5];
preset_t preset[30];

static int make_product_default(void);
static int make_config_default(void);
static int make_preset_default(int num);
static int populate_calibrate(calibrate_t *my_struct, const ini_entry_t *entry);
static int populate_product(product_t *my_struct, const ini_entry_t *entry);
static int populate_config(config_t *my_struct, const ini_entry_t *entry);
static int populate_preset(preset_t *my_struct, const ini_entry_t *entry);
static int populate_lesion(preset_t *my_struct, const ini_entry_t *entry);

static int init_calibrate(void);
static int init_product(void);
static int init_config(void);
static int init_preset(int num);

const char *path = "image";
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void ff(void)
{
    struct  dirent **namelist;
    int     count;
    int     idx;

    if((count = scandir(path, &namelist, NULL, alphasort)) == -1) {
        fprintf(stderr, "%s Directory Scan Error: %s\n", path, strerror(errno));
//        return 1;
    }

    for(idx = 0; idx < count; idx++) {
        printf("%s\n", namelist[idx]->d_name);
    }

    // 건별 데이터 메모리 해제
    for(idx = 0; idx < count; idx++) {
        free(namelist[idx]);
    }

    // namelist에 대한 메모리 해제
    free(namelist);

}


static int populate_preset(preset_t *my_struct, const ini_entry_t *entry)
{
    preset_t *ptr;
    int offset;

    /* use the XXX in struct XXX section as index into my_struct */
    if (strncmp("step ", entry->section, 5) == 0)
    {

        offset = atoi(entry->section + 5);

        if (offset < 1)
        {
            return -1;      /* invalid number in struct_XXX */
        }

        offset--;
        ptr = my_struct;
    }
    else
    {
        return -1;      /* unexpected section */
    }

    if (strcmp("indication", entry->key) == 0)
    {
        strncpy(ptr->dat[offset].mark, entry->value, 30);
    }
    else if (strcmp("wavelength", entry->key) == 0)
    {
        ptr->dat[offset].wavelength = atoi(entry->value);
    }
    else if (strcmp("handpiece", entry->key) == 0)
    {
        ptr->dat[offset].handpiece = atoi(entry->value);
    }
    else if (strcmp("spot", entry->key) == 0)
    {
        ptr->dat[offset].spot = atoi(entry->value);
    }
    else if (strcmp("energy", entry->key) == 0)
    {
        ptr->dat[offset].energy = atoi(entry->value);
    }
    else if (strcmp("frequency", entry->key) == 0)
    {
        ptr->dat[offset].frequency = atoi(entry->value);
    }
    else
    {
        return -1;      /* unexpected key */
    }

    return offset + 1; // count
}

static int populate_lesion(preset_t *my_struct, const ini_entry_t *entry)
{
    preset_t *ptr;
    int offset;

    /* use the XXX in struct XXX section as index into my_struct */
    if (strncmp("lesion", entry->section, 6) == 0)
    {
        ptr = my_struct;
    }
    else
    {
        return -1;      /* unexpected section */
    }

    if (strcmp("name", entry->key) == 0)
    {
        strncpy(ptr->name, entry->value, 30);
    }
    else
    {
        return -1;      /* unexpected key */
    }

    return 0; // count
}

static int init_preset(int num)
{
    int i, cnt;
    int result;
    FILE *fp;

    ini_entry_t entry;
    ini_entry_list_t list;
    char buf[30];
    sprintf(buf, "preset%d.ini", num);

//    printf("\nReading %s\n", buf);
//    printf("=======================\n");

    fp = fopen(buf, "r");
    if(fp == NULL)
    {
        printf("파일이 없습니다. 새로운 파일을 생성합니다.");
        make_preset_default(num);
        printf("\nReading %s\n", buf);
        fp = fopen(buf, "r");
    }

    /* initialize entry structure before reading first entry */
    entry.section = NULL;
    entry.key = NULL;
    entry.value = NULL;


    /* read ini file back into a structure */
    while ((result = GetEntryFromFile(fp, &entry)) > 0)
    {
        populate_lesion(&preset[num], &entry);
        preset[num].count = populate_preset(&preset[num], &entry);
    }

    fclose(fp);

    printf("lesion : %s, step : %d\n", preset[num].name, preset[num].count);

    if (result < 0)
    {
        printf("Error getting entry from preset.ini\n");
    }

#if 0
    for (i = 0; i < preset[num].count; i++)
    {
        if(preset[num].dat[i].mark == NULL) break;
        printf("step %d\n", (i + 1));
        printf("\tindication %s\n", preset[num].dat[i].mark);
        printf("\twavelength %d\n", preset[num].dat[i].wavelength);
        printf("\thandpiece %d\n", preset[num].dat[i].handpiece);
        printf("\tspot %d\n", preset[num].dat[i].spot);
        printf("\tenergy %d\n", preset[num].dat[i].energy);
        printf("\tfrequency %d\n", preset[num].dat[i].frequency);
    }
#endif
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////

static int init_calibrate(void)
{
    int i, j;
    int result;
    FILE *fp;

    ini_entry_t entry;
    ini_entry_list_t list;

    printf("\nReading calibrate.ini\n");
    printf("=======================\n");
    fp = fopen("calibrate.ini", "r");
    if(fp == NULL)
    {
        printf("파일이 없습니다. 새로운 파일을 생성합니다.");
        make_calibrate_default();
        printf("\nReading calibrate.ini\n");
        fp = fopen("calibrate.ini", "r");
    }

    /* initialize entry structure before reading first entry */
    entry.section = NULL;
    entry.key = NULL;
    entry.value = NULL;

    /* read ini file back into a structure */
    i = 0;
    while ((result = GetEntryFromFile(fp, &entry)) > 0)
    {
        populate_calibrate(&calibrate, &entry);
    }

    fclose(fp);

    if (result < 0)
    {
        printf("Error getting entry from calibrate.ini\n");
    }

//    printf("calibrate\n");
    for (i = 0; i < 7; i++)
    {
        printf("qdelay [%d] %d\n", i, calibrate.qdelay[i]);
    }

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < number_map[i]; j++)
        {
            calibrate.energy[i][j] = energy_map[i][j];
//            printf("energy->%d, voltage->%d\n", calibrate.energy[i][j], calibrate.voltage[i][j]);
        }    
    }    

    return 0;
}


int make_calibrate_default(void)
{
    char section[10];
    char key[10];
    char value[10];
    int i, j;

    ini_entry_t entry;
    ini_entry_list_t list;

    /* build list of entries for MakeINI */
    list = NULL;

    for (i = 0; i < 7; i++)
    {
        snprintf(key, sizeof(key), "Qdelay v%d", i);
        snprintf(value, sizeof(value), "%d", qdelay_default[i]);
        AddEntryToList(&list, "calibrate", key, value);
    }

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < number_map[i]; j++)
        {
            snprintf(key, sizeof(key), "%s v%d", label_calwave[i], j);
            snprintf(value, sizeof(value), "%d", calibrate.voltage[i][j]);
            AddEntryToList(&list, "calibrate", key, value);
        }    
    }

    /* now create the ini */
    if (0 != MakeINIFile("calibrate.ini", list))
    {
        printf("Error making calibrate.ini file\n");
    }

    FreeList(list);

    return 0;
}

int make_calibrate_modify(void)
{
    char section[10];
    char key[10];
    char value[10];
    int i, j;

    ini_entry_t entry;
    ini_entry_list_t list;

    /* build list of entries for MakeINI */
    list = NULL;

    for (i = 0; i < 7; i++)
    {
        snprintf(key, sizeof(key), "Qdelay v%d", i);
        snprintf(value, sizeof(value), "%d", calibrate.qdelay[i]);
        AddEntryToList(&list, "calibrate", key, value);
    }

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < number_map[i]; j++)
        {
            snprintf(key, sizeof(key), "%s v%d", label_calwave[i], j);
            snprintf(value, sizeof(value), "%d", calibrate.voltage[i][j]);
            AddEntryToList(&list, "calibrate", key, value);
        }    
    }

    /* now create the ini */
    if (0 != MakeINIFile("calibrate.ini", list))
    {
        printf("Error making calibrate.ini file\n");
    }

    FreeList(list);

    return 0;
}

static int populate_calibrate(calibrate_t *cfg, const ini_entry_t *entry)
{
    calibrate_t *ptr;

    /* use the XXX in struct XXX section as index into my_struct */
    if (strncmp("calibrate", entry->section, 9) == 0)
    {
        ptr = cfg;
    }
    else
    {
        return -1;      /* unexpected section */
    }

    if (strcmp("Qdelay v0", entry->key) == 0)
    {
        ptr->qdelay[0] = atoi(entry->value);
    }
    else if (strcmp("Qdelay v1", entry->key) == 0)
    {
        ptr->qdelay[1] = atoi(entry->value);
    }
    else if (strcmp("Qdelay v2", entry->key) == 0)
    {
        ptr->qdelay[2] = atoi(entry->value);
    }
    else if (strcmp("Qdelay v3", entry->key) == 0)
    {
        ptr->qdelay[3] = atoi(entry->value);
    }
    else if (strcmp("Qdelay v4", entry->key) == 0)
    {
        ptr->qdelay[4] = atoi(entry->value);
    }
    else if (strcmp("Qdelay v5", entry->key) == 0)
    {
        ptr->qdelay[5] = atoi(entry->value);
    }
    else if (strcmp("Qdelay v6", entry->key) == 0)
    {
        ptr->qdelay[6] = atoi(entry->value);
    }
///////////////////////////////////////////////////////////
    else if (strcmp("1064S v0", entry->key) == 0)
    {
        ptr->voltage[0][0] = atoi(entry->value);
    }
    else if (strcmp("1064S v1", entry->key) == 0)
    {
        ptr->voltage[0][1] = atoi(entry->value);
    }
    else if (strcmp("1064S v2", entry->key) == 0)
    {
        ptr->voltage[0][2] = atoi(entry->value);
    }
    else if (strcmp("1064S v3", entry->key) == 0)
    {
        ptr->voltage[0][3] = atoi(entry->value);
    }
    else if (strcmp("1064S v4", entry->key) == 0)
    {
        ptr->voltage[0][4] = atoi(entry->value);
    }
    else if (strcmp("1064S v5", entry->key) == 0)
    {
        ptr->voltage[0][5] = atoi(entry->value);
    }
    else if (strcmp("1064S v6", entry->key) == 0)
    {
        ptr->voltage[0][6] = atoi(entry->value);
    }
    else if (strcmp("1064S v7", entry->key) == 0)
    {
        ptr->voltage[0][7] = atoi(entry->value);
    }
    else if (strcmp("1064S v8", entry->key) == 0)
    {
        ptr->voltage[0][8] = atoi(entry->value);
    }
    else if (strcmp("1064S v9", entry->key) == 0)
    {
        ptr->voltage[0][9] = atoi(entry->value);
    }
    else if (strcmp("1064S v10", entry->key) == 0)
    {
        ptr->voltage[0][10] = atoi(entry->value);
    }
    else if (strcmp("1064S v11", entry->key) == 0)
    {
        ptr->voltage[0][11] = atoi(entry->value);
    }
    else if (strcmp("1064S v12", entry->key) == 0)
    {
        ptr->voltage[0][12] = atoi(entry->value);
    }
    else if (strcmp("1064S v13", entry->key) == 0)
    {
        ptr->voltage[0][13] = atoi(entry->value);
    }
    else if (strcmp("1064S v14", entry->key) == 0)
    {
        ptr->voltage[0][14] = atoi(entry->value);
    }
///////////////////////////////////////////////////////////////
    else if (strcmp("1064D v0", entry->key) == 0)
    {
        ptr->voltage[1][0] = atoi(entry->value);
    }
    else if (strcmp("1064D v1", entry->key) == 0)
    {
        ptr->voltage[1][1] = atoi(entry->value);
    }
    else if (strcmp("1064D v2", entry->key) == 0)
    {
        ptr->voltage[1][2] = atoi(entry->value);
    }
    else if (strcmp("1064D v3", entry->key) == 0)
    {
        ptr->voltage[1][3] = atoi(entry->value);
    }
    else if (strcmp("1064D v4", entry->key) == 0)
    {
        ptr->voltage[1][4] = atoi(entry->value);
    }
    else if (strcmp("1064D v5", entry->key) == 0)
    {
        ptr->voltage[1][5] = atoi(entry->value);
    }
    else if (strcmp("1064D v6", entry->key) == 0)
    {
        ptr->voltage[1][6] = atoi(entry->value);
    }
    else if (strcmp("1064D v7", entry->key) == 0)
    {
        ptr->voltage[1][7] = atoi(entry->value);
    }
    else if (strcmp("1064D v8", entry->key) == 0)
    {
        ptr->voltage[1][8] = atoi(entry->value);
    }
    else if (strcmp("1064D v9", entry->key) == 0)
    {
        ptr->voltage[1][9] = atoi(entry->value);
    }
    else if (strcmp("1064D v10", entry->key) == 0)
    {
        ptr->voltage[1][10] = atoi(entry->value);
    }
    else if (strcmp("1064D v11", entry->key) == 0)
    {
        ptr->voltage[1][11] = atoi(entry->value);
    }
    else if (strcmp("1064D v12", entry->key) == 0)
    {
        ptr->voltage[1][12] = atoi(entry->value);
    }
    else if (strcmp("1064D v13", entry->key) == 0)
    {
        ptr->voltage[1][13] = atoi(entry->value);
    }
    else if (strcmp("1064D v14", entry->key) == 0)
    {
        ptr->voltage[1][14] = atoi(entry->value);
    }
////////////////////////////////////////////////////////
    else if (strcmp("MiiN v0", entry->key) == 0)
    {
        ptr->voltage[2][0] = atoi(entry->value);
    }
    else if (strcmp("MiiN v1", entry->key) == 0)
    {
        ptr->voltage[2][1] = atoi(entry->value);
    }
    else if (strcmp("MiiN v2", entry->key) == 0)
    {
        ptr->voltage[2][2] = atoi(entry->value);
    }
    else if (strcmp("MiiN v3", entry->key) == 0)
    {
        ptr->voltage[2][3] = atoi(entry->value);
    }
    else if (strcmp("MiiN v4", entry->key) == 0)
    {
        ptr->voltage[2][4] = atoi(entry->value);
    }
    else if (strcmp("MiiN v5", entry->key) == 0)
    {
        ptr->voltage[2][5] = atoi(entry->value);
    }
    else if (strcmp("MiiN v6", entry->key) == 0)
    {
        ptr->voltage[2][6] = atoi(entry->value);
    }
    else if (strcmp("MiiN v7", entry->key) == 0)
    {
        ptr->voltage[2][7] = atoi(entry->value);
    }
    else if (strcmp("MiiN v8", entry->key) == 0)
    {
        ptr->voltage[2][8] = atoi(entry->value);
    }
    else if (strcmp("MiiN v9", entry->key) == 0)
    {
        ptr->voltage[2][9] = atoi(entry->value);
    }
    else if (strcmp("MiiN v10", entry->key) == 0)
    {
        ptr->voltage[2][10] = atoi(entry->value);
    }
    else if (strcmp("MiiN v11", entry->key) == 0)
    {
        ptr->voltage[2][11] = atoi(entry->value);
    }
    else if (strcmp("MiiN v12", entry->key) == 0)
    {
        ptr->voltage[2][12] = atoi(entry->value);
    }
    else if (strcmp("MiiN v13", entry->key) == 0)
    {
        ptr->voltage[2][13] = atoi(entry->value);
    }
    else if (strcmp("MiiN v14", entry->key) == 0)
    {
        ptr->voltage[2][14] = atoi(entry->value);
    }
////////////////////////////////////////////////////////
    else if (strcmp("532 v0", entry->key) == 0)
    {
        ptr->voltage[3][0] = atoi(entry->value);
    }
    else if (strcmp("532 v1", entry->key) == 0)
    {
        ptr->voltage[3][1] = atoi(entry->value);
    }
    else if (strcmp("532 v2", entry->key) == 0)
    {
        ptr->voltage[3][2] = atoi(entry->value);
    }
    else if (strcmp("532 v3", entry->key) == 0)
    {
        ptr->voltage[3][3] = atoi(entry->value);
    }
    else if (strcmp("532 v4", entry->key) == 0)
    {
        ptr->voltage[3][4] = atoi(entry->value);
    }
    else if (strcmp("532 v5", entry->key) == 0)
    {
        ptr->voltage[3][5] = atoi(entry->value);
    }
    else if (strcmp("532 v6", entry->key) == 0)
    {
        ptr->voltage[3][6] = atoi(entry->value);
    }
    else if (strcmp("532 v7", entry->key) == 0)
    {
        ptr->voltage[3][7] = atoi(entry->value);
    }
    else if (strcmp("532 v8", entry->key) == 0)
    {
        ptr->voltage[3][8] = atoi(entry->value);
    }
    else if (strcmp("532 v9", entry->key) == 0)
    {
        ptr->voltage[3][9] = atoi(entry->value);
    }
    else if (strcmp("532 v10", entry->key) == 0)
    {
        ptr->voltage[3][10] = atoi(entry->value);
    }
    else if (strcmp("532 v11", entry->key) == 0)
    {
        ptr->voltage[3][11] = atoi(entry->value);
    }
    else if (strcmp("532 v12", entry->key) == 0)
    {
        ptr->voltage[3][12] = atoi(entry->value);
    }
    else if (strcmp("532 v13", entry->key) == 0)
    {
        ptr->voltage[3][13] = atoi(entry->value);
    }
    else if (strcmp("532 v14", entry->key) == 0)
    {
        ptr->voltage[3][14] = atoi(entry->value);
    }
////////////////////////////////////////////////////////
    else if (strcmp("Quasi v0", entry->key) == 0)
    {
        ptr->voltage[4][0] = atoi(entry->value);
    }
    else if (strcmp("Quasi v1", entry->key) == 0)
    {
        ptr->voltage[4][1] = atoi(entry->value);
    }
    else if (strcmp("Quasi v2", entry->key) == 0)
    {
        ptr->voltage[4][2] = atoi(entry->value);
    }
    else if (strcmp("Quasi v3", entry->key) == 0)
    {
        ptr->voltage[4][3] = atoi(entry->value);
    }
    else if (strcmp("Quasi v4", entry->key) == 0)
    {
        ptr->voltage[4][4] = atoi(entry->value);
    }
    else if (strcmp("Quasi v5", entry->key) == 0)
    {
        ptr->voltage[4][5] = atoi(entry->value);
    }
    else if (strcmp("Quasi v6", entry->key) == 0)
    {
        ptr->voltage[4][6] = atoi(entry->value);
    }
    else if (strcmp("Quasi v7", entry->key) == 0)
    {
        ptr->voltage[4][7] = atoi(entry->value);
    }
    else if (strcmp("Quasi v8", entry->key) == 0)
    {
        ptr->voltage[4][8] = atoi(entry->value);
    }
    else if (strcmp("Quasi v9", entry->key) == 0)
    {
        ptr->voltage[4][9] = atoi(entry->value);
    }
    else if (strcmp("Quasi v10", entry->key) == 0)
    {
        ptr->voltage[4][10] = atoi(entry->value);
    }
    else if (strcmp("Quasi v11", entry->key) == 0)
    {
        ptr->voltage[4][11] = atoi(entry->value);
    }
    else if (strcmp("Quasi v12", entry->key) == 0)
    {
        ptr->voltage[4][12] = atoi(entry->value);
    }
    else if (strcmp("Quasi v13", entry->key) == 0)
    {
        ptr->voltage[4][13] = atoi(entry->value);
    }
    else if (strcmp("Quasi v14", entry->key) == 0)
    {
        ptr->voltage[4][14] = atoi(entry->value);
    }

    else
    {
        return -1;      /* unexpected key */
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////

static int init_config(void)
{
//    config_t cfg;
    int i;
    int result;
    FILE *fp;

    ini_entry_t entry;
    ini_entry_list_t list;

    printf("\nReading config.ini\n");
    printf("=======================\n");
    fp = fopen("config.ini", "r");
    if(fp == NULL)
    {
        printf("파일이 없습니다. 새로운 파일을 생성합니다.");
        make_config_default();
        printf("\nReading config.ini\n");
        fp = fopen("config.ini", "r");
    }

    /* initialize entry structure before reading first entry */
    entry.section = NULL;
    entry.key = NULL;
    entry.value = NULL;

    /* read ini file back into a structure */
    while ((result = GetEntryFromFile(fp, &entry)) > 0)
    {
        populate_config(&config, &entry);
    }

    fclose(fp);

    if (result < 0)
    {
        printf("Error getting entry from config.ini\n");
    }

    printf("config\n");
    printf("passwd %s\n", config.password);
    printf("volume %d\n", config.volume);
    printf("standbytime %d\n", config.standbytime);
    printf("logo %d\n", config.logo);
    printf("aimingbeam %d\n", config.aiming);
    printf("temp %d\n", config.temp);
    printf("mode %d\n", config.mode);

    printf("SSID %s\n", config.SSID);
    printf("PSK %s\n", config.PSK);

    return 0;
}


static int make_config_default(void)
{
    char section[10];
    char key[10];
    char value[10];
    int i, j;

    ini_entry_t entry;
    ini_entry_list_t list;

    /* build list of entries for MakeINI */
    list = NULL;
    AddEntryToList(&list, "config", "password", "1111");
    AddEntryToList(&list, "config", "calibrate", "0316205811");
    AddEntryToList(&list, "config", "volume", "3");
    AddEntryToList(&list, "config", "standbytime", "3");
    AddEntryToList(&list, "config", "logo", "0");
    AddEntryToList(&list, "config", "aiming", "1000");

    AddEntryToList(&list, "config", "SSID", "LTRA");
    AddEntryToList(&list, "config", "PSK", "1234");

//    AddEntryToList(&list, "config", "temp", "0");
//    AddEntryToList(&list, "config", "mode", "24");

    for (i = 0; i < 25; i++)
    {
//        snprintf(key, sizeof(key), "step%d", i);
//        snprintf(value, sizeof(value), "%d", config.step[i]);
//        AddEntryToList(&list, "config", key, value);
    }

    printf("\nWriting config.ini\n");
    printf("=======================\n");

    /* now create the ini */
    if (0 != MakeINIFile("config.ini", list))
    {
        printf("Error making config.ini file\n");
    }

    FreeList(list);

    return 0;
}

int modify_config(const char *iniFile, const char *section, const char *key, const char *value)
{
    int i;
    int result;
    FILE *fp;

    ini_entry_t entry;
    ini_entry_list_t list;
 

    fp = fopen(iniFile, "r");
    if(fp == NULL)
    {
        printf("file not found!");
        return -1;
    }

    /* initialize entry structure before reading first entry */
    entry.section = NULL;
    entry.key = NULL;
    entry.value = NULL;

    /* read ini file back into a structure */
    while ((result = GetEntryFromFile(fp, &entry)) > 0)
    {
        populate_config(&config, &entry);
    }

    fclose(fp);

    if (0 != DeleteEntryFromFile(iniFile, section, key))
    {
        printf("Error deleting entry from config.ini file\n");
        return -1;
    }

    list = NULL;
    AddEntryToList(&list, section, key, value);

    if (0 != AddEntryToFile(iniFile, list))
    {
        printf("Error deleting entry from config.ini file\n");
        return -1;
    }

    fp = fopen(iniFile, "r");

    /* read ini file back into a structure */
    while ((result = GetEntryFromFile(fp, &entry)) > 0)
    {
        populate_config(&config, &entry);
    }

    fclose(fp);

    if (result < 0)
    {
        printf("Error getting entry from config.ini\n");
        return -1;
    }

    return 0;
}


static int populate_config(config_t *cfg, const ini_entry_t *entry)
{
    config_t *ptr;

    /* use the XXX in struct XXX section as index into my_struct */
    if (strncmp("config", entry->section, 6) == 0)
    {
        ptr = cfg;
    }
    else
    {
        return -1;      /* unexpected section */
    }

    if (strcmp("password", entry->key) == 0)
    {
        strncpy(ptr->password, entry->value, 20);
    }
    else if (strcmp("volume", entry->key) == 0)
    {
        ptr->volume = atoi(entry->value);
    }
    else if (strcmp("standbytime", entry->key) == 0)
    {
        ptr->standbytime = atoi(entry->value);
    }
    else if (strcmp("logo", entry->key) == 0)
    {
        ptr->logo = atoi(entry->value);
    }
    else if (strcmp("aiming", entry->key) == 0)
    {
        ptr->aiming = atoi(entry->value);
    }
    else if (strcmp("SSID", entry->key) == 0)
    {
        strncpy(ptr->SSID, entry->value, 20);
    }
    else if (strcmp("PSK", entry->key) == 0)
    {
        strncpy(ptr->PSK, entry->value, 20);
    }

////////////////////////////////////////////////////////////
/*
    else if (strcmp("temp", entry->key) == 0)
    {
        ptr->temp = atoi(entry->value);
    }
    else if (strcmp("mode", entry->key) == 0)
    {
        ptr->mode = atoi(entry->value);
    }
    else if (strcmp("step0", entry->key) == 0)
    {
        ptr->step[0] = atoi(entry->value);
    }
    else if (strcmp("step1", entry->key) == 0)
    {
        ptr->step[1] = atoi(entry->value);
    }
    else if (strcmp("step2", entry->key) == 0)
    {
        ptr->step[2] = atoi(entry->value);
    }
    else if (strcmp("step3", entry->key) == 0)
    {
        ptr->step[3] = atoi(entry->value);
    }
    else if (strcmp("step4", entry->key) == 0)
    {
        ptr->step[4] = atoi(entry->value);
    }
    else if (strcmp("step5", entry->key) == 0)
    {
        ptr->step[5] = atoi(entry->value);
    }
    else if (strcmp("step6", entry->key) == 0)
    {
        ptr->step[6] = atoi(entry->value);
    }
    else if (strcmp("step7", entry->key) == 0)
    {
        ptr->step[7] = atoi(entry->value);
    }
    else if (strcmp("step8", entry->key) == 0)
    {
        ptr->step[8] = atoi(entry->value);
    }
    else if (strcmp("step9", entry->key) == 0)
    {
        ptr->step[9] = atoi(entry->value);
    }
    else if (strcmp("step10", entry->key) == 0)
    {
        ptr->step[10] = atoi(entry->value);
    }
    else if (strcmp("step11", entry->key) == 0)
    {
        ptr->step[11] = atoi(entry->value);
    }
    else if (strcmp("step12", entry->key) == 0)
    {
        ptr->step[12] = atoi(entry->value);
    }
    else if (strcmp("step13", entry->key) == 0)
    {
        ptr->step[13] = atoi(entry->value);
    }
    else if (strcmp("step14", entry->key) == 0)
    {
        ptr->step[14] = atoi(entry->value);
    }
    else if (strcmp("step15", entry->key) == 0)
    {
        ptr->step[15] = atoi(entry->value);
    }
    else if (strcmp("step16", entry->key) == 0)
    {
        ptr->step[16] = atoi(entry->value);
    }
    else if (strcmp("step17", entry->key) == 0)
    {
        ptr->step[17] = atoi(entry->value);
    }
    else if (strcmp("step18", entry->key) == 0)
    {
        ptr->step[18] = atoi(entry->value);
    }
    else if (strcmp("step19", entry->key) == 0)
    {
        ptr->step[19] = atoi(entry->value);
    }
    else if (strcmp("step20", entry->key) == 0)
    {
        ptr->step[20] = atoi(entry->value);
    }
    else if (strcmp("step21", entry->key) == 0)
    {
        ptr->step[21] = atoi(entry->value);
    }
    else if (strcmp("step22", entry->key) == 0)
    {
        ptr->step[22] = atoi(entry->value);
    }
    else if (strcmp("step23", entry->key) == 0)
    {
        ptr->step[23] = atoi(entry->value);
    }
    else if (strcmp("step24", entry->key) == 0)
    {
        ptr->step[24] = atoi(entry->value);
    }
*/
//////////////////////////////////////////////////////////////
    else
    {
        return -1;      /* unexpected key */
    }

    return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////


static int init_product(void)
{
    int i;
    int result;
    FILE *fp;

    ini_entry_t entry;
    ini_entry_list_t list;

    printf("\nReading product.ini\n");
    printf("=======================\n");
    fp = fopen("product.ini", "r");
    if(fp == NULL)
    {
        printf("파일이 없습니다. 새로운 파일을 생성합니다.");
        make_product_default();
        printf("\nReading product.ini\n");
        fp = fopen("product.ini", "r");
    }

    /* initialize entry structure before reading first entry */
    entry.section = NULL;
    entry.key = NULL;
    entry.value = NULL;

    /* read ini file back into a structure */
    while ((result = GetEntryFromFile(fp, &entry)) > 0)
    {
        populate_product(&product, &entry);
    }

    fclose(fp);

    if (result < 0)
    {
        printf("Error getting entry from product.ini\n");
    }

    printf("product\n");

    printf("count total %d\n", product.total);
    printf("count power %d\n", product.power);
    printf("count lamp %d\n",  product.lamp);

    printf("serial laser %d\n", product.seriallaser);
    printf("serial power %d\n", product.serialpower);
    printf("serial product %d\n", product.serialproduct);

    return 0;
}


static int make_product_default(void)
{
    FILE *fp;

    ini_entry_t entry;
    ini_entry_list_t list;

    /* build list of entries for MakeINI */
    list = NULL;


    AddEntryToList(&list, "product", "count total", "0");
    AddEntryToList(&list, "product", "count power", "0");
    AddEntryToList(&list, "product", "count lamp", "0");

    AddEntryToList(&list, "product", "serial laser", "0");
    AddEntryToList(&list, "product", "serial power", "0");
    AddEntryToList(&list, "product", "serial product", "0");

    printf("\nWriting product.ini\n");
    printf("=======================\n");

    /* now create the ini */
    if (0 != MakeINIFile("product.ini", list))
    {
        printf("Error making product.ini file\n");
    }

    FreeList(list);

    return 0;
}


int modify_product(const char *iniFile, const char *section, const char *key, const char *value)
{
    int i;
    int result;
    FILE *fp;

    ini_entry_t entry;
    ini_entry_list_t list;
 

    fp = fopen(iniFile, "r");
    if(fp == NULL)
    {
        printf("file not found!");
        return -1;
    }

    /* initialize entry structure before reading first entry */
    entry.section = NULL;
    entry.key = NULL;
    entry.value = NULL;

    /* read ini file back into a structure */
    while ((result = GetEntryFromFile(fp, &entry)) > 0)
    {
        populate_product(&product, &entry);
    }

    fclose(fp);

    if (0 != DeleteEntryFromFile(iniFile, section, key))
    {
        printf("Error deleting entry from product.ini file\n");
        return -1;
    }

    list = NULL;
    AddEntryToList(&list, section, key, value);

    if (0 != AddEntryToFile(iniFile, list))
    {
        printf("Error deleting entry from product.ini file\n");
        return -1;
    }

    fp = fopen(iniFile, "r");

    /* read ini file back into a structure */
    while ((result = GetEntryFromFile(fp, &entry)) > 0)
    {
        populate_product(&product, &entry);
    }

    fclose(fp);

    if (result < 0)
    {
        printf("Error getting entry from product.ini\n");
        return -1;
    }

    return 0;
}


static int populate_product(product_t *cfg, const ini_entry_t *entry)
{
    product_t *ptr;

    /* use the XXX in struct XXX section as index into my_struct */
    if (strncmp("product", entry->section, 7) == 0)
    {
        ptr = cfg;
    }
    else
    {
        return -1;      /* unexpected section */
    }

    if (strcmp("count total", entry->key) == 0)
    {
        ptr->total = atoi(entry->value);
    }
    else if (strcmp("count power", entry->key) == 0)
    {
        ptr->power = atoi(entry->value);
    }
    else if (strcmp("count lamp", entry->key) == 0)
    {
        ptr->lamp = atoi(entry->value);
    }
    else if (strcmp("serial laser", entry->key) == 0)
    {
        ptr->seriallaser = atoi(entry->value);
    }
    else if (strcmp("serial power", entry->key) == 0)
    {
        ptr->serialpower = atoi(entry->value);
    }
    else if (strcmp("serial product", entry->key) == 0)
    {
        ptr->serialproduct = atoi(entry->value);
    }
    else
    {
        return -1;      /* unexpected key */
    }

    return 0;
}

int make_preset_modify(int number)
{
    char section[32];
    char key[32];
    char value[32];
    int i, j;

    ini_entry_t entry;
    ini_entry_list_t list;

    char name[30];
    sprintf(name, "preset%d.ini", number);

    /* build list of entries for MakeINI */
    list = NULL;

    AddEntryToList(&list, "lesion", "name", preset[number].name);
    AddEntryToList(&list, "lesion", "pos", "0");
    printf("make_preset_modify %s\n", name, preset[number].count);

    for (i = 0; i < preset[number].count; i++)
    {
        snprintf(section, sizeof(section), "step %d", i+1);
//        snprintf(key, sizeof(key), "step %d", i);
        snprintf(value, sizeof(value), "%s", preset[number].dat[i].mark);
        AddEntryToList(&list, section, "indication", value);
        snprintf(value, sizeof(value), "%d", preset[number].dat[i].wavelength);
        AddEntryToList(&list, section, "wavelength", value);
        snprintf(value, sizeof(value), "%d", preset[number].dat[i].handpiece);
        AddEntryToList(&list, section, "handpiece", value);
        snprintf(value, sizeof(value), "%d", preset[number].dat[i].spot);
        AddEntryToList(&list, section, "spot", value);
        snprintf(value, sizeof(value), "%d", preset[number].dat[i].energy);
        AddEntryToList(&list, section, "energy", value);
        snprintf(value, sizeof(value), "%d", preset[number].dat[i].frequency);
        AddEntryToList(&list, section, "frequency", value);
    }

    printf("\nWriting %s\n", name);
    printf("=======================\n");

    /* now create the ini */
    if (0 != MakeINIFile(name, list))
    {
        printf("Error making %s file\n", name);
    }

    FreeList(list);

    return 0;
}


int make_preset_default(int num)
{
    FILE *fp;

    ini_entry_t entry;
    ini_entry_list_t list;

    char name[30];
    sprintf(name, "preset%d.ini", num);

    /* build list of entries for MakeINI */
    list = NULL;

    switch(num)
    {
        case 0: // ABNON
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "ABNON");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "ABNOM 1");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "0");
            AddEntryToList(&list, "step 1", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 1", "energy", "650");
            AddEntryToList(&list, "step 1", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 2", "indication", "ABNOM 2");
            AddEntryToList(&list, "step 2", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 2", "handpiece", "0");  // 
            AddEntryToList(&list, "step 2", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 2", "energy", "700");
            AddEntryToList(&list, "step 2", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 3", "indication", "ABNOM 3");
            AddEntryToList(&list, "step 3", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 3", "handpiece", "0");
            AddEntryToList(&list, "step 3", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 3", "energy", "750");
            AddEntryToList(&list, "step 3", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 4", "indication", "ABNOM 4");
            AddEntryToList(&list, "step 4", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 4", "handpiece", "0");
            AddEntryToList(&list, "step 4", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 4", "energy", "800");
            AddEntryToList(&list, "step 4", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 5", "indication", "ABNOM 5");
            AddEntryToList(&list, "step 5", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 5", "handpiece", "0");
            AddEntryToList(&list, "step 5", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 5", "energy", "850");
            AddEntryToList(&list, "step 5", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 6", "indication", "ABNOM 6");
            AddEntryToList(&list, "step 6", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 6", "handpiece", "0");
            AddEntryToList(&list, "step 6", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 6", "energy", "900");
            AddEntryToList(&list, "step 6", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 7", "indication", "ABNOM 7");
            AddEntryToList(&list, "step 7", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 7", "handpiece", "0");
            AddEntryToList(&list, "step 7", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 7", "energy", "950");
            AddEntryToList(&list, "step 7", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 8", "indication", "ABNOM 8");
            AddEntryToList(&list, "step 8", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 8", "handpiece", "0");
            AddEntryToList(&list, "step 8", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 8", "energy", "1000");
            AddEntryToList(&list, "step 8", "frequency", "0");  //  "0" -> 1Hz
            break;

        case 1: // TATTO(Black)
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "TATTOO(Black)");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "TATTOO(Black) 1");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "0");
            AddEntryToList(&list, "step 1", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 1", "energy", "300");
            AddEntryToList(&list, "step 1", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 2", "indication", "TATTOO(Black) 2");
            AddEntryToList(&list, "step 2", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 2", "handpiece", "0");
            AddEntryToList(&list, "step 2", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 2", "energy", "350");
            AddEntryToList(&list, "step 2", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 3", "indication", "TATTOO(Black) 3");
            AddEntryToList(&list, "step 3", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 3", "handpiece", "0");
            AddEntryToList(&list, "step 3", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 3", "energy", "400");
            AddEntryToList(&list, "step 3", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 4", "indication", "TATTOO(Black) 4");
            AddEntryToList(&list, "step 4", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 4", "handpiece", "0");
            AddEntryToList(&list, "step 4", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 4", "energy", "450");
            AddEntryToList(&list, "step 4", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 5", "indication", "TATTOO(Black) 5");
            AddEntryToList(&list, "step 5", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 5", "handpiece", "0");
            AddEntryToList(&list, "step 5", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 5", "energy", "500");
            AddEntryToList(&list, "step 5", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 6", "indication", "TATTOO(Black) 6");
            AddEntryToList(&list, "step 6", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 6", "handpiece", "0");
            AddEntryToList(&list, "step 6", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 6", "energy", "550");
            AddEntryToList(&list, "step 6", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 7", "indication", "TATTOO(Black) 7");
            AddEntryToList(&list, "step 7", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 7", "handpiece", "0");
            AddEntryToList(&list, "step 7", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 7", "energy", "600");
            AddEntryToList(&list, "step 7", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 8", "indication", "TATTOO(Black) 8");
            AddEntryToList(&list, "step 8", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 8", "handpiece", "0");
            AddEntryToList(&list, "step 8", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 8", "energy", "650");
            AddEntryToList(&list, "step 8", "frequency", "0");  //  "0" -> 1Hz
            break;

        case 2: // TATTO(Red) 
                // ZOOM 532 { "1 mm", "2 mm", "3 mm", "3.5 mm", "4 mm", "5 mm", "6 mm", "6.5 mm", "7 mm"  },
            AddEntryToList(&list, "lesion", "name", "TATTO(Red)");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "TATTOO(Red) 1");
            AddEntryToList(&list, "step 1", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 1", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 1", "spot", "4");       // ZOOM 532 4(4 mm)
            AddEntryToList(&list, "step 1", "energy", "120");
            AddEntryToList(&list, "step 1", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 2", "indication", "TATTOO(Red) 2");
            AddEntryToList(&list, "step 2", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 2", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 2", "spot", "4");       // ZOOM 532 4(4 mm)
            AddEntryToList(&list, "step 2", "energy", "130");
            AddEntryToList(&list, "step 2", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 3", "indication", "TATTOO(Red) 3");
            AddEntryToList(&list, "step 3", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 3", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 3", "spot", "4");       // ZOOM 532 4(4 mm)
            AddEntryToList(&list, "step 3", "energy", "140");
            AddEntryToList(&list, "step 3", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 4", "indication", "TATTOO(Red) 4");
            AddEntryToList(&list, "step 4", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 4", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 4", "spot", "4");       // ZOOM 532 4(4 mm)
            AddEntryToList(&list, "step 4", "energy", "150");
            AddEntryToList(&list, "step 4", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 5", "indication", "TATTOO(Red) 5");
            AddEntryToList(&list, "step 5", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 5", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 5", "spot", "4");
            AddEntryToList(&list, "step 5", "energy", "160");
            AddEntryToList(&list, "step 5", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 6", "indication", "TATTOO(Red) 6");
            AddEntryToList(&list, "step 6", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 6", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 6", "spot", "4");       // ZOOM 532 4(4 mm)
            AddEntryToList(&list, "step 6", "energy", "170");
            AddEntryToList(&list, "step 6", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 7", "indication", "TATTOO(Red) 7");
            AddEntryToList(&list, "step 7", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 7", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 7", "spot", "4");       // ZOOM 532 4(4 mm)
            AddEntryToList(&list, "step 7", "energy", "180");
            AddEntryToList(&list, "step 7", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 8", "indication", "TATTOO(Red) 8");
            AddEntryToList(&list, "step 8", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 8", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 8", "spot", "4");       // ZOOM 532 4(4 mm)
            AddEntryToList(&list, "step 8", "energy", "190");
            AddEntryToList(&list, "step 8", "frequency", "0");  //  "0" -> 1Hz
            break;

        case 3: // Melasma
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "Melasma");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Melasma 1");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "0");
            AddEntryToList(&list, "step 1", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 1", "energy", "500");
            AddEntryToList(&list, "step 1", "frequency", "7");  //  "7" -> 8Hz

            AddEntryToList(&list, "step 2", "indication", "Melasma 2");
            AddEntryToList(&list, "step 2", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 2", "handpiece", "0");
            AddEntryToList(&list, "step 2", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 2", "energy", "550");
            AddEntryToList(&list, "step 2", "frequency", "7");  //  "7" -> 8Hz

            AddEntryToList(&list, "step 3", "indication", "Melasma 3");
            AddEntryToList(&list, "step 3", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 3", "handpiece", "0");
            AddEntryToList(&list, "step 3", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 3", "energy", "600");
            AddEntryToList(&list, "step 3", "frequency", "7");  //  "7" -> 8Hz

            AddEntryToList(&list, "step 4", "indication", "Melasma 4");
            AddEntryToList(&list, "step 4", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 4", "handpiece", "0");
            AddEntryToList(&list, "step 4", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 4", "energy", "650");
            AddEntryToList(&list, "step 4", "frequency", "7");  //  "7" -> 8Hz

            AddEntryToList(&list, "step 5", "indication", "Melasma 5");
            AddEntryToList(&list, "step 5", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 5", "handpiece", "0");
            AddEntryToList(&list, "step 5", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 5", "energy", "700");
            AddEntryToList(&list, "step 5", "frequency", "7");  //  "7" -> 8Hz

            AddEntryToList(&list, "step 6", "indication", "Melasma 6");
            AddEntryToList(&list, "step 6", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 6", "handpiece", "0");
            AddEntryToList(&list, "step 6", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 6", "energy", "750");
            AddEntryToList(&list, "step 6", "frequency", "7");  //  "7" -> 8Hz

            AddEntryToList(&list, "step 7", "indication", "Melasma 7");
            AddEntryToList(&list, "step 7", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 7", "handpiece", "0");
            AddEntryToList(&list, "step 7", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 7", "energy", "800");
            AddEntryToList(&list, "step 7", "frequency", "7");  //  "7" -> 8Hz

            AddEntryToList(&list, "step 8", "indication", "Melasma 8");
            AddEntryToList(&list, "step 8", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 8", "handpiece", "0");
            AddEntryToList(&list, "step 8", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 8", "energy", "850");
            AddEntryToList(&list, "step 8", "frequency", "7");  //  "7" -> 8Hz
            break;

        case 4: // Nevus of Ota
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "Nevus of Ota");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Nevus of Ota 1");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "0");
            AddEntryToList(&list, "step 1", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 1", "energy", "450");
            AddEntryToList(&list, "step 1", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 2", "indication", "Nevus of Ota 2");
            AddEntryToList(&list, "step 2", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 2", "handpiece", "0");
            AddEntryToList(&list, "step 2", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 2", "energy", "500");
            AddEntryToList(&list, "step 2", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 3", "indication", "Nevus of Ota 3");
            AddEntryToList(&list, "step 3", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 3", "handpiece", "0");
            AddEntryToList(&list, "step 3", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 3", "energy", "550");
            AddEntryToList(&list, "step 3", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 4", "indication", "Nevus of Ota 4");
            AddEntryToList(&list, "step 4", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 4", "handpiece", "0");
            AddEntryToList(&list, "step 4", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 4", "energy", "600");
            AddEntryToList(&list, "step 4", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 5", "indication", "Nevus of Ota 5");
            AddEntryToList(&list, "step 5", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 5", "handpiece", "0");
            AddEntryToList(&list, "step 5", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 5", "energy", "650");
            AddEntryToList(&list, "step 5", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 6", "indication", "Nevus of Ota 6");
            AddEntryToList(&list, "step 6", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 6", "handpiece", "0");
            AddEntryToList(&list, "step 6", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 6", "energy", "700");
            AddEntryToList(&list, "step 6", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 7", "indication", "Nevus of Ota 7");
            AddEntryToList(&list, "step 7", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 7", "handpiece", "0");
            AddEntryToList(&list, "step 7", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 7", "energy", "750");
            AddEntryToList(&list, "step 7", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 8", "indication", "Nevus of Ota 8");
            AddEntryToList(&list, "step 8", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 8", "handpiece", "0");
            AddEntryToList(&list, "step 8", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 8", "energy", "800");
            AddEntryToList(&list, "step 8", "frequency", "0");  //  "0" -> 1Hz
            break;

        case 5: // Café au lait
                // ZOOM 532 { "1 mm", "2 mm", "3 mm", "3.5 mm", "4 mm", "5 mm", "6 mm", "6.5 mm", "7 mm"  },
            AddEntryToList(&list, "lesion", "name", "Café au lait");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Café au lait 1");
            AddEntryToList(&list, "step 1", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 1", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 1", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 1", "energy", "30");
            AddEntryToList(&list, "step 1", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 2", "indication", "Café au lait 2");
            AddEntryToList(&list, "step 2", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 2", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 2", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 2", "energy", "40");
            AddEntryToList(&list, "step 2", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 3", "indication", "Café au lait 3");
            AddEntryToList(&list, "step 3", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 3", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 3", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 3", "energy", "50");
            AddEntryToList(&list, "step 3", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 4", "indication", "Café au lait 4");
            AddEntryToList(&list, "step 4", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 4", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 4", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 4", "energy", "60");
            AddEntryToList(&list, "step 4", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 5", "indication", "Café au lait 5");
            AddEntryToList(&list, "step 5", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 5", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 5", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 5", "energy", "70");
            AddEntryToList(&list, "step 5", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 6", "indication", "Café au lait 6");
            AddEntryToList(&list, "step 6", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 6", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 6", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 6", "energy", "80");
            AddEntryToList(&list, "step 6", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 7", "indication", "Café au lait 7");
            AddEntryToList(&list, "step 7", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 7", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 7", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 7", "energy", "90");
            AddEntryToList(&list, "step 7", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 8", "indication", "Café au lait 8");
            AddEntryToList(&list, "step 8", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 8", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 8", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 8", "energy", "100");
            AddEntryToList(&list, "step 8", "frequency", "0");  //  "0" -> 1Hz
            break;

        case 6: // Freckle
                // ZOOM 532 { "1 mm", "2 mm", "3 mm", "3.5 mm", "4 mm", "5 mm", "6 mm", "6.5 mm", "7 mm"  },
            AddEntryToList(&list, "lesion", "name", "Freckle");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Freckle 1");
            AddEntryToList(&list, "step 1", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 1", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 1", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 1", "energy", "30");
            AddEntryToList(&list, "step 1", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 2", "indication", "Freckle 2");
            AddEntryToList(&list, "step 2", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 2", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 2", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 2", "energy", "40");
            AddEntryToList(&list, "step 2", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 3", "indication", "Freckle 3");
            AddEntryToList(&list, "step 3", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 3", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 3", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 3", "energy", "50");
            AddEntryToList(&list, "step 3", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 4", "indication", "Freckle 4");
            AddEntryToList(&list, "step 4", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 4", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 4", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 4", "energy", "60");
            AddEntryToList(&list, "step 4", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 5", "indication", "Freckle 5");
            AddEntryToList(&list, "step 5", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 5", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 5", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 5", "energy", "70");
            AddEntryToList(&list, "step 5", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 6", "indication", "Freckle 6");
            AddEntryToList(&list, "step 6", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 6", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 6", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 6", "energy", "80");
            AddEntryToList(&list, "step 6", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 7", "indication", "Freckle 7");
            AddEntryToList(&list, "step 7", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 7", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 7", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 7", "energy", "90");
            AddEntryToList(&list, "step 7", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 8", "indication", "Freckle 8");
            AddEntryToList(&list, "step 8", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 8", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 8", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 8", "energy", "100");
            AddEntryToList(&list, "step 8", "frequency", "0");  //  "0" -> 1Hz
            break;

        case 7: // Lentigo
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "Lentigo");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Lentigo 1");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "0");
            AddEntryToList(&list, "step 1", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 1", "energy", "400");
            AddEntryToList(&list, "step 1", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 2", "indication", "Lentigo 2");
            AddEntryToList(&list, "step 2", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 2", "handpiece", "0");
            AddEntryToList(&list, "step 2", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 2", "energy", "450");
            AddEntryToList(&list, "step 2", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 3", "indication", "Lentigo 3");
            AddEntryToList(&list, "step 3", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 3", "handpiece", "0");
            AddEntryToList(&list, "step 3", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 3", "energy", "500");
            AddEntryToList(&list, "step 3", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 4", "indication", "Lentigo 4");
            AddEntryToList(&list, "step 4", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 4", "handpiece", "0");
            AddEntryToList(&list, "step 4", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 4", "energy", "550");
            AddEntryToList(&list, "step 4", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 5", "indication", "Lentigo 5");
            AddEntryToList(&list, "step 5", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 5", "handpiece", "0");
            AddEntryToList(&list, "step 5", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 5", "energy", "600");
            AddEntryToList(&list, "step 5", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 6", "indication", "Lentigo 6");
            AddEntryToList(&list, "step 6", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 6", "handpiece", "0");
            AddEntryToList(&list, "step 6", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 6", "energy", "650");
            AddEntryToList(&list, "step 6", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 7", "indication", "Lentigo 7");
            AddEntryToList(&list, "step 7", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 7", "handpiece", "0");
            AddEntryToList(&list, "step 7", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 7", "energy", "700");
            AddEntryToList(&list, "step 7", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 8", "indication", "Lentigo 8");
            AddEntryToList(&list, "step 8", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 8", "handpiece", "0");
            AddEntryToList(&list, "step 8", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 8", "energy", "750");
            AddEntryToList(&list, "step 8", "frequency", "0");  //  "0" -> 1Hz
            break;

        case 8: // PIH
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "PIH");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "PIH 1");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "0");
            AddEntryToList(&list, "step 1", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 1", "energy", "600");
            AddEntryToList(&list, "step 1", "frequency", "7");  //  "7" -> 8Hz

            AddEntryToList(&list, "step 2", "indication", "PIH 2");
            AddEntryToList(&list, "step 2", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 2", "handpiece", "0");
            AddEntryToList(&list, "step 2", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 2", "energy", "650");
            AddEntryToList(&list, "step 2", "frequency", "7");  //  "7" -> 8Hz

            AddEntryToList(&list, "step 3", "indication", "PIH 3");
            AddEntryToList(&list, "step 3", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 3", "handpiece", "0");
            AddEntryToList(&list, "step 3", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 3", "energy", "700");
            AddEntryToList(&list, "step 3", "frequency", "7");  //  "7" -> 8Hz

            AddEntryToList(&list, "step 4", "indication", "PIH 4");
            AddEntryToList(&list, "step 4", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 4", "handpiece", "0");
            AddEntryToList(&list, "step 4", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 4", "energy", "750");
            AddEntryToList(&list, "step 4", "frequency", "7");  //  "7" -> 8Hz

            AddEntryToList(&list, "step 5", "indication", "PIH 5");
            AddEntryToList(&list, "step 5", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 5", "handpiece", "0");
            AddEntryToList(&list, "step 5", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 5", "energy", "800");
            AddEntryToList(&list, "step 5", "frequency", "7");  //  "7" -> 8Hz

            AddEntryToList(&list, "step 6", "indication", "PIH 6");
            AddEntryToList(&list, "step 6", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 6", "handpiece", "0");
            AddEntryToList(&list, "step 6", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 6", "energy", "850");
            AddEntryToList(&list, "step 6", "frequency", "7");  //  "7" -> 8Hz

            AddEntryToList(&list, "step 7", "indication", "PIH 7");
            AddEntryToList(&list, "step 7", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 7", "handpiece", "0");
            AddEntryToList(&list, "step 7", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 7", "energy", "900");
            AddEntryToList(&list, "step 7", "frequency", "7");  //  "7" -> 8Hz

            AddEntryToList(&list, "step 8", "indication", "PIH 8");
            AddEntryToList(&list, "step 8", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 8", "handpiece", "0");
            AddEntryToList(&list, "step 8", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 8", "energy", "950");
            AddEntryToList(&list, "step 8", "frequency", "7");  //  "7" -> 8Hz
            break;

        case 9: // Seborrheic Keratosis
                // ZOOM 532 { "1 mm", "2 mm", "3 mm", "3.5 mm", "4 mm", "5 mm", "6 mm", "6.5 mm", "7 mm"  },
//            AddEntryToList(&list, "lesion", "name", "Seborrheic Keratosis");
            AddEntryToList(&list, "lesion", "name", "Seborrheic Kera.");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Seborrheic Kera. 1");
            AddEntryToList(&list, "step 1", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 1", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 1", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 1", "energy", "70");
            AddEntryToList(&list, "step 1", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 2", "indication", "Seborrheic Kera. 2");
            AddEntryToList(&list, "step 2", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 2", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 2", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 2", "energy", "80");
            AddEntryToList(&list, "step 2", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 3", "indication", "Seborrheic Kera. 3");
            AddEntryToList(&list, "step 3", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 3", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 3", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 3", "energy", "90");
            AddEntryToList(&list, "step 3", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 4", "indication", "Seborrheic Kera. 4");
            AddEntryToList(&list, "step 4", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 4", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 4", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 4", "energy", "100");
            AddEntryToList(&list, "step 4", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 5", "indication", "Seborrheic Kera. 5");
            AddEntryToList(&list, "step 5", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 5", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 5", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 5", "energy", "110");
            AddEntryToList(&list, "step 5", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 6", "indication", "Seborrheic Kera. 6");
            AddEntryToList(&list, "step 6", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 6", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 6", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 6", "energy", "120");
            AddEntryToList(&list, "step 6", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 7", "indication", "Seborrheic Kera. 7");
            AddEntryToList(&list, "step 7", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 7", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 7", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 7", "energy", "130");
            AddEntryToList(&list, "step 7", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 8", "indication", "Seborrheic Kera. 8");
            AddEntryToList(&list, "step 8", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 8", "handpiece", "1");  // ZOOM 532
            AddEntryToList(&list, "step 8", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 8", "energy", "140");
            AddEntryToList(&list, "step 8", "frequency", "0");  //  "0" -> 1Hz
            break;

        case 10: // Soft Peeling
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "Soft Peeling");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Soft Peeling 1");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "0");
            AddEntryToList(&list, "step 1", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 1", "energy", "400");
            AddEntryToList(&list, "step 1", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 2", "indication", "Soft Peeling 2");
            AddEntryToList(&list, "step 2", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 2", "handpiece", "0");
            AddEntryToList(&list, "step 2", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 2", "energy", "450");
            AddEntryToList(&list, "step 2", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 3", "indication", "Soft Peeling 3");
            AddEntryToList(&list, "step 3", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 3", "handpiece", "0");
            AddEntryToList(&list, "step 3", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 3", "energy", "500");
            AddEntryToList(&list, "step 3", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 4", "indication", "Soft Peeling 4");
            AddEntryToList(&list, "step 4", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 4", "handpiece", "0");
            AddEntryToList(&list, "step 4", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 4", "energy", "550");
            AddEntryToList(&list, "step 4", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 5", "indication", "Soft Peeling 5");
            AddEntryToList(&list, "step 5", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 5", "handpiece", "0");
            AddEntryToList(&list, "step 5", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 5", "energy", "600");
            AddEntryToList(&list, "step 5", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 6", "indication", "Soft Peeling 6");
            AddEntryToList(&list, "step 6", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 6", "handpiece", "0");
            AddEntryToList(&list, "step 6", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 6", "energy", "650");
            AddEntryToList(&list, "step 6", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 7", "indication", "Soft Peeling 7");
            AddEntryToList(&list, "step 7", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 7", "handpiece", "0");
            AddEntryToList(&list, "step 7", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 7", "energy", "700");
            AddEntryToList(&list, "step 7", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 8", "indication", "Soft Peeling 8");
            AddEntryToList(&list, "step 8", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 8", "handpiece", "0");
            AddEntryToList(&list, "step 8", "spot", "6");       // ZOOM 1064 6(8 mm)
            AddEntryToList(&list, "step 8", "energy", "750");
            AddEntryToList(&list, "step 8", "frequency", "9");  //  "9" -> 10Hz
            break;

        case 11: // Ochronosis
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "Ochronosis");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Ochronosis 1");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "0");
            AddEntryToList(&list, "step 1", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 1", "energy", "350");
            AddEntryToList(&list, "step 1", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 2", "indication", "Ochronosis 2");
            AddEntryToList(&list, "step 2", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 2", "handpiece", "0");
            AddEntryToList(&list, "step 2", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 2", "energy", "400");
            AddEntryToList(&list, "step 2", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 3", "indication", "Ochronosis 3");
            AddEntryToList(&list, "step 3", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 3", "handpiece", "0");
            AddEntryToList(&list, "step 3", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 3", "energy", "450");
            AddEntryToList(&list, "step 3", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 4", "indication", "Ochronosis 4");
            AddEntryToList(&list, "step 4", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 4", "handpiece", "0");
            AddEntryToList(&list, "step 4", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 4", "energy", "500");
            AddEntryToList(&list, "step 4", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 5", "indication", "Ochronosis 5");
            AddEntryToList(&list, "step 5", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 5", "handpiece", "0");
            AddEntryToList(&list, "step 5", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 5", "energy", "550");
            AddEntryToList(&list, "step 5", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 6", "indication", "Ochronosis 6");
            AddEntryToList(&list, "step 6", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 6", "handpiece", "0");
            AddEntryToList(&list, "step 6", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 6", "energy", "600");
            AddEntryToList(&list, "step 6", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 7", "indication", "Ochronosis 7");
            AddEntryToList(&list, "step 7", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 7", "handpiece", "0");
            AddEntryToList(&list, "step 7", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 7", "energy", "650");
            AddEntryToList(&list, "step 7", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 8", "indication", "Ochronosis 8");
            AddEntryToList(&list, "step 8", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 8", "handpiece", "0");
            AddEntryToList(&list, "step 8", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 8", "energy", "700");
            AddEntryToList(&list, "step 8", "frequency", "2");  //  "2" -> 3Hz
            break;

        case 12: // Acne Scar
                // MLA { "4 mm", "7 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "Acne Scar");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Acne Scar 1");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 1", "spot", "2");       // MLA 2(10 mm)
            AddEntryToList(&list, "step 1", "energy", "800");
            AddEntryToList(&list, "step 1", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 2", "indication", "Acne Scar 2");
            AddEntryToList(&list, "step 2", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 2", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 2", "spot", "2");       // MLA 2(10 mm)
            AddEntryToList(&list, "step 2", "energy", "850");
            AddEntryToList(&list, "step 2", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 3", "indication", "Acne Scar 3");
            AddEntryToList(&list, "step 3", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 3", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 3", "spot", "2");       // MLA 2(10 mm)
            AddEntryToList(&list, "step 3", "energy", "900");
            AddEntryToList(&list, "step 3", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 4", "indication", "Acne Scar 4");
            AddEntryToList(&list, "step 4", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 4", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 4", "spot", "2");       // MLA 2(10 mm)
            AddEntryToList(&list, "step 4", "energy", "950");
            AddEntryToList(&list, "step 4", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 5", "indication", "Acne Scar 5");
            AddEntryToList(&list, "step 5", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 5", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 5", "spot", "2");       // MLA 2(10 mm)
            AddEntryToList(&list, "step 5", "energy", "1000");
            AddEntryToList(&list, "step 5", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 6", "indication", "Acne Scar 6");
            AddEntryToList(&list, "step 6", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 6", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 6", "spot","2");       // MLA 2(10 mm)
            AddEntryToList(&list, "step 6", "energy", "1050");
            AddEntryToList(&list, "step 6", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 7", "indication", "Acne Scar 7");
            AddEntryToList(&list, "step 7", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 7", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 7", "spot", "2");       // MLA 2(10 mm)
            AddEntryToList(&list, "step 7", "energy", "1100");
            AddEntryToList(&list, "step 7", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 8", "indication", "Acne Scar 8");
            AddEntryToList(&list, "step 8", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 8", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 8", "spot", "2");       // MLA 2(10 mm)
            AddEntryToList(&list, "step 8", "energy", "1150");
            AddEntryToList(&list, "step 8", "frequency", "4");  //  "4" -> 5Hz
            break;

        case 13: // Acne Redness
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "Acne Redness");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Acne Redness 1");
            AddEntryToList(&list, "step 1", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 1", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 1", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 1", "energy", "3000");
            AddEntryToList(&list, "step 1", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 2", "indication", "Acne Redness 2");
            AddEntryToList(&list, "step 2", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 2", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 2", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 2", "energy", "3100");
            AddEntryToList(&list, "step 2", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 3", "indication", "Acne Redness 3");
            AddEntryToList(&list, "step 3", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 3", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 3", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 3", "energy", "3200");
            AddEntryToList(&list, "step 3", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 4", "indication", "Acne Redness 4");
            AddEntryToList(&list, "step 4", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 4", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 4", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 4", "energy", "3300");
            AddEntryToList(&list, "step 4", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 5", "indication", "Acne Redness 5");
            AddEntryToList(&list, "step 5", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 5", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 5", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 5", "energy", "3400");
            AddEntryToList(&list, "step 5", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 6", "indication", "Acne Redness 6");
            AddEntryToList(&list, "step 6", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 6", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 6", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 6", "energy", "3500");
            AddEntryToList(&list, "step 6", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 7", "indication", "Acne Redness 7");
            AddEntryToList(&list, "step 7", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 7", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 7", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 7", "energy", "3500");
            AddEntryToList(&list, "step 7", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 8", "indication", "Acne Redness 8");
            AddEntryToList(&list, "step 8", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 8", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 8", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 8", "energy", "3500");
            AddEntryToList(&list, "step 8", "frequency", "4");  //  "4" -> 5Hz
            break;

        case 14: // Rejuvenation
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "Rejuvenation");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Rejuvenation 1");
            AddEntryToList(&list, "step 1", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 1", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 1", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 1", "energy", "3000");
            AddEntryToList(&list, "step 1", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 2", "indication", "Rejuvenation 2");
            AddEntryToList(&list, "step 2", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 2", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 2", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 2", "energy", "3100");
            AddEntryToList(&list, "step 2", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 3", "indication", "Rejuvenation 3");
            AddEntryToList(&list, "step 3", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 3", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 3", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 3", "energy", "3200");
            AddEntryToList(&list, "step 3", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 4", "indication", "Rejuvenation 4");
            AddEntryToList(&list, "step 4", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 4", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 4", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 4", "energy", "3300");
            AddEntryToList(&list, "step 4", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 5", "indication", "Rejuvenation 5");
            AddEntryToList(&list, "step 5", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 5", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 5", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 5", "energy", "3400");
            AddEntryToList(&list, "step 5", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 6", "indication", "Rejuvenation 6");
            AddEntryToList(&list, "step 6", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 6", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 6", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 6", "energy", "3500");
            AddEntryToList(&list, "step 6", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 7", "indication", "Rejuvenation 7");
            AddEntryToList(&list, "step 7", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 7", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 7", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 7", "energy", "3500");
            AddEntryToList(&list, "step 7", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 8", "indication", "Rejuvenation 8");
            AddEntryToList(&list, "step 8", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 8", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 8", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 8", "energy", "3500");
            AddEntryToList(&list, "step 8", "frequency", "4");  //  "4" -> 5Hz
            break;

        case 15: // Pore Reducation
                // MLA { "4 mm", "7 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "Pore Reducation");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Pore Reducation 1");
            AddEntryToList(&list, "step 1", "wavelength", "2"); // Miin
            AddEntryToList(&list, "step 1", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 1", "spot", "2");       // MLA 2(10 mm)
            AddEntryToList(&list, "step 1", "energy", "1500");
            AddEntryToList(&list, "step 1", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 2", "indication", "Pore Reducation 2");
            AddEntryToList(&list, "step 2", "wavelength", "2"); // Miin
            AddEntryToList(&list, "step 2", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 2", "spot", "2");       // MLA 2(10 mm)
            AddEntryToList(&list, "step 2", "energy", "1600");
            AddEntryToList(&list, "step 2", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 3", "indication", "Pore Reducation 3");
            AddEntryToList(&list, "step 3", "wavelength", "2"); // Miin
            AddEntryToList(&list, "step 3", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 3", "spot", "2");       // MLA 2(10 mm)
            AddEntryToList(&list, "step 3", "energy", "1700");
            AddEntryToList(&list, "step 3", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 4", "indication", "Pore Reducation 4");
            AddEntryToList(&list, "step 4", "wavelength", "2"); // Miin
            AddEntryToList(&list, "step 4", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 4", "spot", "2");       // MLA 2(10 mm)
            AddEntryToList(&list, "step 4", "energy", "1800");
            AddEntryToList(&list, "step 4", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 5", "indication", "Pore Reducation 5");
            AddEntryToList(&list, "step 5", "wavelength", "2"); // Miin
            AddEntryToList(&list, "step 5", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 5", "spot", "2");       // MLA 2(10 mm)
            AddEntryToList(&list, "step 5", "energy", "1900");
            AddEntryToList(&list, "step 5", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 6", "indication", "Pore Reducation 6");
            AddEntryToList(&list, "step 6", "wavelength", "2"); // Miin
            AddEntryToList(&list, "step 6", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 6", "spot", "2");       // MLA 2(10 mm)
            AddEntryToList(&list, "step 6", "energy", "2000");
            AddEntryToList(&list, "step 6", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 7", "indication", "Pore Reducation 7");
            AddEntryToList(&list, "step 7", "wavelength", "2"); // Miin
            AddEntryToList(&list, "step 7", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 7", "spot", "2");       // MLA 2(10 mm)
            AddEntryToList(&list, "step 7", "energy", "2100");
            AddEntryToList(&list, "step 7", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 8", "indication", "Pore Reducation 8");
            AddEntryToList(&list, "step 8", "wavelength", "2"); // Miin
            AddEntryToList(&list, "step 8", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 8", "spot", "2");       // MLA 2(10 mm)
            AddEntryToList(&list, "step 8", "energy", "2200");
            AddEntryToList(&list, "step 8", "frequency", "2");  //  "2" -> 3Hz
            break;

        case 16: // Dark Eye Circles
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "Dark Eye Circles");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Dark Eye Circles 1");
            AddEntryToList(&list, "step 1", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 1", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 1", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 1", "energy", "2500");
            AddEntryToList(&list, "step 1", "frequency", "6");  //  "6" -> 7Hz

            AddEntryToList(&list, "step 2", "indication", "Dark Eye Circles 2");
            AddEntryToList(&list, "step 2", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 2", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 2", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 2", "energy", "2600");
            AddEntryToList(&list, "step 2", "frequency", "6");  //  "6" -> 7Hz

            AddEntryToList(&list, "step 3", "indication", "Dark Eye Circles 3");
            AddEntryToList(&list, "step 3", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 3", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 3", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 3", "energy", "2700");
            AddEntryToList(&list, "step 3", "frequency", "6");  //  "6" -> 7Hz

            AddEntryToList(&list, "step 4", "indication", "Dark Eye Circles 4");
            AddEntryToList(&list, "step 4", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 4", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 4", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 4", "energy", "2800");
            AddEntryToList(&list, "step 4", "frequency", "6");  //  "6" -> 7Hz

            AddEntryToList(&list, "step 5", "indication", "Dark Eye Circles 5");
            AddEntryToList(&list, "step 5", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 5", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 5", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 5", "energy", "2900");
            AddEntryToList(&list, "step 5", "frequency", "6");  //  "6" -> 7Hz

            AddEntryToList(&list, "step 6", "indication", "Dark Eye Circles 6");
            AddEntryToList(&list, "step 6", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 6", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 6", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 6", "energy", "3000");
            AddEntryToList(&list, "step 6", "frequency", "6");  //  "6" -> 7Hz

            AddEntryToList(&list, "step 7", "indication", "Dark Eye Circles 7");
            AddEntryToList(&list, "step 7", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 7", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 7", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 7", "energy", "3100");
            AddEntryToList(&list, "step 7", "frequency", "6");  //  "6" -> 7Hz

            AddEntryToList(&list, "step 8", "indication", "Dark Eye Circles 8");
            AddEntryToList(&list, "step 8", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 8", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 8", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 8", "energy", "3200");
            AddEntryToList(&list, "step 8", "frequency", "6");  //  "6" -> 7Hz
            break;

        case 17: // Dark Lips
                // ZOOM 532 { "1 mm", "2 mm", "3 mm", "3.5 mm", "4 mm", "5 mm", "6 mm", "6.5 mm", "7 mm"  },
            AddEntryToList(&list, "lesion", "name", "Dark Lips");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Dark Lips 1");
            AddEntryToList(&list, "step 1", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 1", "handpiece", "1"); // ZOOM 532
            AddEntryToList(&list, "step 1", "spot", "3");     // ZOOM 532 3(3.5 mm)
            AddEntryToList(&list, "step 1", "energy", "120");
            AddEntryToList(&list, "step 1", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 2", "indication", "Dark Lips 2");
            AddEntryToList(&list, "step 2", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 2", "handpiece", "1"); // ZOOM 532
            AddEntryToList(&list, "step 2", "spot", "3");     // ZOOM 532 3(3.5 mm)
            AddEntryToList(&list, "step 2", "energy", "130");
            AddEntryToList(&list, "step 2", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 3", "indication", "Dark Lips 3");
            AddEntryToList(&list, "step 3", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 3", "handpiece", "1"); // ZOOM 532
            AddEntryToList(&list, "step 3", "spot", "3");     // ZOOM 532 3(3.5 mm)
            AddEntryToList(&list, "step 3", "energy", "140");
            AddEntryToList(&list, "step 3", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 4", "indication", "Dark Lips 4");
            AddEntryToList(&list, "step 4", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 4", "handpiece", "1"); // ZOOM 532
            AddEntryToList(&list, "step 4", "spot", "3");     // ZOOM 532 3(3.5 mm)
            AddEntryToList(&list, "step 4", "energy", "150");
            AddEntryToList(&list, "step 4", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 5", "indication", "Dark Lips 5");
            AddEntryToList(&list, "step 5", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 5", "handpiece", "1"); // ZOOM 532
            AddEntryToList(&list, "step 5", "spot", "3");     // ZOOM 532 3(3.5 mm)
            AddEntryToList(&list, "step 5", "energy", "160");
            AddEntryToList(&list, "step 5", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 6", "indication", "Dark Lips 6");
            AddEntryToList(&list, "step 6", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 6", "handpiece", "1"); // ZOOM 532
            AddEntryToList(&list, "step 6", "spot", "3");     // ZOOM 532 3(3.5 mm)
            AddEntryToList(&list, "step 6", "energy", "170");
            AddEntryToList(&list, "step 6", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 7", "indication", "Dark Lips 7");
            AddEntryToList(&list, "step 7", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 7", "handpiece", "1"); // ZOOM 532
            AddEntryToList(&list, "step 7", "spot", "3");     // ZOOM 532 3(3.5 mm)
            AddEntryToList(&list, "step 7", "energy", "180");
            AddEntryToList(&list, "step 7", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 8", "indication", "Dark Lips 8");
            AddEntryToList(&list, "step 8", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 8", "handpiece", "1"); // ZOOM 532
            AddEntryToList(&list, "step 8", "spot", "3");     // ZOOM 532 3(3.5 mm)
            AddEntryToList(&list, "step 8", "energy", "190");
            AddEntryToList(&list, "step 8", "frequency", "0");  //  "0" -> 1Hz
            break;

        case 18: // Nail Fungus
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "Nail Fungus");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Nail Fungus 1");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 1", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 1", "energy", "700");
            AddEntryToList(&list, "step 1", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 2", "indication", "Nail Fungus 2");
            AddEntryToList(&list, "step 2", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 2", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 2", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 2", "energy", "750");
            AddEntryToList(&list, "step 2", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 3", "indication", "Nail Fungus 3");
            AddEntryToList(&list, "step 3", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 3", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 3", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 3", "energy", "800");
            AddEntryToList(&list, "step 3", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 4", "indication", "Nail Fungus 4");
            AddEntryToList(&list, "step 4", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 4", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 4", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 4", "energy", "850");
            AddEntryToList(&list, "step 4", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 5", "indication", "Nail Fungus 5");
            AddEntryToList(&list, "step 5", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 5", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 5", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 5", "energy", "900");
            AddEntryToList(&list, "step 5", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 6", "indication", "Nail Fungus 6");
            AddEntryToList(&list, "step 6", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 6", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 6", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 6", "energy", "950");
            AddEntryToList(&list, "step 6", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 7", "indication", "Nail Fungus 7");
            AddEntryToList(&list, "step 7", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 7", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 7", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 7", "energy", "1000");
            AddEntryToList(&list, "step 7", "frequency", "2");  //  "2" -> 3Hz

            AddEntryToList(&list, "step 8", "indication", "Nail Fungus 8");
            AddEntryToList(&list, "step 8", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 8", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 8", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 8", "energy", "1050");
            AddEntryToList(&list, "step 8", "frequency", "2");  //  "2" -> 3Hz
            break;

        case 19: // Nano Genesis
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "Nano Genesis");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Nano Genesis 1");
            AddEntryToList(&list, "step 1", "wavelength", "2"); // Miin
            AddEntryToList(&list, "step 1", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 1", "spot", "7");       // ZOOM 1064 7(9 mm)
            AddEntryToList(&list, "step 1", "energy", "3000");
            AddEntryToList(&list, "step 1", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 2", "indication", "Nano Genesis 2");
            AddEntryToList(&list, "step 2", "wavelength", "2"); // Miin
            AddEntryToList(&list, "step 2", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 2", "spot", "5");       // ZOOM 1064 5(7 mm)
            AddEntryToList(&list, "step 2", "energy", "3000");
            AddEntryToList(&list, "step 2", "frequency", "4");  //  "4" -> 5Hz

            AddEntryToList(&list, "step 3", "indication", "Nano Genesis 3");
            AddEntryToList(&list, "step 3", "wavelength", "2"); // Miin
            AddEntryToList(&list, "step 3", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 3", "spot", "3");       // ZOOM 1064 3(5 mm)
            AddEntryToList(&list, "step 3", "energy", "2000");
            AddEntryToList(&list, "step 3", "frequency", "5");  //  "5" -> 6Hz
            break;

        case 20: // Kaiser Method
                // DOE { "5 mm" },
            AddEntryToList(&list, "lesion", "name", "Kaiser Method");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Kaiser Method 1");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "3"); // DOE
            AddEntryToList(&list, "step 1", "spot", "0");       // DOE 0(5 mm)
            AddEntryToList(&list, "step 1", "energy", "250");
            AddEntryToList(&list, "step 1", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 2", "indication", "Kaiser Method 2");
            AddEntryToList(&list, "step 2", "wavelength", "1"); // 1064D
            AddEntryToList(&list, "step 2", "handpiece", "3"); // DOE
            AddEntryToList(&list, "step 2", "spot", "0");       // DOE 0(5 mm)
            AddEntryToList(&list, "step 2", "energy", "500");
            AddEntryToList(&list, "step 2", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 3", "indication", "Kaiser Method 3");
            AddEntryToList(&list, "step 3", "wavelength", "2"); // Miin
            AddEntryToList(&list, "step 3", "handpiece", "3"); // DOE
            AddEntryToList(&list, "step 3", "spot", "0");       // DOE 0(5 mm)
            AddEntryToList(&list, "step 3", "energy", "800");
            AddEntryToList(&list, "step 3", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 4", "indication", "Kaiser Method 4");
            AddEntryToList(&list, "step 4", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 4", "handpiece", "3"); // DOE
            AddEntryToList(&list, "step 4", "spot", "0");       // DOE 0(5 mm)
            AddEntryToList(&list, "step 4", "energy", "3500");
            AddEntryToList(&list, "step 4", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 5", "indication", "Kaiser Method 5");
            AddEntryToList(&list, "step 5", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 5", "handpiece", "3"); // DOE
            AddEntryToList(&list, "step 5", "spot", "0");       // DOE 0(5 mm)
            AddEntryToList(&list, "step 5", "energy", "350");
            AddEntryToList(&list, "step 5", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 6", "indication", "Kaiser Method 6");
            AddEntryToList(&list, "step 6", "wavelength", "1"); // 1064D
            AddEntryToList(&list, "step 6", "handpiece", "3"); // DOE
            AddEntryToList(&list, "step 6", "spot", "0");       // DOE 0(5 mm)
            AddEntryToList(&list, "step 6", "energy", "600");
            AddEntryToList(&list, "step 6", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 7", "indication", "Kaiser Method 7");
            AddEntryToList(&list, "step 7", "wavelength", "2"); // Miin
            AddEntryToList(&list, "step 7", "handpiece", "3"); // DOE
            AddEntryToList(&list, "step 7", "spot", "0");       // DOE 0(5 mm)
            AddEntryToList(&list, "step 7", "energy", "1100");
            AddEntryToList(&list, "step 7", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 8", "indication", "Kaiser Method 8");
            AddEntryToList(&list, "step 8", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 8", "handpiece", "3"); // DOE
            AddEntryToList(&list, "step 8", "spot", "0");       // DOE 0(5 mm)
            AddEntryToList(&list, "step 8", "energy", "3500");
            AddEntryToList(&list, "step 8", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 9", "indication", "Kaiser Method 9");
            AddEntryToList(&list, "step 9", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 9", "handpiece", "3"); // DOE
            AddEntryToList(&list, "step 9", "spot", "0");       // DOE 0(5 mm)
            AddEntryToList(&list, "step 9", "energy", "450");
            AddEntryToList(&list, "step 9", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 10", "indication", "Kaiser Method 10");
            AddEntryToList(&list, "step 10", "wavelength", "1"); // 1064D
            AddEntryToList(&list, "step 10", "handpiece", "3"); // DOE
            AddEntryToList(&list, "step 10", "spot", "0");       // DOE 0(5 mm)
            AddEntryToList(&list, "step 10", "energy", "700");
            AddEntryToList(&list, "step 10", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 11", "indication", "Kaiser Method 11");
            AddEntryToList(&list, "step 11", "wavelength", "2"); // Miin
            AddEntryToList(&list, "step 11", "handpiece", "3"); // DOE
            AddEntryToList(&list, "step 11", "spot", "0");       // DOE 0(5 mm)
            AddEntryToList(&list, "step 11", "energy", "1400");
            AddEntryToList(&list, "step 11", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 12", "indication", "Kaiser Method 12");
            AddEntryToList(&list, "step 12", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 12", "handpiece", "3"); // DOE
            AddEntryToList(&list, "step 12", "spot", "0");       // DOE 0(5 mm)
            AddEntryToList(&list, "step 12", "energy", "3500");
            AddEntryToList(&list, "step 12", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 13", "indication", "Kaiser Method 13");
            AddEntryToList(&list, "step 13", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 13", "handpiece", "3"); // DOE
            AddEntryToList(&list, "step 13", "spot", "0");       // DOE 0(5 mm)
            AddEntryToList(&list, "step 13", "energy", "800");
            AddEntryToList(&list, "step 13", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 14", "indication", "Kaiser Method 14");
            AddEntryToList(&list, "step 14", "wavelength", "1"); // 1064D
            AddEntryToList(&list, "step 14", "handpiece", "3"); // DOE
            AddEntryToList(&list, "step 14", "spot", "0");       // DOE 0(5 mm)
            AddEntryToList(&list, "step 14", "energy", "1300");
            AddEntryToList(&list, "step 14", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 15", "indication", "Kaiser Method 15");
            AddEntryToList(&list, "step 15", "wavelength", "2"); // Miin
            AddEntryToList(&list, "step 15", "handpiece", "3"); // DOE
            AddEntryToList(&list, "step 15", "spot", "0");       // DOE 0(5 mm)
            AddEntryToList(&list, "step 15", "energy", "2400");
            AddEntryToList(&list, "step 15", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 16", "indication", "Kaiser Method 16");
            AddEntryToList(&list, "step 16", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 16", "handpiece", "3"); // DOE
            AddEntryToList(&list, "step 16", "spot", "0");       // DOE 0(5 mm)
            AddEntryToList(&list, "step 16", "energy", "3500");
            AddEntryToList(&list, "step 16", "frequency", "9");  //  "9" -> 10Hz
            break;
#ifdef CUSTOM
        case 21: // Laser Toning
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "Laser Toning");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Laser Toning 1");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 1", "spot", "5");       // ZOOM 1064 5(7 mm)
            AddEntryToList(&list, "step 1", "energy", "850");
            AddEntryToList(&list, "step 1", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 2", "indication", "Laser Toning 2");
            AddEntryToList(&list, "step 2", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 2", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 2", "spot", "5");       // ZOOM 1064 5(7 mm)
            AddEntryToList(&list, "step 2", "energy", "900");
            AddEntryToList(&list, "step 2", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 3", "indication", "Laser Toning 3");
            AddEntryToList(&list, "step 3", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 3", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 3", "spot", "5");       // ZOOM 1064 5(7 mm)
            AddEntryToList(&list, "step 3", "energy", "1000");
            AddEntryToList(&list, "step 3", "frequency", "1");  //  "1" -> 2Hz

            AddEntryToList(&list, "step 4", "indication", "Laser Toning 4");
            AddEntryToList(&list, "step 4", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 4", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 4", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 4", "energy", "1000");
            AddEntryToList(&list, "step 4", "frequency", "0");  //  "0" -> 1Hz
            break;

        case 22: // Epidermis
                // ZOOM 532 { "1 mm", "2 mm", "3 mm", "3.5 mm", "4 mm", "5 mm", "6 mm", "6.5 mm", "7 mm"  },
            AddEntryToList(&list, "lesion", "name", "Epidermis");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Epidermis 1");
            AddEntryToList(&list, "step 1", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 1", "handpiece", "1"); // ZOOM 532
            AddEntryToList(&list, "step 1", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 1", "energy", "30");
            AddEntryToList(&list, "step 1", "frequency", "1");  //  "1" -> 2Hz

            AddEntryToList(&list, "step 2", "indication", "Epidermis 2");
            AddEntryToList(&list, "step 2", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 2", "handpiece", "1"); // ZOOM 532
            AddEntryToList(&list, "step 2", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 2", "energy", "60");
            AddEntryToList(&list, "step 2", "frequency", "1");  //  "1" -> 2Hz

            AddEntryToList(&list, "step 3", "indication", "Epidermis 3");
            AddEntryToList(&list, "step 3", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 3", "handpiece", "1"); // ZOOM 532
            AddEntryToList(&list, "step 3", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 3", "energy", "100");
            AddEntryToList(&list, "step 3", "frequency", "1");  //  "1" -> 2Hz

            AddEntryToList(&list, "step 4", "indication", "Epidermis 4");
            AddEntryToList(&list, "step 4", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 4", "handpiece", "1"); // ZOOM 532
            AddEntryToList(&list, "step 4", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 4", "energy", "120");
            AddEntryToList(&list, "step 4", "frequency", "1");  //  "1" -> 2Hz
            break;

        case 23: // Dermis
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "Dermis");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Dermis 1");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 1", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 1", "energy", "700");
            AddEntryToList(&list, "step 1", "frequency", "1");  //  "1" -> 2Hz

            AddEntryToList(&list, "step 2", "indication", "Dermis 2");
            AddEntryToList(&list, "step 2", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 2", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 2", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 2", "energy", "500");
            AddEntryToList(&list, "step 2", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 3", "indication", "Dermis 3");
            AddEntryToList(&list, "step 3", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 3", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 3", "spot", "4");       // ZOOM 1064 4(6 mm)
            AddEntryToList(&list, "step 3", "energy", "800");
            AddEntryToList(&list, "step 3", "frequency", "1");  //  "1" -> 2Hz

            AddEntryToList(&list, "step 4", "indication", "Dermis 4");
            AddEntryToList(&list, "step 4", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 4", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 4", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 4", "energy", "500");
            AddEntryToList(&list, "step 4", "frequency", "1");  //  "1" -> 2Hz

            AddEntryToList(&list, "step 5", "indication", "Dermis 5");
            AddEntryToList(&list, "step 5", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 5", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 5", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 5", "energy", "300");
            AddEntryToList(&list, "step 5", "frequency", "1");  //  "1" -> 2Hz

            AddEntryToList(&list, "step 6", "indication", "Dermis 6");
            AddEntryToList(&list, "step 6", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 6", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 6", "spot", "1");       // ZOOM 1064 1(3 mm)
            AddEntryToList(&list, "step 6", "energy", "500");
            AddEntryToList(&list, "step 6", "frequency", "1");  //  "1" -> 2Hz
            break;
#else
        case 21: // Custom1
                // MLA { "4 mm", "7 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "Laser Toning");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Toning");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 1", "spot", "1");       // MAL 1(7 mm)
            AddEntryToList(&list, "step 1", "energy", "850");
            AddEntryToList(&list, "step 1", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 2", "indication", "MLA-Toning");
            AddEntryToList(&list, "step 2", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 2", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 2", "spot", "1");       // MAL 1(7 mm)
            AddEntryToList(&list, "step 2", "energy", "900");
            AddEntryToList(&list, "step 2", "frequency", "9");  //  "9" -> 10Hz

            AddEntryToList(&list, "step 3", "indication", "MLA-Pore");
            AddEntryToList(&list, "step 3", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 3", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 3", "spot", "1");       // MAL 1(7 mm)
            AddEntryToList(&list, "step 3", "energy", "1000");
            AddEntryToList(&list, "step 3", "frequency", "1");  //  "1" -> 2Hz

            AddEntryToList(&list, "step 4", "indication", "MLA-Scar");
            AddEntryToList(&list, "step 4", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 4", "handpiece", "2"); // MLA
            AddEntryToList(&list, "step 4", "spot", "0");       // MAL 0(4 mm)
            AddEntryToList(&list, "step 4", "energy", "1000");
            AddEntryToList(&list, "step 4", "frequency", "0");  //  "0" -> 1Hz
            break;

        case 22: // Custom2
                // ZOOM 532 { "1 mm", "2 mm", "3 mm", "3.5 mm", "4 mm", "5 mm", "6 mm", "6.5 mm", "7 mm"  },
            AddEntryToList(&list, "lesion", "name", "Epidermis");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "Freckle");
            AddEntryToList(&list, "step 1", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 1", "handpiece", "1"); // ZOOM 532
            AddEntryToList(&list, "step 1", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 1", "energy", "30");
            AddEntryToList(&list, "step 1", "frequency", "1");  //  "1" -> 2Hz

            AddEntryToList(&list, "step 2", "indication", "Cafe-Au-Lait");
            AddEntryToList(&list, "step 2", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 1", "handpiece", "1"); // ZOOM 532
            AddEntryToList(&list, "step 2", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 2", "energy", "60");
            AddEntryToList(&list, "step 2", "frequency", "1");  //  "1" -> 2Hz

            AddEntryToList(&list, "step 3", "indication", "Seborrheic Keratosis");
            AddEntryToList(&list, "step 3", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 1", "handpiece", "1"); // ZOOM 532
            AddEntryToList(&list, "step 3", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 3", "energy", "100");
            AddEntryToList(&list, "step 3", "frequency", "1");  //  "1" -> 2Hz

            AddEntryToList(&list, "step 4", "indication", "Tattoo Red");
            AddEntryToList(&list, "step 4", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 1", "handpiece", "1"); // ZOOM 532
            AddEntryToList(&list, "step 4", "spot", "2");       // ZOOM 532 2(3 mm)
            AddEntryToList(&list, "step 4", "energy", "120");
            AddEntryToList(&list, "step 4", "frequency", "1");  //  "1" -> 2Hz
            break;

        case 23: // Custom3
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "Dermis");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "ABNOM");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 1", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 1", "energy", "700");
            AddEntryToList(&list, "step 1", "frequency", "1");  //  "1" -> 2Hz

            AddEntryToList(&list, "step 2", "indication", "Nuvus");
            AddEntryToList(&list, "step 2", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 2", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 2", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 2", "energy", "500");
            AddEntryToList(&list, "step 2", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 3", "indication", "Ota Ito Blue");
            AddEntryToList(&list, "step 3", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 3", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 3", "spot", "4");       // ZOOM 1064 4(6 mm)
            AddEntryToList(&list, "step 3", "energy", "800");
            AddEntryToList(&list, "step 3", "frequency", "1");  //  "1" -> 2Hz

            AddEntryToList(&list, "step 4", "indication", "Tattoo Body");
            AddEntryToList(&list, "step 4", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 4", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 4", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 4", "energy", "500");
            AddEntryToList(&list, "step 4", "frequency", "1");  //  "1" -> 2Hz

            AddEntryToList(&list, "step 5", "indication", "Tattoo Eyebrow");
            AddEntryToList(&list, "step 5", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 5", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 5", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 5", "energy", "300");
            AddEntryToList(&list, "step 5", "frequency", "1");  //  "1" -> 2Hz

            AddEntryToList(&list, "step 6", "indication", "Dark lips");
            AddEntryToList(&list, "step 6", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 6", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 6", "spot", "1");       // ZOOM 1064 1(3 mm)
            AddEntryToList(&list, "step 6", "energy", "500");
            AddEntryToList(&list, "step 6", "frequency", "1");  //  "1" -> 2Hz
            break;
#endif
        case 24: // memory
                // ZOOM 1064 { "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },
            AddEntryToList(&list, "lesion", "name", "memory");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "1064S");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 1", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 1", "energy", "600");
            AddEntryToList(&list, "step 1", "frequency", "0");  //  "0" -> 1Hz

            AddEntryToList(&list, "step 2", "indication", "1064S");
            AddEntryToList(&list, "step 2", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 2", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 2", "spot", "2");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 2", "energy", "650");
            AddEntryToList(&list, "step 2", "frequency", "1");

            AddEntryToList(&list, "step 3", "indication", "1064S");
            AddEntryToList(&list, "step 3", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 3", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 3", "spot", "2");       // ZOOM 1064 4(6 mm)
            AddEntryToList(&list, "step 3", "energy", "700");
            AddEntryToList(&list, "step 3", "frequency", "1");

            AddEntryToList(&list, "step 4", "indication", "1064D");
            AddEntryToList(&list, "step 4", "wavelength", "1"); // 1064D
            AddEntryToList(&list, "step 4", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 4", "spot", "3");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 4", "energy", "1200");
            AddEntryToList(&list, "step 4", "frequency", "2");

            AddEntryToList(&list, "step 5", "indication", "1064D");
            AddEntryToList(&list, "step 5", "wavelength", "1"); // 1064D
            AddEntryToList(&list, "step 5", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 5", "spot", "3");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 5", "energy", "1300");
            AddEntryToList(&list, "step 5", "frequency", "2");

            AddEntryToList(&list, "step 6", "indication", "1064D");
            AddEntryToList(&list, "step 6", "wavelength", "1"); // 1064D
            AddEntryToList(&list, "step 6", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 6", "spot", "3");       // ZOOM 1064 1(3 mm)
            AddEntryToList(&list, "step 6", "energy", "1400");
            AddEntryToList(&list, "step 6", "frequency", "2");

            AddEntryToList(&list, "step 7", "indication", "MiiN");
            AddEntryToList(&list, "step 7", "wavelength", "2"); // MiiN
            AddEntryToList(&list, "step 7", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 7", "spot", "3");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 7", "energy", "1500");
            AddEntryToList(&list, "step 7", "frequency", "5");

            AddEntryToList(&list, "step 8", "indication", "MiiN");
            AddEntryToList(&list, "step 8", "wavelength", "2"); // MiiN
            AddEntryToList(&list, "step 8", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 8", "spot", "3");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 8", "energy", "2000");
            AddEntryToList(&list, "step 8", "frequency", "5");

            AddEntryToList(&list, "step 9", "indication", "MiiN");
            AddEntryToList(&list, "step 9", "wavelength", "2"); // MiiN
            AddEntryToList(&list, "step 9", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 9", "spot", "3");       // ZOOM 1064 1(3 mm)
            AddEntryToList(&list, "step 9", "energy", "2500");
            AddEntryToList(&list, "step 9", "frequency", "5");

            AddEntryToList(&list, "step 10", "indication", "532");
            AddEntryToList(&list, "step 10", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 10", "handpiece", "1"); // ZOOM
            AddEntryToList(&list, "step 10", "spot", "3");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 10", "energy", "40");
            AddEntryToList(&list, "step 10", "frequency", "1");

            AddEntryToList(&list, "step 11", "indication", "532");
            AddEntryToList(&list, "step 11", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 11", "handpiece", "1"); // ZOOM
            AddEntryToList(&list, "step 11", "spot", "3");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 11", "energy", "60");
            AddEntryToList(&list, "step 11", "frequency", "1");

            AddEntryToList(&list, "step 12", "indication", "532");
            AddEntryToList(&list, "step 12", "wavelength", "3"); // 532
            AddEntryToList(&list, "step 12", "handpiece", "1"); // ZOOM
            AddEntryToList(&list, "step 12", "spot", "3");       // ZOOM 1064 1(3 mm)
            AddEntryToList(&list, "step 12", "energy", "80");
            AddEntryToList(&list, "step 12", "frequency", "1");

            AddEntryToList(&list, "step 13", "indication", "Quasi");
            AddEntryToList(&list, "step 13", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 13", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 13", "spot", "3");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 13", "energy", "1000");
            AddEntryToList(&list, "step 13", "frequency", "1");

            AddEntryToList(&list, "step 14", "indication", "Quasi");
            AddEntryToList(&list, "step 14", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 14", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 14", "spot", "3");       // ZOOM 1064 2(4 mm)
            AddEntryToList(&list, "step 14", "energy", "2000");
            AddEntryToList(&list, "step 14", "frequency", "1");

            AddEntryToList(&list, "step 15", "indication", "Quasi");
            AddEntryToList(&list, "step 15", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 15", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 15", "spot", "3");       // ZOOM 1064 1(3 mm)
            AddEntryToList(&list, "step 15", "energy", "3000");
            AddEntryToList(&list, "step 15", "frequency", "1");

            AddEntryToList(&list, "step 16", "indication", "temp");
            AddEntryToList(&list, "step 16", "wavelength", "4"); // Quasi
            AddEntryToList(&list, "step 16", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 16", "spot", "3");       // ZOOM 1064 1(3 mm)
            AddEntryToList(&list, "step 16", "energy", "200");
            AddEntryToList(&list, "step 16", "frequency", "1");
            break;

        default: // NONE
            AddEntryToList(&list, "lesion", "name", "NONE");
            AddEntryToList(&list, "lesion", "pos", "0");

            AddEntryToList(&list, "step 1", "indication", "NONE");
            AddEntryToList(&list, "step 1", "wavelength", "0"); // 1064S
            AddEntryToList(&list, "step 1", "handpiece", "0"); // ZOOM
            AddEntryToList(&list, "step 1", "spot", "0");
            AddEntryToList(&list, "step 1", "energy", "0");
            AddEntryToList(&list, "step 1", "frequency", "0");

            break;
    }

    printf("\nWriting %s\n", name);
    printf("=======================\n");

    /* now create the ini */
    if (0 != MakeINIFile(name, list))
    {
        printf("Error making %s file\n", name);
    }

    FreeList(list);

    return 0;
}

int init_configuration(void)
{
    int i;

    memset(&config, 0, sizeof(config_t));
    
    init_calibrate();
    init_product();
    init_config();

    for(i=0; i < 25; i++)
    {
        init_preset(i);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

#if 0

typedef struct my_struct_t
{
    int     myInt;          /*!< An integer value */
    float   myFloat;        /*!< An floating point value */
    char    myString[11];   /*!< A NULL terminated string */
} my_struct_t;

static int PopulateMyStruct(my_struct_t *my_struct, const ini_entry_t *entry)
{
    my_struct_t *ptr;

    /* use the XXX in struct XXX section as index into my_struct */
    if (strncmp("struct ", entry->section, 7) == 0)
    {
        int offset;

        offset = atoi(entry->section + 7);

        if (offset < 1)
        {
            return -1;      /* invalid number in struct_XXX */
        }

        offset--;
        ptr = my_struct + offset;
    }
    else
    {
        return -1;      /* unexpected section */
    }

    if (strcmp("int field", entry->key) == 0)
    {
        ptr->myInt = atoi(entry->value);
    }
    else if (strcmp("float field", entry->key) == 0)
    {
        ptr->myFloat = atof(entry->value);
    }
    else if (strcmp("str field", entry->key) == 0)
    {
        strncpy(ptr->myString, entry->value, 10);
    }
    else
    {
        return -1;      /* unexpected key */
    }

    return 0;
}

int ini_test()
{
    my_struct_t my_structs[2];
    int i;
    int result;
    FILE *fp;

    ini_entry_t entry;
    ini_entry_list_t list;

 
    /* build list of entries for MakeINI */
    list = NULL;
    AddEntryToList(&list, "struct 1", "int field", "123");
    AddEntryToList(&list, "struct 2", "str field", "string2");
    AddEntryToList(&list, "struct 1", "float field", "456.789");
    AddEntryToList(&list, "struct 2", "float field", "987.654");
    AddEntryToList(&list, "struct 1", "str field", "string1");
    AddEntryToList(&list, "struct 2", "int field", "321");

    printf("\nWriting test_struct.ini\n");
    printf("=======================\n");

    /* now create the ini */
    if (0 != MakeINIFile("/mnt/UDISK/config/test_struct.ini", list))
    {
        printf("Error making test_struct.ini file\n");
    }

    FreeList(list);

    printf("\nReading test_struct.ini\n");
    printf("=======================\n");
    fp = fopen("/mnt/UDISK/config/test_struct.ini", "r");

    /* initialize entry structure before reading first entry */
    entry.section = NULL;
    entry.key = NULL;
    entry.value = NULL;

    /* read ini file back into a structure */
    while ((result = GetEntryFromFile(fp, &entry)) > 0)
    {
        PopulateMyStruct(my_structs, &entry);
    }

    fclose(fp);

    if (result < 0)
    {
        printf("Error getting entry from test_struct.ini\n");
    }

    for (i = 0; i < 2; i++)
    {
        printf("struct %d\n", (i + 1));
        printf("\tmyInt %d\n", my_structs[i].myInt);
        printf("\tmyFloat %f\n", my_structs[i].myFloat);
        printf("\tmyString %s\n", my_structs[i].myString);
    }

    printf("\nModifying test_struct.ini\n");
    printf("=======================\n");

    if (0 != DeleteEntryFromFile("test_struct.ini", "struct 1", "int field"))
    {
        printf("Error deleting entry from test_struct.ini file\n");
    }

    list = NULL;
    AddEntryToList(&list, "struct 1", "int field", "1234");
    AddEntryToList(&list, "struct 2", "str field", "string2A");
    AddEntryToList(&list, "struct 1", "float field", "456.7890");
    AddEntryToList(&list, "struct 2", "float field", "987.6543");
    AddEntryToList(&list, "struct 1", "str field", "string1A");
    AddEntryToList(&list, "struct 2", "int field", "3210");

    if (0 != AddEntryToFile("/mnt/UDISK/config/test_struct.ini", list))
    {
        printf("Error deleting entry from test_struct.ini file\n");
    }

    fp = fopen("/mnt/UDISK/config/test_struct.ini", "r");

    /* read ini file back into a structure */
    while ((result = GetEntryFromFile(fp, &entry)) > 0)
    {
        PopulateMyStruct(my_structs, &entry);
    }

    fclose(fp);

    if (result < 0)
    {
        printf("Error getting entry from test_struct.ini\n");
    }

    for (i = 0; i < 2; i++)
    {
        printf("struct %d\n", (i + 1));
        printf("\tmyInt %d\n", my_structs[i].myInt);
        printf("\tmyFloat %f\n", my_structs[i].myFloat);
        printf("\tmyString %s\n", my_structs[i].myString);
    }

    remove("test_struct.ini");
    return 0;
}

#endif