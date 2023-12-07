

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "config.h"
#include "ui_msg.h"

#include "../lvgl/lvgl.h"
#include "lv_ui/ui.h"
#include "lv_drivers/display/sunxifb.h"


#define MEMZERO(x) memset(&(x), 0, sizeof(x))   /* 변수 초기화를 위한 매크로 */
#define NUMCOLOR 4


lv_obj_t * msg_box;

static void msgbox_event_callback(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * msgbox = lv_event_get_current_target(e);

    if((code == LV_EVENT_VALUE_CHANGED) && (msgbox != NULL))
    {
        const char * txt = lv_msgbox_get_active_btn_text(msgbox);
        if(strcmp(txt, "Close") == 0)
        {
            lv_msgbox_close(msgbox);
        }
    }
}
static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * msgbox = lv_event_get_current_target(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
            lv_msgbox_close(msg_box);
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}

static const char * const msg_str[5] = {
    "The setting value of the handpiece or spot size is incorrect.",
    "1064 D", "MiiN", "532", "Quasi" };

static const char * const str_hp[4] = {
    "ZOOM1064", "ZOOM532", "MLA", "DOE" };

static const char * const str_spot[4][9] = { // zoom 1064S, zoom 1064D, zoom 532, mla, doe
{"2", "3", "4", "5", "6", "7", "8", "9", "10"},    // ZOOM 1064D
{"1", "2", "3", "3.5", "4", "5", "6", "6.5", "7" },    // ZOOM 532
{"4", "7", "10", "", "", "", "", "", ""},    // MLA
{"5", "", "", "", "", "", "", "", ""},    // DOE
};

