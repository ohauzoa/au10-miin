#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "lv_ui/ui.h"
#include "ui_preset.h"
#include "config.h"
#include "gpio.h"

#define LV_KB_BTN(width) LV_BTNMATRIX_CTRL_POPOVER | width

#define PRESET_NUM 24
#define TAG_NUM 16

static const char * const kb_map_spec[] = {" ", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", LV_SYMBOL_BACKSPACE, "\n",
                                                   "abc", "+", "-", "/", "*", "=", "%", "!", "?", "#", LV_SYMBOL_NEW_LINE, "\n",
                                                   "\\",  "@", "$", "(", ")", "{", "}", "[", "]", ":", "\"", "'", "\n",
                                                   LV_SYMBOL_KEYBOARD, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                  };

static const lv_btnmatrix_ctrl_t kb_ctrl_spec_map[] = {
    LV_KB_BTN(5), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_BTNMATRIX_CTRL_CHECKED | 7,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 6, LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_BTNMATRIX_CTRL_CHECKED | 7,
    LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1),
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, LV_BTNMATRIX_CTRL_CHECKED | 2, 6, LV_BTNMATRIX_CTRL_CHECKED | 2, LV_KEYBOARD_CTRL_BTN_FLAGS | 2
};

static const char * const label_wave[5] = { "1064 S", "1064 D", "MiiN", "532", "Quasi" };
static const char * const label_handpiece[4] = { "z 1064", "z 532", "MLA", "DOE" };
// zoom인식모드 s,d,q,m,532
// mla인식모드 s,d,q,m,532
// doe인식모드 s,d,q,m
static const char * const label_spot[4][9] = { // zoom 1064, zoom 532, mla, doe
{ "2 mm", "3 mm", "4 mm", "5 mm", "6 mm", "7 mm", "8 mm", "9 mm", "10 mm" },    // ZOOM 1064
{ "1 mm", "2 mm", "3 mm", "3.5 mm", "4 mm", "5 mm", "6 mm", "6.5 mm", "7 mm" }, // ZOOM 532
{ "4 mm", "7 mm", "10 mm", "", "", "", "", "", "" },                            // MLA
{ "5 mm", "", "", "", "", "", "", "", "" }                                      // DOE
};

static const char * const dd_spot[4] = { // zoom 532, zoom 1064, mla, doe
"2 mm\n3 mm\n4 mm\n5 mm\n6 mm\n7 mm\n8 mm\n9 mm\n10 mm",    // ZOOM 1064
"1 mm\n2 mm\n3 mm\n3.5 mm\n4 mm\n5 mm\n6 mm\n6.5 mm\n7 mm",    // ZOOM 532
"4 mm\n7 mm\n10 mm",    // MLA
"5 mm",    // DOE
};

//static const int dd_freq[] = { 1, 2, 3, 4, 5, "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20" };

static lv_obj_t ** ui_preset_name[24] = {
    &ui_lblPreset0, &ui_lblPreset1, &ui_lblPreset2, &ui_lblPreset3, &ui_lblPreset4, &ui_lblPreset5,
    &ui_lblPreset6, &ui_lblPreset7, &ui_lblPreset8, &ui_lblPreset9, &ui_lblPreset10, &ui_lblPreset11,
    &ui_lblPreset12, &ui_lblPreset13, &ui_lblPreset14, &ui_lblPreset15, &ui_lblPreset16, &ui_lblPreset17,
    &ui_lblPreset18, &ui_lblPreset19, &ui_lblPreset20, &ui_lblPreset21, &ui_lblPreset22, &ui_lblPreset23
};

static lv_obj_t ** ui_preset_mark[16] = {
    &ui_txtMark0, &ui_txtMark1, &ui_txtMark2, &ui_txtMark3, &ui_txtMark4, &ui_txtMark5, &ui_txtMark6, &ui_txtMark7,
    &ui_txtMark8, &ui_txtMark9, &ui_txtMark10, &ui_txtMark11, &ui_txtMark12, &ui_txtMark13, &ui_txtMark14, &ui_txtMark15
};

