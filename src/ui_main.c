#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>


#include "ui_main.h"
#include "config.h"
#include "functions.h"
#include "ui_calibration.h"
#include "uart.h"
#include "gpio.h"
#include "sound.h"
#include "ui_msg.h"

// zoom인식모드 s,d,q,m,532
// mla인식모드 s,d,q,m,532
// doe인식모드 s,d,q,m
static const char * const roller_spot[4] = { // zoom 1064S, zoom 1064D, zoom 532, mla, doe
"2 \n3 \n4 \n5 \n6 \n7 \n8 \n9 \n10 ",    // ZOOM 1064D
"1 \n2 \n3 \n3.5\n4 \n5 \n6 \n6.5\n7 ",    // ZOOM 532
"4 \n7 \n10 ",    // MLA
"5 ",    // DOE
};

static uint16_t _limit[5][3] = {
{100, 1400, 50}, 
{300, 2500, 100}, 
{500, 3000, 100}, 
{20, 260, 10}, 
{500, 3500, 100} 
};

static lv_obj_t ** ui_volume[5] = {&ui_btnVolume0, &ui_btnVolume0, &ui_btnVolume1, &ui_btnVolume2, &ui_btnVolume3 };
static lv_obj_t ** ui_standby[5] = {&ui_btnStandbyTime0, &ui_btnStandbyTime1, &ui_btnStandbyTime2, &ui_btnStandbyTime3, &ui_btnStandbyTime4 };

static lv_obj_t *volume_current_obj = NULL;

static int setup_open = false;
static int flag_memory = 0;
static int flag_frequency = 0;
static int flag_spotsize = 0;

static const double const size_spot[4][9] = { // zoom 532, zoom 1064, mla, doe
{3.141592, 7.068582, 12.566368, 19.63495, 28.274328, 38.484502, 50.265472, 63.617238, 78.5398},    // ZOOM 1064
{0.785398, 3.141592, 7.068582, 9.621126, 12.566368, 19.63495, 28.274328, 33.18307, 38.484502},    // ZOOM 532
{ 12.566368, 38.484502, 78.5398, 0, 0, 0, 0, 0, 0 },    // MLA
{ 19.63495, 0, 0, 0, 0, 0, 0, 0, 0 },    // DOE
};
#define    PI    3.141597


timer_t timer_shot;


static lv_style_t style_gray;
static lv_style_t style_red;
static lv_style_t style_green;

static pthread_t _ui_thread;


static int calc_fluence(int energy, int handpiece, int spot)
{
    int fluence;
    fluence = (int)(energy / size_spot[handpiece][spot] * 1);
    return fluence;
}

static int calc_fluence1(int energy, int handpiece, int spot)
{
    int fluence;
    fluence = (int)(energy / size_spot[handpiece][spot] * 10);
    return fluence;
}

static void event_clickable_toggle(lv_obj_t * obj, int flag)
{
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        lv_obj_t * child = lv_obj_get_child(obj, i);
        if(flag) {
            lv_obj_add_flag(child, LV_OBJ_FLAG_CLICKABLE);
        }
        else {
            lv_obj_clear_flag(child, LV_OBJ_FLAG_CLICKABLE);
        }
    }
}