void lv_msgbox_show(int type)
{
    static const char * btns[] = {"Close",""};
    static const char * title[] = {"Warning", "Error"};
    char buf[256] = "\0";

    static lv_style_t msgbox_style;
    lv_style_init(&msgbox_style);
    lv_style_set_radius(&msgbox_style, 0);
    lv_obj_t * btnClose;
    lv_obj_t * lblClose;


    lv_snprintf(buf, sizeof(buf), "%s\nSetup H/P [%s] %s mm", msg_str[type], str_hp[config.handpiece], str_spot[config.handpiece][config.spotsize]);


    msg_box = lv_msgbox_create(NULL, title[type], buf, btns, false);

    if(msg_box != NULL)
    {
        lv_obj_add_event_cb(msg_box, msgbox_event_callback, LV_EVENT_ALL, NULL);
        lv_obj_align(msg_box, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_style(msg_box, &msgbox_style, 0);
        lv_obj_set_style_text_font(msg_box, &ui_font_Medium18, 0);
        lv_obj_set_style_radius(msg_box, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_width(msg_box, 400);
        lv_obj_set_height(msg_box, 250);

        // title
        //lv_obj_set_y(lv_msgbox_get_title(msg_box), -14);
        lv_obj_set_height(lv_msgbox_get_title(msg_box), 40);
        lv_obj_set_style_text_color(lv_msgbox_get_title(msg_box), lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(lv_msgbox_get_title(msg_box), 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(lv_msgbox_get_title(msg_box), LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(lv_msgbox_get_title(msg_box), &ui_font_Medium18, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(lv_msgbox_get_title(msg_box), 3, LV_PART_MAIN | LV_STATE_DEFAULT);
        if(type) lv_obj_set_style_bg_color(lv_msgbox_get_title(msg_box), lv_color_hex(0xf82020), LV_PART_MAIN | LV_STATE_DEFAULT);
        else lv_obj_set_style_bg_color(lv_msgbox_get_title(msg_box), lv_color_hex(0xf8df23), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(lv_msgbox_get_title(msg_box), 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        if(type) lv_obj_set_style_bg_grad_color(lv_msgbox_get_title(msg_box), lv_color_hex(0x551515), LV_PART_MAIN | LV_STATE_DEFAULT);
        else lv_obj_set_style_bg_grad_color(lv_msgbox_get_title(msg_box), lv_color_hex(0x9a7a15), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_main_stop(lv_msgbox_get_title(msg_box), 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_stop(lv_msgbox_get_title(msg_box), 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_dir(lv_msgbox_get_title(msg_box), LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(lv_msgbox_get_title(msg_box), 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_color(lv_msgbox_get_title(msg_box), lv_color_hex(0xB1B1B1), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_opa(lv_msgbox_get_title(msg_box), 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(lv_msgbox_get_title(msg_box), 4, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_spread(lv_msgbox_get_title(msg_box), 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_ofs_x(lv_msgbox_get_title(msg_box), 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_ofs_y(lv_msgbox_get_title(msg_box), 6, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_pad_left(lv_msgbox_get_title(msg_box), 10, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_right(lv_msgbox_get_title(msg_box), 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(lv_msgbox_get_title(msg_box), 7, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_bottom(lv_msgbox_get_title(msg_box), 0, LV_PART_MAIN | LV_STATE_DEFAULT);

        // text line 1
        lv_obj_set_height(lv_msgbox_get_text(msg_box), 100);
        lv_obj_set_style_text_color(lv_msgbox_get_text(msg_box), lv_color_hex(0x0E0A6B), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(lv_msgbox_get_text(msg_box), 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(lv_msgbox_get_text(msg_box), LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(lv_msgbox_get_text(msg_box), &ui_font_Medium18, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(lv_msgbox_get_text(msg_box), 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(lv_msgbox_get_text(msg_box), lv_color_hex(0xDDDDDD), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(lv_msgbox_get_text(msg_box), 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_color(lv_msgbox_get_text(msg_box), lv_color_hex(0xEEEEEE), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_main_stop(lv_msgbox_get_text(msg_box), 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_stop(lv_msgbox_get_text(msg_box), 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_dir(lv_msgbox_get_text(msg_box), LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(lv_msgbox_get_text(msg_box), 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_color(lv_msgbox_get_text(msg_box), lv_color_hex(0xB1B1B1), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_opa(lv_msgbox_get_text(msg_box), 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(lv_msgbox_get_text(msg_box), 4, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_spread(lv_msgbox_get_text(msg_box), 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_ofs_x(lv_msgbox_get_text(msg_box), 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_ofs_y(lv_msgbox_get_text(msg_box), 6, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_pad_left(lv_msgbox_get_text(msg_box), 10, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_right(lv_msgbox_get_text(msg_box), 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(lv_msgbox_get_text(msg_box), 9, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_bottom(lv_msgbox_get_text(msg_box), 0, LV_PART_MAIN | LV_STATE_DEFAULT);


        // buttons
        //lv_obj_align(lv_msgbox_get_btns(msg_box), LV_ALIGN_BOTTOM_RIGHT, 100, 100);
        //lv_obj_set_x(lv_msgbox_get_btns(msg_box), 100);
        lv_obj_set_style_radius(lv_msgbox_get_btns(msg_box), 1, LV_PART_MAIN | LV_STATE_DEFAULT);
        //lv_obj_set_y(lv_msgbox_get_btns(msg_box), 14);
        lv_obj_set_height(lv_msgbox_get_btns(msg_box), 45);
        lv_obj_set_width(lv_msgbox_get_btns(msg_box), 180);
lv_obj_add_flag( lv_msgbox_get_btns(msg_box), LV_OBJ_FLAG_HIDDEN );   /// Flags

        btnClose = lv_btn_create(msg_box);
        lv_obj_set_width( btnClose, 180);
        lv_obj_set_height( btnClose, 40);
        //lv_obj_set_x( btnClose, 200 );
        //lv_obj_set_y( btnClose, 50 );
        lv_obj_set_style_radius(btnClose, 3, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(btnClose, lv_color_hex(0xEAE9E9), LV_PART_MAIN | LV_STATE_DEFAULT );
        lv_obj_set_style_bg_opa(btnClose, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_color(btnClose, lv_color_hex(0x9D9C9C), LV_PART_MAIN | LV_STATE_DEFAULT );
        lv_obj_set_style_bg_main_stop(btnClose, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_stop(btnClose, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_dir(btnClose, LV_GRAD_DIR_VER, LV_PART_MAIN| LV_STATE_DEFAULT);

        lv_obj_add_event_cb(btnClose, event_handler, LV_EVENT_ALL, NULL);

        lblClose = lv_label_create(btnClose);
        lv_obj_set_style_text_font(msg_box, &ui_font_Medium18, 0);
        lv_obj_set_width( lblClose, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height( lblClose, LV_SIZE_CONTENT);   /// 1
        lv_obj_set_align( lblClose, LV_ALIGN_CENTER );
        lv_label_set_text(lblClose,"Close");
        lv_obj_set_style_text_color(lblClose, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
        lv_obj_set_style_text_opa(lblClose, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
//        lv_obj_set_style_text_font(lblClose, &lv_font_montserrat_14, LV_PART_MAIN| LV_STATE_DEFAULT);

        lv_obj_t * background_obj = lv_obj_get_parent(msg_box);
        if(background_obj != NULL)
        {
            lv_obj_set_style_bg_opa(background_obj, LV_OPA_30, 0);
            lv_obj_set_style_bg_color(background_obj, lv_palette_lighten(LV_PALETTE_GREY, 1), 0);
        }
    }
}