static lv_obj_t ** ui_preset_wave[16] = {
    &ui_txtWave0, &ui_txtWave1, &ui_txtWave2, &ui_txtWave3, &ui_txtWave4, &ui_txtWave5, &ui_txtWave6, &ui_txtWave7,
    &ui_txtWave8, &ui_txtWave9, &ui_txtWave10, &ui_txtWave11, &ui_txtWave12, &ui_txtWave13, &ui_txtWave14, &ui_txtWave15
};

static lv_obj_t ** ui_preset_handpiece[16] = {
    &ui_txtHandPiece0, &ui_txtHandPiece1, &ui_txtHandPiece2, &ui_txtHandPiece3, &ui_txtHandPiece4, &ui_txtHandPiece5, &ui_txtHandPiece6, &ui_txtHandPiece7,
    &ui_txtHandPiece8, &ui_txtHandPiece9, &ui_txtHandPiece10, &ui_txtHandPiece11, &ui_txtHandPiece12, &ui_txtHandPiece13, &ui_txtHandPiece14, &ui_txtHandPiece15
};

static lv_obj_t ** ui_preset_spot[16] = {
    &ui_txtSpot0, &ui_txtSpot1, &ui_txtSpot2, &ui_txtSpot3, &ui_txtSpot4, &ui_txtSpot5, &ui_txtSpot6, &ui_txtSpot7,
    &ui_txtSpot8, &ui_txtSpot9, &ui_txtSpot10, &ui_txtSpot11, &ui_txtSpot12, &ui_txtSpot13, &ui_txtSpot14, &ui_txtSpot15
};

static lv_obj_t ** ui_preset_fluence[16] = {
    &ui_txtFlue0, &ui_txtFlue1, &ui_txtFlue2, &ui_txtFlue3, &ui_txtFlue4, &ui_txtFlue5, &ui_txtFlue6, &ui_txtFlue7,
    &ui_txtFlue8, &ui_txtFlue9, &ui_txtFlue10, &ui_txtFlue11, &ui_txtFlue12, &ui_txtFlue13, &ui_txtFlue14, &ui_txtFlue15
};

static lv_obj_t ** ui_preset_energy[16] = {
    &ui_txtEner0, &ui_txtEner1, &ui_txtEner2, &ui_txtEner3, &ui_txtEner4, &ui_txtEner5, &ui_txtEner6, &ui_txtEner7,
    &ui_txtEner8, &ui_txtEner9, &ui_txtEner10, &ui_txtEner11, &ui_txtEner12, &ui_txtEner13, &ui_txtEner14, &ui_txtEner15
};

static lv_obj_t ** ui_preset_frequency[16] = {
    &ui_txtPulse0, &ui_txtPulse1, &ui_txtPulse2, &ui_txtPulse3, &ui_txtPulse4, &ui_txtPulse5, &ui_txtPulse6, &ui_txtPulse7,
    &ui_txtPulse8, &ui_txtPulse9, &ui_txtPulse10, &ui_txtPulse11, &ui_txtPulse12, &ui_txtPulse13, &ui_txtPulse14, &ui_txtPulse15
};

static lv_obj_t *resion_current_obj = NULL;
static lv_obj_t *preset_current_obj = NULL;
static lv_obj_t *modify_current_obj = NULL;

static int select_resion;
static int select_preset;
static int preset_open = false;

static lv_event_cb_t modify_kb_cb;

static const double const size_spot[4][9] = { // zoom 532, zoom 1064, mla, doe
{3.141592, 7.068582, 12.566368, 19.63495, 28.274328, 38.484502, 50.265472, 63.617238, 78.5398},    // ZOOM 1064
{0.785398, 3.141592, 7.068582, 9.621126, 12.566368, 19.63495, 28.274328, 33.18307, 38.484502},    // ZOOM 532
{ 12.566368, 38.484502, 78.5398, 0, 0, 0, 0, 0, 0 },    // MLA
{ 19.63495, 0, 0, 0, 0, 0, 0, 0, 0 },    // DOE
};
#define    PI    3.141597

static int calc_fluence(int energy, int handpiece, int spot)
{
    int fluence = (int)(energy / size_spot[handpiece][spot] * 10);
    return fluence;
}

