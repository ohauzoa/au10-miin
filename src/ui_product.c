#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "ui_product.h"
#include "config.h"
#include "gpio.h"
#include "functions.h"

 
static const char * const user_kb_map_num[] = {"1", "2", "3", LV_SYMBOL_BACKSPACE, "\n",
                                                  "4", "5", "6", LV_SYMBOL_REFRESH, "\n",
                                                  "7", "8", "9", LV_SYMBOL_CLOSE, "\n",
                                                  "+/-", "0", ".", LV_SYMBOL_OK, ""
                                                 };

static const lv_btnmatrix_ctrl_t user_kb_ctrl_num_map[] = {
    1, 1, 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
    1, 1, 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
    1, 1, 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
    1, 1, 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 2
};

static lv_obj_t ** ui_product_logo[3] = {
    &ui_lblLogoMode0, &ui_lblLogoMode1, &ui_lblLogoMode2
};

//static lv_event_cb_t keypad_cb;
static lv_obj_t * product_current_obj = NULL;
static int select_logo;


static void event_check_toggle(lv_obj_t * obj, int flag)
{
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        lv_obj_t * child = lv_obj_get_child(obj, i);
        if(flag) {
            lv_obj_add_state(child, LV_STATE_CHECKED);
        }
        else {
            lv_obj_clear_state(child, LV_STATE_CHECKED);
        }
    }
}

static void event_check_clear_all(void)
{
    event_check_toggle(ui_Grp_SerialNumber, false);
    event_check_toggle(ui_Grp_Date, false);
}

static void save_serial_parameter(void)
{
    char value[10];
    lv_snprintf(value, sizeof(value), "%d", config.logo);
    modify_config("config.ini", "config", "logo", value);

    modify_product("product.ini", "product", "serial laser", lv_label_get_text(ui_txtSnHead));
    modify_product("product.ini", "product", "serial power", lv_label_get_text(ui_txtSnPower));
    modify_product("product.ini", "product", "serial product", lv_label_get_text(ui_txtSnProduct));

    modify_product("product.ini", "product", "count total", lv_label_get_text(ui_txtCntShot));
    modify_product("product.ini", "product", "count power", lv_label_get_text(ui_txtCntPower));
    modify_product("product.ini", "product", "count lamp", lv_label_get_text(ui_txtCntLamp));
}

static void event_btnProductExit(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_ScreenCalibration, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenCalibration_screen_init);
        //_ui_screen_change(&ui_ScreenCalibration, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, &ui_ScreenCalibration_screen_init);
    }
}

static void event_btnProductApply(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        save_serial_parameter();
        _ui_screen_change(&ui_ScreenCalibration, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenCalibration_screen_init);
        //_ui_screen_change(&ui_ScreenCalibration, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, &ui_ScreenCalibration_screen_init);
    }
}
static void init_value(void)
{
    if((product_current_obj == ui_txtCntLamp) || (product_current_obj == ui_txtCntPower)|| (product_current_obj == ui_txtCntShot)){
        char buf[20] = "\0";
        int val = atoi(lv_label_get_text(product_current_obj));
        lv_snprintf(buf, sizeof(buf), "%d",  val);
        lv_textarea_set_text(ui_editProduct, buf);
    }
    else{
        lv_textarea_set_text(ui_editProduct, lv_label_get_text(product_current_obj));
    }
}

static void event_product_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        event_check_clear_all();
        product_current_obj = obj;

        lv_obj_t * parent = lv_obj_get_parent(obj);
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == product_current_obj) {
                lv_obj_add_state(child, LV_STATE_CHECKED);
                init_value();
//                lv_textarea_set_text(ui_editProduct, lv_label_get_text(product_current_obj));
            }
            else {
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }
    }
}

static void event_editProductDate(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        lv_keyboard_set_mode(ui_kbProduct, LV_KEYBOARD_MODE_TEXT_UPPER);
    }
}

static void update_value(void)
{
    if((product_current_obj == ui_txtCntLamp) || (product_current_obj == ui_txtCntPower)|| (product_current_obj == ui_txtCntShot)){
        lv_label_set_text_fmt(product_current_obj, "%08d", atoi(lv_textarea_get_text(ui_editProduct)));
    }
    else{
        lv_label_set_text(product_current_obj, lv_textarea_get_text(ui_editProduct));
    }
}
// keyboard event
static void event_product_kb_handler(lv_event_t * e)
{
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";
    int value;
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        int val = lv_keyboard_get_selected_btn(ui_kbProduct);
        if(val == 39) { // apply
//            event_check_clear_all();
            update_value();
//printf("kb39->%d\n", val);
        }
        else if(val == 22) {
            event_check_clear_all();
            update_value();
        }
        else if(val == 35) { // keyboard
//            close_modify();
        }
        else{
//            update_value();
        }
    }
}


void cal_datetime(void)
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
}

void event_btnReloadDate( lv_event_t * e) {
    char date[5][4];
    char *buf;
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_CLICKED) {
        config.flag_rtc_write = 1;
        //cal_datetime();


    }
}

static void disp_datetime(void)
{
	char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";
	int cnt = 0;
//	set_systemtime(2023, 11, 5, 11, 12, 50);
	get_systemtime(buf);
//	printf("%s\n", buf);

    lv_label_set_text(ui_txtDate, buf);

//	    char *ptr = strtok(buf, "/:- ");    //구분자는 콤마(,)입니다.
//    while (ptr != NULL)
//    {
//        printf("(%d)%s\n", cnt++, ptr);
//        ptr = strtok(NULL, "/:- ");
//    }

}