static void disp_volume(void)
{
    for(int i = 0; i < 4; i++)
    _ui_flag_modify(*ui_volume[i], LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    _ui_flag_modify(*ui_volume[config.volume], LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
}

static void disp_standby(void)
{
    for(int i = 0; i < 5; i++)
    _ui_flag_modify(*ui_standby[i], LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(*ui_standby[config.standbytime], LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
}

static void disp_ready(void)
{
    static int old;
    if(config.ready > 0){
        system("tinyplayer tts/standby.mp3 &");
    }

    if(old != config.ready){
        _ui_state_modify(ui_btnReady, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
        config.energy = atoi(lv_label_get_text(ui_txtEnergy));
        config.voltage = convert_voltage(config.wavelength, config.energy);
        config.ready = lv_obj_get_state(ui_btnReady);
        if(config.ready > 1) config.ready = 0;
        old = config.ready;
    printf("disp_standby-->%d\n", config.ready);
        send_txd(PACKET_COMMAND_STANDBY);
    }
}

static void disp_freq(int value)
{
    config.frequency = value;
    _ui_roller_set_property(ui_RollerFreq, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, value);
}


int compare_handpiece(void)
{

//    rx value
//           1064         532

//   0:  HP_NONE;

// HP 1:  HP_ZOOM_2;   HP_ZOOM_1;  
//    2:  HP_ZOOM_3;   HP_ZOOM_2;
//    3:  HP_ZOOM_4;   HP_ZOOM_3;
//    4:  HP_ZOOM_5;   HP_ZOOM_3.5;
//    5:  HP_ZOOM_6;   HP_ZOOM_4;
//    6:  HP_ZOOM_7;   HP_ZOOM_5;
//    7:  HP_ZOOM_8;   HP_ZOOM_6;
//    8:  HP_ZOOM_9;   HP_ZOOM_6.5;
//    9:  HP_ZOOM_10;  HP_ZOOM_7;

//   10:  HP_1064DOE;
//   11:  HP_MLA_10;
//   12:  HP_MLA_4;
//   13:  HP_MLA_7;
//   14:  HP_Colimation;
//   15:  HP_595DYE;
//   16:  HP_532DOE;


//   config.handpiece 
//    0:  ZOOM1064
//    1:  ZOOM532
//    2:  MLA
//    3:  DOE

//    zoom인식모드 s,d,q,m,532
//    mla인식모드  s,d,q,m,532
//    doe인식모드  s,d,q,m

    int tmp = config.handpiece_rx;

    switch(config.handpiece)
    {
        case ZOOM_1064: case ZOOM_532:
            tmp = tmp - 1;
            break;
        case MLA:
            if(config.handpiece_rx == 12) tmp = 0;  // MLA  4 mm
            if(config.handpiece_rx == 13) tmp = 1;  // MLA  7 mm
            if(config.handpiece_rx == 11) tmp = 2;  // MLA 10 mm
            
            break;
        case DOE:
            if(config.handpiece_rx == 10) tmp = 0;  // MLA 10 mm
            break;
    }
    printf("\n\nrx-->%d, hp->%d, sp->%d, tmp->%d\n\n", config.handpiece_rx, config.handpiece, config.spotsize, tmp);
    
    if(config.spotsize != tmp) return false;

    return true; 
}


static void disp_spot(int hp, int value)
{
    config.spotsize = value;
    config.handpiece = hp;
    lv_roller_set_options(ui_RollerSpot, roller_spot[hp], LV_ROLLER_MODE_NORMAL);
    _ui_roller_set_property(ui_RollerSpot, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, value);
    printf("disp_spot : hp[%d], [%d]\n", hp, value);

    _ui_roller_set_property(ui_RollerHandPiece, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, hp);
    //if(config.hp_mode == 0) _ui_roller_set_property(ui_RollerHandPiece, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, hp);
}

static void disp_mode(int mode)
{
//    printf("disp_mode-->%d\n", mode);
    if(mode == 24) {
        _ui_flag_modify(ui_Grp_Memory, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify(ui_Grp_Easy, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        event_clickable_toggle(ui_GrpWaveSelect, true);
    }
    else {
        _ui_flag_modify(ui_Grp_Memory, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_Grp_Easy, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        event_clickable_toggle(ui_GrpWaveSelect, false);
    }
}

// (min + max) - lv_arc_get_value(src)
static void disp_guage_text(lv_obj_t * trg, int wavelength, int value)
{
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";
    int fluence;
    int result = value;
//    result = (min + max) - result;
//    result = (result / unit) * unit;
    result = (_limit[wavelength][0] + _limit[wavelength][1]) - result;
    result = (result/_limit[wavelength][2]) * _limit[wavelength][2];

    if(wavelength == WAVE_532){
        if(result > 200){
            result = 200 + (((result - 200) / 10) * 50); 
        }
    }

    config.energy = result;

    lv_snprintf(buf, sizeof(buf), "%d", result);
    lv_label_set_text(trg, buf);
    fluence = calc_fluence(result, preset[config.mode].dat[config.step[config.mode]].handpiece, preset[config.mode].dat[config.step[config.mode]].spot);
    lv_snprintf(buf, sizeof(buf), "%d.%d", fluence/10, fluence%10);
    lv_label_set_text(ui_txtFluence, buf);
    //lv_label_set_text_fmt(ui_txtFluence, "%d.%1d", fluence/100, fluence%100);
}

// (min + max) - lv_arc_get_value(src)
static void disp_guage_text_manual(lv_obj_t * trg, int wavelength, int value)
{
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";
    int fluence;
    int result = value;
//    result = (min + max) - result;
//    result = (result / unit) * unit;
    result = (_limit[wavelength][0] + _limit[wavelength][1]) - result;
    result = (result/_limit[wavelength][2]) * _limit[wavelength][2];

    if(wavelength == WAVE_532){
        if(result > 200){
            result = 200 + (((result - 200) / 10) * 50); 
        }
    }

    config.energy = result;

    lv_snprintf(buf, sizeof(buf), "%d", result);
    lv_label_set_text(trg, buf);
    fluence = calc_fluence(result, config.handpiece, config.spotsize);
    lv_snprintf(buf, sizeof(buf), "%d.%d", fluence/10, fluence%10);
    lv_label_set_text(ui_txtFluence, buf);
    //lv_label_set_text_fmt(ui_txtFluence, "1234", fluence/100, fluence%100);
}

static void disp_guage(int wavelength, int value)
{
//    printf("disp_guage-->%d\n", value);
    // 게이지에 맞게 변경
    int result = (_limit[wavelength][0] + _limit[wavelength][1]) - value;
    lv_arc_set_value(ui_GuageArc, result);
    disp_guage_text(ui_txtEnergy, wavelength, result);
}

static void disp_memory(int mode)
{
//   printf("disp_memory-->%d\n", mode);
    
    disp_ready();

    switch(mode)
    {
        case 0: case 3: case 6: case 9: case 12:
            _ui_state_modify(ui_btnMemory0, LV_STATE_CHECKED, _UI_MODIFY_STATE_ADD);
            _ui_state_modify(ui_btnMemory1, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
            _ui_state_modify(ui_btnMemory2, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
            break;
        case 1: case 4: case 7: case 10: case 13:
            _ui_state_modify(ui_btnMemory0, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
            _ui_state_modify(ui_btnMemory1, LV_STATE_CHECKED, _UI_MODIFY_STATE_ADD);
            _ui_state_modify(ui_btnMemory2, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
            break;
        case 2: case 5: case 8: case 11: case 14:
            _ui_state_modify(ui_btnMemory0, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
            _ui_state_modify(ui_btnMemory1, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
            _ui_state_modify(ui_btnMemory2, LV_STATE_CHECKED, _UI_MODIFY_STATE_ADD);
            break;
        case 15:
            _ui_state_modify(ui_btnMemory0, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
            _ui_state_modify(ui_btnMemory1, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
            _ui_state_modify(ui_btnMemory2, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
            break;
    }
}

//LV_IMG_DECLARE( ui_img_memory1_g_png);   // assets\memory1_r.png
//LV_IMG_DECLARE( ui_img_memory2_g_png);   // assets\memory2_r.png
//LV_IMG_DECLARE( ui_img_memory3_g_png);   // assets\memory3_r.png

static void disp_wave(int wavelength)
{
    static int flag_1064d = false;
    static int flag_532_draw = false;
//    printf("disp_wave-->%d\n", wavelength);
    lv_arc_set_range(ui_GuageArc, _limit[wavelength][0], _limit[wavelength][1]);

    _ui_flag_modify(ui_Image1064S0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_Image1064S1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_Image1064D0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_Image1064D1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_ImageMiin0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_ImageMiin1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_Image532R0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_Image532R1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_ImageQuasi0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_ImageQuasi1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    config.wavelength = wavelength;

    if((wavelength != WAVE_532) && (flag_532_draw == true)){
        printf("542 red\n");
        flag_532_draw = false;
        lv_obj_set_style_bg_img_src( ui_btnMemory0, &ui_img_memory1_r_png, LV_PART_MAIN | LV_STATE_CHECKED );
        lv_obj_set_style_bg_img_src( ui_btnMemory0, &ui_img_memory1_r_png, LV_PART_MAIN | LV_STATE_PRESSED );
        lv_obj_set_style_bg_img_src( ui_btnMemory1, &ui_img_memory2_r_png, LV_PART_MAIN | LV_STATE_CHECKED );
        lv_obj_set_style_bg_img_src( ui_btnMemory1, &ui_img_memory2_r_png, LV_PART_MAIN | LV_STATE_PRESSED );
        lv_obj_set_style_bg_img_src( ui_btnMemory2, &ui_img_memory3_r_png, LV_PART_MAIN | LV_STATE_CHECKED );
        lv_obj_set_style_bg_img_src( ui_btnMemory2, &ui_img_memory3_r_png, LV_PART_MAIN | LV_STATE_PRESSED );

        lv_obj_add_style(ui_btnReady, &style_red, LV_STATE_PRESSED);
        lv_obj_set_style_bg_img_src( ui_btnReady, &ui_img_ready_1_png, LV_PART_MAIN | LV_STATE_CHECKED );
        lv_obj_set_style_bg_img_src( ui_btnReady, &ui_img_ready_1_png, LV_PART_MAIN | LV_STATE_PRESSED );
//        lv_imgbtn_set_src(ui_btnReady, LV_IMGBTN_STATE_PRESSED, NULL, &ui_img_ready_1_png, NULL);
//        lv_imgbtn_set_src(ui_btnReady, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &ui_img_ready_1_png, NULL);
    }
    else if((wavelength == WAVE_532) && (flag_532_draw == false)){
        printf("542 green\n");
        flag_532_draw = true;
        lv_obj_set_style_bg_img_src( ui_btnMemory0, &ui_img_memory1_g_png, LV_PART_MAIN | LV_STATE_CHECKED );
        lv_obj_set_style_bg_img_src( ui_btnMemory0, &ui_img_memory1_g_png, LV_PART_MAIN | LV_STATE_PRESSED );
        lv_obj_set_style_bg_img_src( ui_btnMemory1, &ui_img_memory2_g_png, LV_PART_MAIN | LV_STATE_CHECKED );
        lv_obj_set_style_bg_img_src( ui_btnMemory1, &ui_img_memory2_g_png, LV_PART_MAIN | LV_STATE_PRESSED );
        lv_obj_set_style_bg_img_src( ui_btnMemory2, &ui_img_memory3_g_png, LV_PART_MAIN | LV_STATE_CHECKED );
        lv_obj_set_style_bg_img_src( ui_btnMemory2, &ui_img_memory3_g_png, LV_PART_MAIN | LV_STATE_PRESSED );

        lv_obj_add_style(ui_btnReady, &style_green, LV_STATE_PRESSED);
        lv_obj_set_style_bg_img_src( ui_btnReady, &ui_img_ready_2_png, LV_PART_MAIN | LV_STATE_CHECKED );
        lv_obj_set_style_bg_img_src( ui_btnReady, &ui_img_ready_2_png, LV_PART_MAIN | LV_STATE_PRESSED );
//        lv_imgbtn_set_src(ui_btnReady, LV_IMGBTN_STATE_PRESSED, NULL, &ui_img_ready_2_png, NULL);
//        lv_imgbtn_set_src(ui_btnReady, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &ui_img_ready_2_png, NULL);
    }


    switch(wavelength)
    {
        case WAVE_1064S:
            flag_1064d = false;
            _ui_flag_modify(ui_Image1064S1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_ImageMiin0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_Image532R0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_ImageQuasi0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            break;
        case WAVE_1064D:
            flag_1064d = true;
            _ui_flag_modify(ui_Image1064D1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_ImageMiin0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_Image532R0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_ImageQuasi0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            break;
        case WAVE_MiiN:
            if(flag_1064d) _ui_flag_modify(ui_Image1064D0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            else _ui_flag_modify(ui_Image1064S0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_ImageMiin1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_Image532R0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_ImageQuasi0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            break;
        case WAVE_532:
            if(flag_1064d) _ui_flag_modify(ui_Image1064D0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            else _ui_flag_modify(ui_Image1064S0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_ImageMiin0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_Image532R1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_ImageQuasi0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            break;
        case WAVE_QUASI:
            if(flag_1064d) _ui_flag_modify(ui_Image1064D0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            else _ui_flag_modify(ui_Image1064S0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_ImageMiin0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_Image532R0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_ImageQuasi1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            break;
    }
}

static void disp_easy_name(void)
{
    lv_label_set_text(ui_txtEasyName, preset[config.mode].dat[config.step[config.mode]].mark);
    lv_label_set_text_fmt(ui_txtStep, "STEP %d", config.step[config.mode] + 1);
}


void disp_update(void)
{
    disp_memory(config.step[config.mode]);
//    disp_mode(config.mode);
    disp_easy_name();
    disp_freq(preset[config.mode].dat[config.step[config.mode]].frequency);
    disp_spot(preset[config.mode].dat[config.step[config.mode]].handpiece, preset[config.mode].dat[config.step[config.mode]].spot);
    config.tmp_handpiece = config.handpiece;
    config.tmp_spotsize = config.spotsize;

    disp_wave(preset[config.mode].dat[config.step[config.mode]].wavelength);
    disp_guage(preset[config.mode].dat[config.step[config.mode]].wavelength, preset[config.mode].dat[config.step[config.mode]].energy);
}

static void freq_mode(int mode)
{
    switch(mode)
    {
        case 0:
            _ui_flag_modify(ui_btnFreqDown, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_ADD);
            _ui_flag_modify(ui_btnFreqUp, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_ADD);
            break;
        case 1:
            _ui_flag_modify(ui_btnFreqDown, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_btnFreqUp, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_REMOVE);
            break;
    }
}

static void event_btnFreqDown(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) system("tinyplayer tts/S_HW_Touch_Calm.mp3 &");
    if((event_code == LV_EVENT_CLICKED) ||(event_code == LV_EVENT_LONG_PRESSED_REPEAT)) {
        if(config.frequency > 0) config.frequency--;
        disp_freq(config.frequency);
        if(config.step[config.mode] != 15){
            disp_memory(15);
        }
    }
}
static void event_btnFreqUp(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) system("tinyplayer tts/S_HW_Touch_Calm.mp3 &");
    if((event_code == LV_EVENT_CLICKED) ||(event_code == LV_EVENT_LONG_PRESSED_REPEAT)) {
        int cnt = lv_roller_get_option_cnt(ui_RollerFreq) - 1;
        if(config.frequency < cnt) config.frequency++;
        disp_freq(config.frequency);
        if(config.step[config.mode] != 15){
            disp_memory(15);
        } 
    }
}


void event_btnSpotHpDown(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) system("tinyplayer tts/Beep_Once.mp3 &");
    if(event_code == LV_EVENT_CLICKED) {
        if(config.handpiece > 0) config.handpiece--;
        config.spotsize = 0;
        disp_spot(config.handpiece, config.spotsize);
        if(config.step[config.mode] != 15){
            disp_memory(15);
        }

        int value = atoi(lv_label_get_text(ui_txtEnergy));
        int result = (_limit[preset[config.mode].dat[config.step[config.mode]].wavelength][0] + _limit[preset[config.mode].dat[config.step[config.mode]].wavelength][1]) - value;
        disp_guage_text_manual(ui_txtEnergy, preset[config.mode].dat[config.step[config.mode]].wavelength, result);
    }
}

void event_btnSpotHpUp(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) system("tinyplayer tts/Beep_Once.mp3 &");
    if(event_code == LV_EVENT_CLICKED) {
        int cnt = lv_roller_get_option_cnt(ui_RollerHandPiece) - 1;
        if(config.handpiece < cnt) config.handpiece++;
        config.spotsize = 0;
        disp_spot(config.handpiece, config.spotsize);
        if(config.step[config.mode] != 15){
            disp_memory(15);
        }

        int value = atoi(lv_label_get_text(ui_txtEnergy));
        int result = (_limit[preset[config.mode].dat[config.step[config.mode]].wavelength][0] + _limit[preset[config.mode].dat[config.step[config.mode]].wavelength][1]) - value;
        disp_guage_text_manual(ui_txtEnergy, preset[config.mode].dat[config.step[config.mode]].wavelength, result);
    }
}


void event_RollerHandPiece(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED) {
        config.handpiece = lv_roller_get_selected(ui_RollerHandPiece);
        config.spotsize = 0;
        disp_spot(config.handpiece, config.spotsize);
        if(config.step[config.mode] != 15){
            disp_memory(15);
        } 

        int value = atoi(lv_label_get_text(ui_txtEnergy));
        int result = (_limit[preset[config.mode].dat[config.step[config.mode]].wavelength][0] + _limit[preset[config.mode].dat[config.step[config.mode]].wavelength][1]) - value;
        disp_guage_text_manual(ui_txtEnergy, preset[config.mode].dat[config.step[config.mode]].wavelength, result);
    }
}

void event_btnSpotDown(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) system("tinyplayer tts/S_HW_Touch_Calm.mp3 &");
    if(event_code == LV_EVENT_CLICKED) {
        if(config.spotsize > 0) config.spotsize--;
        disp_spot(config.handpiece, config.spotsize);
        if(config.step[config.mode] != 15){
            disp_memory(15);
        } 

        int value = atoi(lv_label_get_text(ui_txtEnergy));
        int result = (_limit[preset[config.mode].dat[config.step[config.mode]].wavelength][0] + _limit[preset[config.mode].dat[config.step[config.mode]].wavelength][1]) - value;
        disp_guage_text_manual(ui_txtEnergy, preset[config.mode].dat[config.step[config.mode]].wavelength, result);
    }
}

void event_btnSpotUp(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) system("tinyplayer tts/S_HW_Touch_Calm.mp3 &");
    if(event_code == LV_EVENT_CLICKED) {
        int cnt = lv_roller_get_option_cnt(ui_RollerSpot) - 1;
        if(config.spotsize < cnt) config.spotsize++;
        disp_spot(config.handpiece, config.spotsize);
        if(config.step[config.mode] != 15){
            disp_memory(15);
        } 

        int value = atoi(lv_label_get_text(ui_txtEnergy));
        int result = (_limit[preset[config.mode].dat[config.step[config.mode]].wavelength][0] + _limit[preset[config.mode].dat[config.step[config.mode]].wavelength][1]) - value;
        disp_guage_text_manual(ui_txtEnergy, preset[config.mode].dat[config.step[config.mode]].wavelength, result);
    }
}

static void spot_mode(int mode)
{
    switch(mode)
    {
        case HP_AUTO:
            lv_obj_add_state(ui_btnSpotMode, LV_STATE_CHECKED);
            _ui_flag_modify(ui_RollerHandPiece, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_REMOVE);
            lv_obj_set_size(ui_RollerHandPiece, 100, 42);
            lv_obj_set_size(ui_btnSpotMode, 108, 50);
            lv_obj_set_style_bg_color(ui_RollerHandPiece, lv_color_hex(0xF0F0F0), LV_PART_MAIN | LV_STATE_DEFAULT );
            lv_obj_set_style_text_color(ui_RollerHandPiece, lv_color_hex(0x000000), LV_PART_SELECTED | LV_STATE_DEFAULT );
            //_ui_flag_modify(ui_btnSpotDown, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_REMOVE);
            //_ui_flag_modify(ui_btnSpotUp, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_Grp_Spot_Hp, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            _ui_flag_modify(ui_Grp_Spot_Size, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            lv_label_set_text(ui_lblSpotMode, "AUTO");
            config.handpiece = config.tmp_handpiece;
            config.spotsize = config.tmp_spotsize;

            disp_spot(config.handpiece, config.spotsize);
            break;
        case HP_MANUAL:
            lv_obj_clear_state(ui_btnSpotMode, LV_STATE_CHECKED);
            _ui_flag_modify(ui_RollerHandPiece, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_ADD);
            lv_obj_set_size(ui_RollerHandPiece, 108, 50);
            lv_obj_set_size(ui_btnSpotMode, 100, 42);
            lv_obj_set_style_bg_color(ui_RollerHandPiece, lv_color_hex(0xAA0A0A), LV_PART_MAIN | LV_STATE_DEFAULT );
            lv_obj_set_style_text_color(ui_RollerHandPiece, lv_color_hex(0xFFFFFF), LV_PART_SELECTED | LV_STATE_DEFAULT );
            _ui_flag_modify(ui_btnSpotDown, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_ADD);
            _ui_flag_modify(ui_btnSpotUp, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_ADD);
            _ui_flag_modify(ui_Grp_Spot_Size, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            _ui_flag_modify(ui_Grp_Spot_Hp, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            lv_label_set_text(ui_lblSpotMode, "MANUAL");
            config.tmp_handpiece = config.handpiece;
            config.tmp_spotsize = config.spotsize;
            break;
    }
}

void event_btnSpotMode(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        if(config.step[config.mode] != 15){
            disp_memory(15);
        }
        
        if(config.hp_mode == HP_AUTO){
            config.hp_mode = HP_MANUAL;
            system("tinyplayer tts/manual.mp3 &");
            spot_mode(config.hp_mode);
        } 
        else {
            config.hp_mode = HP_AUTO;
            system("tinyplayer tts/auto.mp3 &");
            spot_mode(config.hp_mode);
        }
    }
}


static void disp_logo(void)
{
    switch(config.logo)
    {
        case LOGO_MiiN:
            lv_img_set_src(ui_ImageMiin0, &ui_img_miin_0_png);
            lv_img_set_src(ui_ImageMiin1, &ui_img_miin_1_png);
            _ui_flag_modify( ui_ImageLogo, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify( ui_ImageLogo1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            _ui_flag_modify( ui_ImageLogo2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            break;
        case LOGO_BRIGHTONE:
            lv_img_set_src(ui_ImageMiin0, &ui_img_miin_0_png);
            lv_img_set_src(ui_ImageMiin1, &ui_img_miin_1_png);
            _ui_flag_modify( ui_ImageLogo, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            _ui_flag_modify( ui_ImageLogo1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify( ui_ImageLogo2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            break;
        case LOGO_PEAKLIGHT:
            lv_img_set_src(ui_ImageMiin0, &ui_img_mp_0_png);
            lv_img_set_src(ui_ImageMiin1, &ui_img_mp_1_png);
            _ui_flag_modify( ui_ImageLogo, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            _ui_flag_modify( ui_ImageLogo1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            _ui_flag_modify( ui_ImageLogo2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            break;
    }
}


static void event_ImageLogo(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e); lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/brightone.mp3 &");
        config.logo = LOGO_BRIGHTONE;
        modify_config("config.ini", "config", "logo", "1");
        _ui_flag_modify( ui_ImageLogo1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify( ui_ImageLogo, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        UpRight_Animation(ui_ImageLogo1, 0);
    }
}

static void event_ImageLogo1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e); lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/peaklight.mp3 &");
        config.logo = LOGO_PEAKLIGHT;
        modify_config("config.ini", "config", "logo", "2");
        lv_img_set_src(ui_ImageMiin0, &ui_img_mp_0_png);
        lv_img_set_src(ui_ImageMiin1, &ui_img_mp_1_png);
        _ui_flag_modify( ui_ImageLogo2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify( ui_ImageLogo1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        UpLeft_Animation(ui_ImageLogo2, 0);
    }
}

static void event_ImageLogo2(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/miin.mp3 &");
        config.logo = LOGO_MiiN;
        modify_config("config.ini", "config", "logo", "0");
        lv_img_set_src(ui_ImageMiin0, &ui_img_miin_0_png);
        lv_img_set_src(ui_ImageMiin1, &ui_img_miin_1_png);
        _ui_flag_modify( ui_ImageLogo, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify( ui_ImageLogo2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        UpRight_Animation(ui_ImageLogo, 0);
    }
}

void save_volume(void)
{
    char key[10]; char value[10];
    //lv_snprintf(key, sizeof(key), "step%d", config.mode);
    lv_snprintf(value, sizeof(value), "%d", config.volume);
    send_txd(PACKET_COMMAND_VOLUME);
    modify_config("config.ini", "config", "volume", value);

}


static void event_btnVolume0(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("amixer set 'Headphone volume' 3 &");
        system("tinyplayer tts/this_voice.mp3 &");
        config.volume = 2;
        _ui_flag_modify(ui_btnVolume0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_btnVolume1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        save_volume();
    }
}

static void event_btnVolume1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("amixer set 'Headphone volume' 5 &");
        system("tinyplayer tts/this_voice.mp3 &");
        config.volume = 3;
        _ui_flag_modify(ui_btnVolume1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_btnVolume2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        save_volume();
     }
}

static void event_btnVolume2(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("amixer set 'Headphone volume' 7 &");
        system("tinyplayer tts/this_voice.mp3 &");
        config.volume = 4;
        _ui_flag_modify(ui_btnVolume2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_btnVolume3, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        save_volume();
     }
}

static void event_btnVolume3(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("amixer set 'Headphone volume' 1 &");
        system("tinyplayer tts/this_voice.mp3 &");
        config.volume = 1;
        _ui_flag_modify(ui_btnVolume0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify(ui_btnVolume3, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        save_volume();
     }
}

void save_standby_time(void)
{
    char key[10]; char value[10];
    //lv_snprintf(key, sizeof(key), "step%d", config.mode);
    lv_snprintf(value, sizeof(value), "%d", config.standbytime);
    modify_config("config.ini", "config", "standbytime", value);

}

void event_btnStandyby0(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/30sec.mp3 &");
        config.standbytime = 1;
        _ui_flag_modify(ui_btnStandbyTime0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_btnStandbyTime1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        save_standby_time();
    }
}

void event_btnStandyby1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/1min.mp3 &"); //system("tinyplayer tts/60second.mp3 &");
        config.standbytime = 2;
        _ui_flag_modify(ui_btnStandbyTime1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_btnStandbyTime2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        save_standby_time();
    }
}

void event_btnStandyby2(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/1min30sec.mp3 &"); //system("tinyplayer tts/90second.mp3 &");
        config.standbytime = 3;
        _ui_flag_modify(ui_btnStandbyTime2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_btnStandbyTime3, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        save_standby_time();
    }
}

void event_btnStandyby3(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/2min.mp3 &");
        config.standbytime = 4;
        _ui_flag_modify(ui_btnStandbyTime3, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_btnStandbyTime4, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        save_standby_time();
    }
}

void event_btnStandyby4(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/0sec.mp3 &");
        config.standbytime = 0;
        _ui_flag_modify(ui_btnStandbyTime4, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_btnStandbyTime0, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        save_standby_time();
    }
}

static void event_Image1064S0(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/Milky_Way.mp3 &");
        config.step[config.mode] = (3 * WAVE_1064S);
        disp_update();
        UpLeft_Animation(ui_Image1064S1, 0);
    }
}

static void event_Image1064S1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/Milky_Way.mp3 &");
        config.step[config.mode] = (3 * WAVE_1064D);
        disp_update();
        UpLeft_Animation(ui_Image1064D1, 0);
    }
}

static void event_Image1064D0(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/Milky_Way.mp3 &");
        config.step[config.mode] = (3 * WAVE_1064D);
        disp_update();
        UpLeft_Animation(ui_Image1064D1, 0);
    }
}

static void event_Image1064D1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/Milky_Way.mp3 &");
        config.step[config.mode] = (3 * WAVE_1064S);
        disp_update();
        UpRight_Animation(ui_Image1064S1, 0);
    }
}

static void event_ImageMiin0(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/Milky_Way.mp3 &");
        config.step[config.mode] = (3 * WAVE_MiiN);
        disp_update();
        DownRight_Animation(ui_ImageMiin1, 0);
    }
}
static void event_Image532R0(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/Milky_Way.mp3 &");
        config.step[config.mode] = (3 * WAVE_532);
        disp_update();
        DownLeft_Animation(ui_Image532R1, 0);
    }
}
static void event_ImageQuasi0(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/Milky_Way.mp3 &");
        config.step[config.mode] = (3 * WAVE_QUASI);
        disp_update();
        UpRight_Animation(ui_ImageQuasi1, 0);
    }
}


static void event_grp_memory_handler(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/easy_mode.mp3 &");
        config.mode = config.temp;
        ds1307_write();
        disp_update();

        UpRight_Animation(ui_Grp_Easy, 0);
        _ui_flag_modify(ui_Grp_Memory, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_Grp_Easy, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        event_clickable_toggle(ui_GrpWaveSelect, false);
    }
}

static void event_grp_preset_handler(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/memory_mode.mp3 &");
        config.temp = config.mode;
        config.mode = 24;
        ds1307_write();
        disp_update();

        UpLeft_Animation(ui_Grp_Memory, 0);
        _ui_flag_modify(ui_Grp_Easy, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_Grp_Memory, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        event_clickable_toggle(ui_GrpWaveSelect, true);
    }
}

void event_btnStepDown(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) system("tinyplayer tts/S_HW_Touch_Calm.mp3 &");
    if(event_code == LV_EVENT_CLICKED) {
        if(config.step[config.mode] > 0) config.step[config.mode]--; 
        ds1307_write();
        disp_update();
    }
}

void event_btnStepUp(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) system("tinyplayer tts/S_HW_Touch_Calm.mp3 &");
    if(event_code == LV_EVENT_CLICKED) {
        if(config.step[config.mode] < preset[config.mode].count - 1) config.step[config.mode]++; 
        ds1307_write();
        disp_update();
    }
}

static lv_obj_t *memory_current_obj = NULL;
static int select_memory = 0;
static int ani_memory_long = 0;
static int ani_memory_click = 0;
static lv_obj_t ** ui_memory[3] = {&ui_btnMemory0, &ui_btnMemory1, &ui_btnMemory2};

static void event_memory_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    static int long_flag = 0;
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";

    if(code == LV_EVENT_LONG_PRESSED) {
        long_flag = 1; // long후 click가 되기때문에 click에서 구별 사용한다
        memory_current_obj = obj;
        lv_obj_t * parent = lv_obj_get_parent(obj);
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == memory_current_obj) {
                select_memory = i;
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else {
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }

        int val = (config.step[config.mode] / 3);
        config.step[config.mode] = (3 * val) + select_memory;

        preset[config.mode].dat[config.step[config.mode]].energy = atoi(lv_label_get_text(ui_txtEnergy));
        preset[config.mode].dat[config.step[config.mode]].frequency = config.frequency;
        preset[config.mode].dat[config.step[config.mode]].spot = config.spotsize;
        preset[config.mode].dat[config.step[config.mode]].handpiece = config.handpiece;

        //변경 사항 저장
        make_preset_modify(24);
//printf("memory save->%d, %d\n", select_memory, config.step[config.mode]);
        if(select_memory == 0)      system("tinyplayer tts/save_memory1.mp3 &");
        else if(select_memory == 1) system("tinyplayer tts/save_memory2.mp3 &");
        else if(select_memory == 2) system("tinyplayer tts/save_memory3.mp3 &");


        switch(ani_memory_long)
        {
            case 0:
                RotateCW_Animation(ui_ImageMemoryBg, 0);
                ani_memory_long = 1;
                break;
            case 1:
                RotateCCW_Animation(ui_ImageMemoryBg, 0);
                ani_memory_long = 0;
                break;
        }

    }
    else if(code == LV_EVENT_CLICKED) {
        if(long_flag){
            long_flag = 0;
            return;
        }
        memory_current_obj = obj;
        lv_obj_t * parent = lv_obj_get_parent(obj);
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == memory_current_obj) {
                select_memory = i;
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else {
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }

        int val = (config.step[config.mode] / 3);
        config.step[config.mode] = (3 * val) + select_memory;

        ds1307_write();

        if(select_memory == 0)      system("tinyplayer tts/load_memory1.mp3 &");
        else if(select_memory == 1) system("tinyplayer tts/load_memory2.mp3 &");
        else if(select_memory == 2) system("tinyplayer tts/load_memory3.mp3 &");
       
#if 1
        switch(ani_memory_click)
        {
            case 0:
                UpLeft_Animation(*ui_memory[select_memory], 0);
                ani_memory_click = 1;
                break;
            case 1:
                UpRight_Animation(*ui_memory[select_memory], 0);
                ani_memory_click = 0;
                break;
        }
#endif
        disp_update();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void draw_txtCount(void)
{
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";
    if(config.flag_shotcount == 0){
        int2comma(buf, config.shotcount, COUNT_WIDTH);
        //lv_snprintf(buf, sizeof(buf), "%d", config.shotcount);
        lv_label_set_text(ui_txtCount, buf);
    }
    // standby count reset
    config.standby_count = 0;
}
 
void draw_txtCountTotal(void)
{
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";;
    int2comma(buf, product.total, COUNT_WIDTH);
//printf("product.total->%d, %s\n", product.total, buf);
//    lv_snprintf(buf, sizeof(buf), "%d", product.total);
    lv_label_set_text(ui_txtCountTotal, buf);
    // standby count reset
    config.standby_count = 0;
}

static void event_btnCount(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) system("tinyplayer tts/S_HW_Touch_Calm.mp3 &");
    if(event_code == LV_EVENT_LONG_PRESSED) {
        system("tinyplayer tts/shotcount_clear.mp3 &");
        config.shotcount = 0;
        lv_label_set_text(ui_txtCount, "0");
        send_txd(PACKET_COMMAND_SHOTCOUNT_ZERO);
        //draw_txtCount();
    }
}

static void calc_shotcount_view(void)
{
    if(++config.flag_shotcount_count > 3)
    { 
        config.flag_shotcount = 0;
        config.flag_shotcount_count = 0;
        _ui_flag_modify( ui_Grp_Count_Total, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify( ui_Grp_Count, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    }
}

static void event_Grp_Count_Label( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_CLICKED) {
        system("tinyplayer tts/shotcount_total.mp3 &");
        config.flag_shotcount = 1;
        config.flag_shotcount_count = 0;
        draw_txtCountTotal();

        _ui_flag_modify( ui_Grp_Count, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify( ui_Grp_Count_Total, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    }
}

static void event_Grp_Count_Label_Total( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_CLICKED) {
        config.flag_shotcount = 0;
        config.flag_shotcount_count = 0;
        _ui_flag_modify( ui_Grp_Count_Total, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify( ui_Grp_Count, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    }
}



void event_txtEasyName(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) system("tinyplayer tts/DingSound.mp3 &");
    if(event_code == LV_EVENT_PRESSED) {
        _ui_screen_change(&ui_ScreenPreset, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenPreset_screen_init);
    }
}

void event_Slider_Aim(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    static clock_t start = 0, end = 0;

    if((event_code == LV_EVENT_VALUE_CHANGED) || (event_code == LV_EVENT_LONG_PRESSED_REPEAT)) {
        end = clock();
        clock_t repeat = end - start;
        if(repeat > 50000){ // 80 ms
           config.aiming = lv_slider_get_value(target);
            send_txd(PACKET_COMMAND_AIMING);
            config.flag_aim_write = 1;
            //printf("event_Slider_Aim->%d\n", config.aiming);
            start = end;
         }   
    }
}

static void event_GuageArc(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);

    if(event_code == LV_EVENT_VALUE_CHANGED) {
        if(config.step[config.mode] != 15){
            disp_memory(15);
        } 
        int value = (int)lv_arc_get_value(target);
        disp_guage_text(ui_txtEnergy, preset[config.mode].dat[config.step[config.mode]].wavelength, value);
    }
}

static void event_btnEnergyDown(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) system("tinyplayer tts/S_HW_Touch_Calm.mp3 &");

    if((event_code == LV_EVENT_CLICKED) || (event_code == LV_EVENT_LONG_PRESSED_REPEAT)){
        if(config.step[config.mode] != 15){
            disp_memory(15);
        } 
        int value = (int)lv_arc_get_value(ui_GuageArc);
        if(value < _limit[preset[config.mode].dat[config.step[config.mode]].wavelength][1]) value += _limit[preset[config.mode].dat[config.step[config.mode]].wavelength][2];
        lv_arc_set_value(ui_GuageArc, value);
        disp_guage_text(ui_txtEnergy, preset[config.mode].dat[config.step[config.mode]].wavelength, value);
    }
}

static void event_btnEnergyUp(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) system("tinyplayer tts/S_HW_Touch_Calm.mp3 &");

    if((event_code == LV_EVENT_CLICKED) || (event_code == LV_EVENT_LONG_PRESSED_REPEAT)){
        if(config.step[config.mode] != 15){
            disp_memory(15);
        } 
        int value = (int)lv_arc_get_value(ui_GuageArc);
        if(value > _limit[preset[config.mode].dat[config.step[config.mode]].wavelength][0]) value -= _limit[preset[config.mode].dat[config.step[config.mode]].wavelength][2];
        lv_arc_set_value(ui_GuageArc, value);
        disp_guage_text(ui_txtEnergy, preset[config.mode].dat[config.step[config.mode]].wavelength, value);
    }
}

void event_btnReady(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);

    if(event_code == LV_EVENT_CLICKED) {
        int cmp = compare_handpiece();

        config.energy = atoi(lv_label_get_text(ui_txtEnergy));
        config.voltage = convert_voltage(config.wavelength, config.energy);
        config.ready = lv_obj_get_state(ui_btnReady) % 2;

        if(config.hp_mode == HP_AUTO){
            if((config.ready == true) && (cmp == true)){
                system("tinyplayer tts/ready.mp3 &");
            }
            else if((config.ready == true) && (cmp == false)){
                system("tinyplayer tts/warning_handpiece.mp3 &");
                config.flag_handpiece = 1;
                lv_msgbox_show(0);
                config.ready = 0;
                _ui_state_modify(ui_btnReady, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
            }
            else{
                config.ready = 0;
                system("tinyplayer tts/standby.mp3 &");
                _ui_state_modify(ui_btnReady, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
            }
            send_txd(PACKET_COMMAND_READY);
        }
        else{
            if(config.ready == true){
                system("tinyplayer tts/ready.mp3 &");
            }
            else{
                system("tinyplayer tts/standby.mp3 &");
            }
            send_txd(PACKET_COMMAND_READY);
        }
    }
}

static void off_standby(void)
{
    printf("off_standby : %d \n", config.standbytime);
    _ui_state_modify(ui_btnReady, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
    lv_event_send(ui_btnReady, LV_EVENT_CLICKED, NULL);
}

static void calc_standby(void)
{
    switch(config.standbytime)
    {
        case 0:
            config.standby_count = 0;
            break;
        case 1:
            if(++config.standby_count > 30) { config.standby_count = 0; off_standby(); }
            break;
        case 2:
            if(++config.standby_count > 60) { config.standby_count = 0; off_standby(); }
            break;
        case 3:
            if(++config.standby_count > 90) { config.standby_count = 0; off_standby(); }
            break;
        case 4:
            if(++config.standby_count > 120) { config.standby_count = 0; off_standby(); }
            break;
    }        
//printf("convert : %d \n", config.standby_count);
}


void write_datetime(void)
{
    char date[6][5] = {0,};
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";
	int cnt = 0;
	int err = 0;
    int year, mon, day, hour, min, sec;

    sprintf(buf, "%s", lv_label_get_text(ui_txtDate));

    char *ptr = strtok(buf, "/:- ");    //구분자는 콤마(,)입니다.

    while (ptr != NULL)
    {
        lv_snprintf(date[cnt], 5, "%s", ptr);
//        printf("(%d)%s\n", cnt, date[cnt]);
        cnt++;
        ptr = strtok(NULL, "/:- ");
    }
/*    
    for (int idx = 0; idx < 4; ++idx)
    {
        if(isdigit(date[0][idx]) == 0)
        printf("cal year error! arr[%d]\t char: %c\tisdigit: %d\n", idx, date[0][idx], isdigit(date[0][idx]));
    }

    for (int idx = 0; idx < 2; ++idx)
    {
        if(isdigit(date[1][idx]) == 0)
        printf("cal month error! arr[%d]\t char: %c\tisdigit: %d\n", idx, date[1][idx], isdigit(date[1][idx]));
    }

    for (int idx = 0; idx < 2; ++idx)
    {
        if(isdigit(date[2][idx]) == 0)
        printf("cal date error! arr[%d]\t char: %c\tisdigit: %d\n", idx, date[2][idx], isdigit(date[2][idx]));
    }

    for (int idx = 0; idx < 2; ++idx)
    {
        if(isdigit(date[3][idx]) == 0)
        printf("cal hours error! arr[%d]\t char: %c\tisdigit: %d\n", idx, date[3][idx], isdigit(date[3][idx]));
    }

    for (int idx = 0; idx < 2; ++idx)
    {
        if(isdigit(date[4][idx]) == 0)
        printf("cal minutes error! arr[%d]\t char: %c\tisdigit: %d\n", idx, date[4][idx], isdigit(date[4][idx]));
    }
*/
    year = atoi(date[0]);
    mon  = atoi(date[1]);
    day  = atoi(date[2]);
    hour = atoi(date[3]);
    min  = atoi(date[4]);
    sec  = 0;
    
    set_systemtime(year, mon, day, hour, min, sec);
    config.flag_rtc_write = 0;
}

void write_aim(void)
{
    char key[10]; char value[10];
    //lv_snprintf(key, sizeof(key), "step%d", config.mode);
    lv_snprintf(value, sizeof(value), "%d", config.aiming);
    modify_config("config.ini", "config", "aiming", value);
    config.flag_aim_write = 0;
}


void event_btnSetup( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_CLICKED) {
        _ui_screen_change( &ui_ScreenCalibration, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenCalibration_screen_init);
        //_ui_screen_change( &ui_ScreenCalibration, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 500, 0, &ui_ScreenCalibration_screen_init);
    }
}

static void *ui_thread(void *arg)
{
    lv_res_t res;
    int cnt = 0;
    int flag = 0;
    int cmp = 0;

    config.flag_shotcount = config.flag_rtc_write = config.ready = 0;
	while(1) {

        switch(cnt)
        {
            case 0:
                if(config.ready) calc_standby();
                break;
            case 2:
                if(config.flag_shotcount) calc_shotcount_view();
                break;
            case 4:
                if(config.flag_rtc_write) write_datetime();
                break;
            case 6:
                if(config.flag_aim_write) write_aim();
                break;
            case 1: case 3: case 5: case 7: case 9:
                if(config.flag_handpiece == 1){
                    cmp = compare_handpiece();
                    if(cmp == 1){
                        lv_msgbox_close(msg_box);
                        config.flag_handpiece = 0;
                    } 
                }
//                if(config.flag_vol_write) write_vol();
                break;
        }

        if(++cnt > 9) cnt = 0;

    
        usleep(100000); // 100 ms
	} // while
}



void event_main_init(void)
{
    printf("event_main_init-->%d, %d\n", config.mode, config.step[config.mode]);
 

    lv_style_init(&style_gray);

    /*Add a large outline when pressed*/
    lv_style_set_outline_width(&style_gray, 30);
    lv_style_set_outline_opa(&style_gray, LV_OPA_TRANSP);
    lv_style_set_outline_color(&style_gray, lv_palette_darken(LV_PALETTE_BLUE_GREY, 2));

    lv_style_set_translate_y(&style_gray, 5);
    lv_style_set_shadow_ofs_y(&style_gray, 3);

    /*Add a transition to the outline*/
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0};
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, TRANSITION_TIME, 0, NULL);

    lv_style_init(&style_red);

    /*Add a large outline when pressed*/
    lv_style_set_outline_width(&style_red, 30);
    lv_style_set_outline_opa(&style_red, LV_OPA_TRANSP);
    lv_style_set_outline_color(&style_red, lv_palette_darken(LV_PALETTE_RED, 2));

    lv_style_set_translate_y(&style_red, 5);
    lv_style_set_shadow_ofs_y(&style_red, 3);

    lv_style_init(&style_green);

    /*Add a large outline when pressed*/
    lv_style_set_outline_width(&style_green, 30);
    lv_style_set_outline_opa(&style_green, LV_OPA_TRANSP);
    lv_style_set_outline_color(&style_green, lv_palette_darken(LV_PALETTE_GREEN, 2));

    lv_style_set_translate_y(&style_green, 5);
    lv_style_set_shadow_ofs_y(&style_green, 3);

    static lv_style_t style_pr2;
    lv_style_init(&style_pr2);

    /*Add a large outline when pressed*/
//    lv_style_set_outline_width(&style_pr2, 30);
//    lv_style_set_outline_opa(&style_pr2, LV_OPA_TRANSP);
//    lv_style_set_outline_color(&style_pr2, lv_palette_darken(LV_PALETTE_RED, 2));

    lv_style_set_translate_y(&style_pr2, 5);
    lv_style_set_shadow_ofs_y(&style_pr2, 3);

    lv_style_set_transition(&style_gray, &trans);
    lv_style_set_transition(&style_red, &trans);

    lv_label_set_text(ui_txtCount, "0");

    lv_obj_add_style(ui_btnFreqDown, &style_gray, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnFreqUp, &style_gray, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnSpotHpDown, &style_gray, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnSpotHpUp, &style_gray, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnSpotDown, &style_gray, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnSpotUp, &style_gray, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnStepDown, &style_gray, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnStepUp, &style_gray, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnCount, &style_gray, LV_STATE_PRESSED);

    lv_obj_add_style(ui_btnEnergyUp, &style_gray, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnEnergyDown, &style_gray, LV_STATE_PRESSED);

    lv_obj_add_style(ui_btnReady, &style_red, LV_STATE_PRESSED);

    lv_obj_add_style(ui_btnVolume0, &style_pr2, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnVolume1, &style_pr2, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnVolume2, &style_pr2, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnVolume3, &style_pr2, LV_STATE_PRESSED);

    lv_obj_add_style(ui_btnStandbyTime0, &style_pr2, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnStandbyTime1, &style_pr2, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnStandbyTime2, &style_pr2, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnStandbyTime3, &style_pr2, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnStandbyTime4, &style_pr2, LV_STATE_PRESSED);

 
    disp_logo();
    disp_volume();
    disp_standby();
    // memory or preset
    disp_mode(config.mode);

    spot_mode(config.hp_mode);
    
    disp_update();

    lv_obj_add_event_cb(ui_ImageLogo, event_ImageLogo, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_ImageLogo1, event_ImageLogo1, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_ImageLogo2, event_ImageLogo2, LV_EVENT_ALL, NULL);
//    lv_obj_add_event_cb(ui_btnTitleSetup, event_btnTitleSetup, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnSetup, event_btnSetup, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_btnVolume0, event_btnVolume0, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnVolume1, event_btnVolume1, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnVolume2, event_btnVolume2, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnVolume3, event_btnVolume3, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_btnStandbyTime0, event_btnStandyby0, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnStandbyTime1, event_btnStandyby1, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnStandbyTime2, event_btnStandyby2, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnStandbyTime3, event_btnStandyby3, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnStandbyTime4, event_btnStandyby4, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_GuageArc, event_GuageArc, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnReady, event_btnReady, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_btnEnergyDown, event_btnEnergyDown, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnEnergyUp, event_btnEnergyUp, LV_EVENT_ALL, NULL);


    lv_obj_add_event_cb(ui_btnFreqDown, event_btnFreqDown, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnFreqUp, event_btnFreqUp, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_btnSpotHpDown, event_btnSpotHpDown, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnSpotHpUp, event_btnSpotHpUp, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_RollerHandPiece, event_RollerHandPiece, LV_EVENT_ALL, NULL); // todo
    lv_obj_add_event_cb(ui_btnSpotDown, event_btnSpotDown, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnSpotUp, event_btnSpotUp, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnSpotMode, event_btnSpotMode, LV_EVENT_ALL, NULL); // todo

    lv_obj_add_event_cb(ui_Image1064D0, event_Image1064D0, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Image1064D1, event_Image1064D1, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Image1064S0, event_Image1064S0, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Image1064S1, event_Image1064S1, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_ImageQuasi0, event_ImageQuasi0, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Image532R0, event_Image532R0, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_ImageMiin0, event_ImageMiin0, LV_EVENT_ALL, NULL);


    lv_obj_add_event_cb(ui_btnMemory0, event_memory_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnMemory1, event_memory_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnMemory2, event_memory_handler, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_Grp_Memory_Label, event_grp_memory_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Grp_Easy_Label, event_grp_preset_handler, LV_EVENT_ALL, NULL);


    lv_obj_add_event_cb(ui_btnCount, event_btnCount, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_txtEasyName, event_txtEasyName, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnStepDown, event_btnStepDown, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnStepUp, event_btnStepUp, LV_EVENT_ALL, NULL);

    lv_slider_set_value( ui_Slider_Aim, config.aiming, LV_ANIM_OFF);
    lv_obj_add_event_cb(ui_Slider_Aim, event_Slider_Aim, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_Grp_Count_Label, event_Grp_Count_Label, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Grp_Count_Label_Total, event_Grp_Count_Label_Total, LV_EVENT_ALL, NULL);

    pthread_create(&_ui_thread, NULL, ui_thread, NULL);

}

void close_ui_main(void)
{
    pthread_join(_ui_thread, (void**)NULL);	// 해당 스레드가 종료되길 기다린다.
}