static void event_modify_keypad_handler(struct _lv_obj_t *obj, lv_event_cb_t new_cb)
{
    lv_obj_remove_event_cb(obj, modify_kb_cb);
    modify_kb_cb = new_cb;
    lv_obj_add_event_cb(obj, modify_kb_cb, LV_EVENT_ALL, NULL);
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

static void event_check_clear(lv_obj_t * obj, int id)
{
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        lv_obj_t * child = lv_obj_get_child(obj, i);
        if(id == i) {
            lv_obj_add_state(child, LV_STATE_CHECKED);
        }
        else {
            lv_obj_clear_state(child, LV_STATE_CHECKED);
        }
    }
}

static void open_modify(void)
{
    if(preset_open == 0)
    {
        PresetOpen_Animation(ui_Grp_Preset, 0);
        lv_obj_set_height(ui_Grp_Preset_Tag, 180);
        lv_obj_set_y(ui_Grp_Preset_Index, -170);
        preset_open = true;
    }
}
static void close_modify(void)
{
    if(preset_open == 1)
    {
        PresetClose_Animation(ui_Grp_Preset, 0);
        lv_obj_set_height(ui_Grp_Preset_Tag, 270);
        lv_obj_set_y(ui_Grp_Preset_Index, -260);
        preset_open = false;
    }

}
static void clear_tag(int no)
{
    for(int i = 0; i < TAG_NUM ; i++)
       lv_label_set_text(*ui_preset_mark[i], "");

    for(int i = 0; i < TAG_NUM ; i++)
       lv_label_set_text(*ui_preset_wave[i], "");

    for(int i = 0; i < TAG_NUM ; i++)
       lv_label_set_text(*ui_preset_handpiece[i], "");

    for(int i = 0; i < TAG_NUM ; i++)
       lv_label_set_text(*ui_preset_spot[i], "");
//        printf("spot-> %s\n", label_spot[preset[lv].dat[i].handpiece][preset[lv].dat[i].spot]);

    for(int i = 0; i < TAG_NUM ; i++)
       lv_label_set_text(*ui_preset_fluence[i], "");

    for(int i = 0; i < TAG_NUM ; i++)
       lv_label_set_text(*ui_preset_energy[i], "");

    for(int i = 0; i < TAG_NUM ; i++)
       lv_label_set_text(*ui_preset_frequency[i], "");
}

static void populate_name(void)
{
    for(int i = 0; i < PRESET_NUM ; i++)
        lv_label_set_text_fmt(*ui_preset_name[i], "%s", preset[i].name);
}

static void populate_tag(int no)
{
    int value;
    clear_tag(no);
    lv_obj_scroll_to(ui_Grp_Preset_Tag, 0, 0, LV_ANIM_ON);
    event_check_clear(ui_Grp_Preset_Tag, 0xff);

    for(int i = 0; i < preset[no].count ; i++)
       lv_label_set_text(*ui_preset_mark[i], preset[no].dat[i].mark);

    for(int i = 0; i < preset[no].count ; i++)
       lv_label_set_text(*ui_preset_wave[i], label_wave[preset[no].dat[i].wavelength]);

    for(int i = 0; i < preset[no].count ; i++)
       lv_label_set_text(*ui_preset_handpiece[i], label_handpiece[preset[no].dat[i].handpiece]);

    for(int i = 0; i < preset[no].count ; i++)
       lv_label_set_text(*ui_preset_spot[i], label_spot[preset[no].dat[i].handpiece][preset[no].dat[i].spot]);
//        printf("spot-> %s\n", label_spot[preset[lv].dat[i].handpiece][preset[lv].dat[i].spot]);

    for(int i = 0; i < preset[no].count ; i++)
    {
        value = calc_fluence(preset[no].dat[i].energy, preset[no].dat[i].handpiece, preset[no].dat[i].spot);
        lv_label_set_text_fmt(*ui_preset_fluence[i], "%d.%02d", value/100, value%100);
    }
//calc_fluence(atoi(lv_label_get_text(ui_txtModifyEnergy)), preset[no].dat[i].handpiece, preset[no].dat[i].spot);
//       lv_label_set_text_fmt(*ui_preset_fluence[i], "%d", calc_fluence(preset[no].dat[i].energy, preset[no].dat[i].handpiece, preset[no].dat[i].spot));
//       lv_label_set_text_fmt(*ui_preset_fluence[i], "%d", preset[no].dat[i].fluence);

    for(int i = 0; i < preset[no].count ; i++)
       lv_label_set_text_fmt(*ui_preset_energy[i], "%d mJ", preset[no].dat[i].energy);

    for(int i = 0; i < preset[no].count ; i++)
       lv_label_set_text_fmt(*ui_preset_frequency[i], "%d Hz", preset[no].dat[i].frequency + 1);
}