static lv_obj_t * mode_current_obj = NULL;

static void product_disp_logo(void)
{
    lv_obj_clear_state(ui_lblLogoMode0, LV_STATE_CHECKED);
    lv_obj_clear_state(ui_lblLogoMode1, LV_STATE_CHECKED);
    lv_obj_clear_state(ui_lblLogoMode2, LV_STATE_CHECKED);

    switch(config.logo)
    {
        case LOGO_MiiN:
            lv_obj_add_state(ui_lblLogoMode0, LV_STATE_CHECKED);
            lv_label_set_text(ui_lblSNHead, "MIQN-");
            lv_label_set_text(ui_lblSNPower, "MGPS-");
            lv_label_set_text(ui_lblSNProduct, "LT-MIQN-");
            modify_config("config.ini", "config", "logo", "0");
            break;
        case LOGO_BRIGHTONE:
            lv_obj_add_state(ui_lblLogoMode1, LV_STATE_CHECKED);
            lv_label_set_text(ui_lblSNHead, "BRQN-");
            lv_label_set_text(ui_lblSNPower, "MGPS-");
            lv_label_set_text(ui_lblSNProduct, "LT-BRQN-");
            modify_config("config.ini", "config", "logo", "1");
            break;
        case LOGO_PEAKLIGHT:
            lv_obj_add_state(ui_lblLogoMode2, LV_STATE_CHECKED);
            lv_label_set_text(ui_lblSNHead, "PLQN-");
            lv_label_set_text(ui_lblSNPower, "MGPS-");
            lv_label_set_text(ui_lblSNProduct, "LT-PLQN-");
            modify_config("config.ini", "config", "logo", "2");
            break;
    }

}

static void event_logomode_handler(lv_event_t * e)
{
	char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        mode_current_obj = obj;

        lv_obj_t * parent = lv_obj_get_parent(obj);
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == mode_current_obj) {
                config.logo = i; // 오브젝트 등록순서에 따른다
                //lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else {
                //lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }

        product_disp_logo();
    }
}



void event_product_init(void)
{
    static lv_style_t style_pr;
    lv_style_init(&style_pr);

    /*Add a large outline when pressed*/
    lv_style_set_outline_width(&style_pr, 30);
    lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);
    lv_style_set_outline_color(&style_pr, lv_palette_darken(LV_PALETTE_RED, 2));

    lv_style_set_translate_y(&style_pr, 5);
    lv_style_set_shadow_ofs_y(&style_pr, 3);

    /*Add a transition to the outline*/
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0};
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, TRANSITION_TIME, 0, NULL);
    lv_style_set_transition(&style_pr, &trans);

    // 배경퍼짐
    //lv_obj_add_style(ui_btnReloadDate, &style_pr, LV_STATE_PRESSED);
    //lv_obj_add_style(ui_btnWifiScan, &style_pr, LV_STATE_PRESSED);

    // 버튼에 기호넣기
    lv_label_set_text(ui_lblReloadDate, LV_SYMBOL_REFRESH);
    lv_label_set_text(ui_lblWifiScan, LV_SYMBOL_REFRESH);
    lv_label_set_text(ui_lblCK3, LV_SYMBOL_WIFI);

    _ui_keyboard_set_target(ui_kbProduct,  ui_editProduct);


    lv_label_set_text_fmt(ui_txtSnHead, "%08d", product.seriallaser);
    lv_label_set_text_fmt(ui_txtSnPower, "%08d", product.serialpower);
    lv_label_set_text_fmt(ui_txtSnProduct, "%08d", product.serialproduct);


#if 1
    lv_label_set_text_fmt(ui_txtCntLamp, "%08d", product.lamp);
    lv_label_set_text_fmt(ui_txtCntPower, "%08d", product.power);
    lv_label_set_text_fmt(ui_txtCntShot, "%08d", product.total);
#else
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";

    int2comma(buf, product.total, COUNT_WIDTH);
    lv_label_set_text(ui_txtCntShot , buf);
    int2comma(buf, product.power, COUNT_WIDTH);
    lv_label_set_text(ui_txtCntPower, buf);
    int2comma(buf, product.lamp, COUNT_WIDTH);
    lv_label_set_text(ui_txtCntLamp, buf);
#endif

    
    disp_datetime();

//    lv_obj_set_style_text_align(ui_txtCntLamp, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
//    lv_obj_set_style_text_align(ui_txtCntPower, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
//    lv_obj_set_style_text_align(ui_txtCntShot, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_lblLogoMode0, event_logomode_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_lblLogoMode1, event_logomode_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_lblLogoMode2, event_logomode_handler, LV_EVENT_ALL, NULL);


    lv_obj_add_event_cb(ui_txtSnHead, event_product_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSnPower, event_product_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSnProduct, event_product_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtCntLamp, event_product_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtCntPower, event_product_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtCntShot, event_product_handler, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_txtDate, event_product_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtSSID, event_product_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtPSK, event_product_handler, LV_EVENT_ALL, NULL);

//    lv_obj_add_event_cb(ui_editProduct, ui_event_editProduct, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_kbProduct, event_product_kb_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnReloadDate, event_btnReloadDate, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnWifiScan, ui_event_btnWifiScan, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnProductExit, event_btnProductExit, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnProductApply, event_btnProductApply, LV_EVENT_ALL, NULL);


    lv_textarea_set_text(ui_editProduct, "");
    lv_keyboard_set_mode(ui_kbProduct, LV_KEYBOARD_MODE_SPECIAL);

    product_disp_logo();
//    event_check_toggle(ui_Grp_Logo_Mode, config.logo);

}