static void modify_tag(void)
{
    int value, temp;

    event_check_clear(ui_Grp_Preset_Tag, 0xff);

    lv_snprintf(preset[select_resion].name, sizeof(preset[select_resion].name), "%s", lv_textarea_get_text(ui_editModifyName));
    lv_label_set_text_fmt(*ui_preset_name[select_resion], "%s", preset[select_resion].name);

    lv_snprintf(preset[select_resion].dat[select_preset/8].mark, sizeof(preset[select_resion].dat[select_preset/8].mark), "%s", lv_textarea_get_text(ui_editModifyMark));
    lv_label_set_text_fmt(*ui_preset_mark[select_preset/8], "%s", preset[select_resion].dat[select_preset/8].mark);

    preset[select_resion].dat[select_preset/8].wavelength = lv_dropdown_get_selected(ui_ddModifyWave);
    lv_label_set_text(*ui_preset_wave[select_preset/8], label_wave[preset[select_resion].dat[select_preset/8].wavelength]);

    preset[select_resion].dat[select_preset/8].handpiece = lv_dropdown_get_selected(ui_ddModifyHP);
    lv_label_set_text(*ui_preset_handpiece[select_preset/8], label_handpiece[preset[select_resion].dat[select_preset/8].handpiece]);

    preset[select_resion].dat[select_preset/8].spot = lv_dropdown_get_selected(ui_ddModifySpot);
    lv_label_set_text(*ui_preset_spot[select_preset/8], label_spot[preset[select_resion].dat[select_preset/8].handpiece][preset[select_resion].dat[select_preset/8].spot]);

//    value = calc_fluence(preset[select_resion].dat[select_preset/8].energy, preset[select_resion].dat[select_preset/8].handpiece, preset[select_resion].dat[select_preset/8].spot);
    value = calc_fluence(atoi(lv_textarea_get_text(ui_editModifyEnergy)), lv_dropdown_get_selected(ui_ddModifyHP), lv_dropdown_get_selected(ui_ddModifySpot));
    lv_label_set_text_fmt(*ui_preset_fluence[select_preset/8], "%d.%02d", value/100, value%100);

    preset[select_resion].dat[select_preset/8].energy = atoi(lv_textarea_get_text(ui_editModifyEnergy));
    lv_label_set_text_fmt(*ui_preset_energy[select_preset/8], "%d mJ", preset[select_resion].dat[select_preset/8].energy);

    preset[select_resion].dat[select_preset/8].frequency = lv_dropdown_get_selected(ui_ddModifyFrequency);
    lv_label_set_text_fmt(*ui_preset_frequency[select_preset/8], "%d Hz", preset[select_resion].dat[select_preset/8].frequency + 1);    // 표시할때는 1 더해준다.

    printf("modify-> %d, %d, %d\n", select_resion, preset[select_resion].count, select_preset/8);
    // preset[select_resion].count는 1부터 시작한다.
    if(preset[select_resion].count < (select_preset/8) + 1) preset[select_resion].count = (select_preset/8) + 1;
}
extern void disp_update(void);

static void event_resion_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
//    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";
    if(code == LV_EVENT_PRESSED) system("tinyplayer tts/Beep_Once.mp3 &");

    if(code == LV_EVENT_LONG_PRESSED) {
        system("tinyplayer tts/DingSound.mp3 &");
        config.mode = select_resion;
        _ui_screen_change(&ui_ScreenMain, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenMain_screen_init);
//        ds1307_write_mode();
//        lv_snprintf(buf, sizeof(buf), "%d", select_resion);
//        modify_config("config.ini", "config", "temp", buf);
//        modify_config("config.ini", "config", "mode", buf);
    }
    else if(code == LV_EVENT_CLICKED) {
        resion_current_obj = obj;

        lv_obj_t * parent = lv_obj_get_parent(obj);
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == resion_current_obj) {
                select_resion = i;
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else {
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }
//printf("lesion->%d\n", select_resion);
        config.mode = select_resion;
        disp_update();
        populate_tag(select_resion);
//        ds1307_write_mode();
//        lv_snprintf(buf, sizeof(buf), "%d", select_resion);
//        modify_config("config.ini", "config", "temp", buf);
//        modify_config("config.ini", "config", "mode", buf);
    }
}

// keyboard event
static void event_modify_kb_handler(lv_event_t * e)
{
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";
    int value;
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) system("tinyplayer tts/S_HW_Touch_Calm.mp3 &");
    if(event_code == LV_EVENT_CLICKED) {
        int val = lv_keyboard_get_selected_btn(ui_kbModify);
        if(val == 39) { // apply
//printf("kb39->%d\n", val);
            int value = calc_fluence(atoi(lv_textarea_get_text(ui_editModifyEnergy)), lv_dropdown_get_selected(ui_ddModifyHP), lv_dropdown_get_selected(ui_ddModifySpot));
            lv_label_set_text_fmt(ui_txtModifyFluence, "%d.%02d", value/100, value%100);
            modify_tag();
            make_preset_modify(select_resion);
        }
        else if(val == 22) {
            int value = calc_fluence(atoi(lv_textarea_get_text(ui_editModifyEnergy)), lv_dropdown_get_selected(ui_ddModifyHP), lv_dropdown_get_selected(ui_ddModifySpot));
            lv_label_set_text_fmt(ui_txtModifyFluence, "%d.%02d", value/100, value%100);
        }
        else if(val == 35) { // keyboard
            close_modify();
        }
    }
}

// GRP Preset TAG
static void event_preset_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";
    if(code == LV_EVENT_PRESSED) system("tinyplayer tts/Beep_Once.mp3 &");
    
    if(code == LV_EVENT_CLICKED) {
        preset_current_obj = obj;
        lv_obj_t * parent = lv_obj_get_parent(obj);
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == preset_current_obj) {
                select_preset = i;
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else {
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }
    }
    else if(code == LV_EVENT_LONG_PRESSED) {
        system("tinyplayer tts/Milky_Way.mp3 &");
        preset_current_obj = obj;
//printf("preset->%d, (%d)\n", select_preset, strlen(preset[select_resion].dat[select_preset/8].mark));
        open_modify();

        lv_keyboard_set_textarea(ui_kbModify, NULL);
        lv_label_set_text_fmt(ui_lblModifyNo, "%d", (select_preset/8) + 1);

        lv_textarea_set_text(ui_editModifyName, preset[select_resion].name);
        lv_textarea_set_text(ui_editModifyMark, preset[select_resion].dat[select_preset/8].mark);
        lv_dropdown_set_selected(ui_ddModifyWave, preset[select_resion].dat[select_preset/8].wavelength);
        lv_dropdown_set_selected(ui_ddModifyHP, preset[select_resion].dat[select_preset/8].handpiece);

// 핸드피스에따라서 스팟변경
        lv_dropdown_set_options(ui_ddModifySpot, dd_spot[preset[select_resion].dat[select_preset/8].handpiece]);
        lv_dropdown_set_selected(ui_ddModifySpot, preset[select_resion].dat[select_preset/8].spot);

        lv_snprintf(buf, sizeof(buf), "%d", preset[select_resion].dat[select_preset/8].energy);
        lv_textarea_set_text(ui_editModifyEnergy, buf);

        lv_dropdown_set_selected(ui_ddModifyFrequency, preset[select_resion].dat[select_preset/8].frequency);
        lv_label_set_text_fmt(ui_txtModifyFluence, "%d", preset[select_resion].dat[select_preset/8].fluence);
    }
}


static void event_editModifyMark(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_keyboard_set_target(ui_kbModify,  ui_editModifyMark);
        lv_keyboard_set_mode(ui_kbModify, LV_KEYBOARD_MODE_TEXT_UPPER);
    }
}
static void event_editModifyName(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_keyboard_set_target(ui_kbModify,  ui_editModifyName);
        lv_keyboard_set_mode(ui_kbModify, LV_KEYBOARD_MODE_TEXT_UPPER);
    }
}
static void event_editModifyEnergy(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_keyboard_set_target(ui_kbModify,  ui_editModifyEnergy);
        lv_keyboard_set_mode(ui_kbModify, LV_KEYBOARD_MODE_SPECIAL);
    }
}

static void event_ddModifyWave_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        LV_LOG_USER("Option: %s", buf);
    }
}

static void event_ddModifyHP_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        lv_dropdown_set_options(ui_ddModifySpot, dd_spot[lv_dropdown_get_selected(ui_ddModifyHP)]);
        LV_LOG_USER("Option: %s", buf);
    }
}

static void event_ddModifySpot_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
            int value = calc_fluence(atoi(lv_textarea_get_text(ui_editModifyEnergy)), lv_dropdown_get_selected(ui_ddModifyHP), lv_dropdown_get_selected(ui_ddModifySpot));
            lv_label_set_text_fmt(ui_txtModifyFluence, "%d.%02d", value/100, value%100);
    }
}

static void event_ddModifyFrequency_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        LV_LOG_USER("Option: %s", buf);
    }
}

void event_preset_init(void)
{
    populate_name();
    clear_tag(0);
    lv_keyboard_set_map(ui_kbModify  , LV_KEYBOARD_MODE_SPECIAL, (void*)&kb_map_spec, (void*)&kb_ctrl_spec_map);

    lv_obj_add_event_cb(ui_btnPreset0, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset1, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset2, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset3, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset4, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset5, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset6, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset7, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset8, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset9, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset10, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset11, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset12, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset13, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset14, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset15, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset16, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset17, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset18, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset19, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset20, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset21, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset22, event_resion_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPreset23, event_resion_handler, LV_EVENT_ALL, NULL);


    lv_obj_add_event_cb(ui_txtMark0, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtWave0, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtHandPiece0, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSpot0, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtEner0, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPulse0, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMark1, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtWave1, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtHandPiece1, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSpot1, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtEner1, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPulse1, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMark2, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtWave2, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtHandPiece2, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSpot2, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtEner2, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPulse2, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMark3, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtWave3, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtHandPiece3, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSpot3, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtEner3, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPulse3, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMark4, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtWave4, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtHandPiece4, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSpot4, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtEner4, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPulse4, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMark5, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtWave5, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtHandPiece5, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSpot5, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtEner5, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPulse5, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMark6, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtWave6, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtHandPiece6, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSpot6, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtEner6, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPulse6, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMark7, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtWave7, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtHandPiece7, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSpot7, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtEner7, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPulse7, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMark8, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtWave8, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtHandPiece8, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSpot8, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtEner8, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPulse8, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMark9, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtWave9, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtHandPiece9, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSpot9, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtEner9, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPulse9, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMark10, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtWave10, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtHandPiece10, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSpot10, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtEner10, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPulse10, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMark11, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtWave11, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtHandPiece11, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSpot11, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtEner11, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPulse11, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMark12, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtWave12, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtHandPiece12, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSpot12, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtEner12, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPulse12, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMark13, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtWave13, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtHandPiece13, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSpot13, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtEner13, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPulse13, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMark14, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtWave14, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtHandPiece14, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSpot14, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtEner14, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPulse14, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMark15, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtWave15, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtHandPiece15, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSpot15, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtEner15, event_preset_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPulse15, event_preset_handler, LV_EVENT_ALL, NULL);

///////////////////////////////////////////////////////////////////////////////////////////
    lv_obj_add_event_cb(ui_editModifyMark, event_editModifyMark, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_editModifyName, event_editModifyName, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_editModifyEnergy, event_editModifyEnergy, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_ddModifyWave, event_ddModifyWave_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_ddModifyHP, event_ddModifyHP_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_ddModifySpot, event_ddModifySpot_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_ddModifyFrequency, event_ddModifyFrequency_handler, LV_EVENT_ALL, NULL);

    lv_keyboard_set_textarea(ui_kbModify, NULL);

    lv_obj_add_event_cb(ui_kbModify, event_modify_kb_handler, LV_EVENT_ALL, NULL);

    if(config.mode != config.temp){
        populate_tag(config.temp);
        event_check_clear(ui_Grp_Preset_Name, config.temp);
    }
    else{
        populate_tag(config.mode);
        event_check_clear(ui_Grp_Preset_Name, config.mode);
    }
}
