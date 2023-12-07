#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui_loading.h"
#include "config.h"


#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

static void password_process(void);

static int password_step = false;
// Realtime Timer사용시 cmake에 추가 -lrt 
timer_t firstTimerID;

static void timer_handler( int sig, siginfo_t *si, void *uc )
{ 
    timer_t *tidp;
    tidp = si->si_value.sival_ptr;
   
    if ( *tidp == firstTimerID )
    {
        printf("firstTimerID\n");
        password_process();
        timer_delete(firstTimerID);
    }
}  



static void pw_timer(lv_timer_t *timer) {
    printf("ex\n");
//lv_timer_del(pw_timer);
}

static void row_gap_anim(void * obj, int32_t v)
{
    lv_obj_set_style_pad_row(obj, v, 0);
}

static void column_gap_anim(void * obj, int32_t v)
{
    lv_obj_set_style_pad_column(obj, v, 0);
}

static void password_process(void)
{
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";
    int cnt = 0;
    switch(password_step)
    {
        case 0: // start
            system("tinyplayer tts/Bells.mp3 &");
            lv_textarea_set_text(ui_editPassword, "");
            lv_textarea_set_placeholder_text(ui_editPassword, "Input Password..");
            password_step = 1;
            break;

        case 1: // enter
            lv_snprintf(buf, sizeof(buf), "%s", lv_textarea_get_text(ui_editPassword));
            if (strcmp(config.password, buf) == 0)
            {
                system("tinyplayer tts/Conclusion.mp3 &");
                lv_textarea_set_text(ui_editPassword, "");
                lv_textarea_set_placeholder_text(ui_editPassword, "Available Password!");
                _ui_screen_change(&ui_ScreenMain, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenMain_screen_init);

            }
            else
            {
                system("tinyplayer tts/Crystal.mp3 &");
                lv_textarea_set_text(ui_editPassword, "");
                lv_textarea_set_placeholder_text(ui_editPassword, "Invalid Password!");
            }
            break;

        case 2: // start
            system("tinyplayer tts/Bells.mp3 &");
            lv_textarea_set_text(ui_editPassword, "");
            lv_textarea_set_placeholder_text(ui_editPassword, "Current Password..");
            password_step = 3;
            break;

        case 3: // new pw
            lv_snprintf(buf, sizeof(buf), "%s", lv_textarea_get_text(ui_editPassword));
            if (strcmp(config.password, buf) == 0) 
            {
                system("tinyplayer tts/Conclusion.mp3 &");
                lv_textarea_set_text(ui_editPassword, "");
                lv_textarea_set_placeholder_text(ui_editPassword, "New Password");
                password_step = 4;
            }
            else
            {
                system("tinyplayer tts/Crystal.mp3 &");
                lv_textarea_set_text(ui_editPassword, "");
                lv_textarea_set_placeholder_text(ui_editPassword, "Invalid Password!");
            }
            break;

        case 4:
            lv_snprintf(buf, sizeof(buf), "%s", lv_textarea_get_text(ui_editPassword));
            cnt = strlen(buf);
            if(cnt != 4)
            {
                system("tinyplayer tts/Evening.mp3 &");
                lv_textarea_set_text(ui_editPassword, "");
                lv_textarea_set_placeholder_text(ui_editPassword, "Invalid PW Length!");
            }
            else
            {
                system("tinyplayer tts/Conclusion.mp3 &");
                lv_textarea_set_text(ui_editPassword, "");
                lv_textarea_set_placeholder_text(ui_editPassword, "New PW Write!");
                modify_config("config.ini", "config", "password", buf);
                create_timer(&firstTimerID, (void*)timer_handler, 2000, 0);
            }
            password_step = 0;
            lv_textarea_set_text(ui_editPassword, "");
            break;
    }
}


// 키가 많을때처리
static void event_pw_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_PRESSED) system("tinyplayer tts/S_HW_Touch_Calm.mp3 &");

    if(code == LV_EVENT_CLICKED) {
//        system("tinyplayer tts/S_HW_Touch_Calm.mp3 &");
//        system("tinyplayer tts/DingSound.mp3 &");

		char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";
		lv_snprintf(buf, sizeof(buf), "%s",  lv_list_get_btn_text(ui_Grp_Password, obj));
        lv_textarea_add_char(ui_editPassword, buf[0]);
    }
}

// delete
static void event_pw_btn_bs(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) system("tinyplayer tts/Beep_Once.mp3 &");
    if((event_code == LV_EVENT_CLICKED) || (event_code == LV_EVENT_LONG_PRESSED_REPEAT)) {
//        system("tinyplayer tts/Beep_Once.mp3 &");
        lv_textarea_del_char(ui_editPassword);
    }
}

// new password
static void event_pw_btn_new(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);

    if(event_code == LV_EVENT_CLICKED) {
        password_step = 2;
        password_process();
    }
}

// apply
static void event_pw_btn_apply(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
//        system("tinyplayer tts/New_World.mp3 &");
        password_process();
    }
}
static void event_pw_btn_apply1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";
        lv_snprintf(buf, sizeof(buf), "%s", lv_textarea_get_text(ui_editPassword));
        if (strcmp(config.password, buf) == 0)
        {
            _ui_screen_change(&ui_ScreenMain, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenMain_screen_init);

        }else
        {
            lv_textarea_set_text(ui_editPassword, "");
        }
    }
}

static void event_logo_password(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_LONG_PRESSED) {
//        _ui_screen_change(&ui_ScreenMain, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenMain_screen_init);
        _ui_screen_change(&ui_ScreenCalibration, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenCalibration_screen_init);
    }
}


void event_loading_init(void)
{
    static lv_style_t style_pr;
    lv_style_init(&style_pr);

    /*Add a large outline when pressed*/
    lv_style_set_outline_width(&style_pr, 30);
    lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);
    lv_style_set_outline_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE_GREY, 2));

    lv_style_set_translate_y(&style_pr, 5);
    lv_style_set_shadow_ofs_y(&style_pr, 3);

    /*Add a transition to the outline*/
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0};
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, TRANSITION_TIME, 0, NULL);

    static lv_style_t style_pr1;
    lv_style_init(&style_pr1);

    /*Add a large outline when pressed*/
    lv_style_set_outline_width(&style_pr1, 30);
    lv_style_set_outline_opa(&style_pr1, LV_OPA_TRANSP);
    lv_style_set_outline_color(&style_pr1, lv_palette_darken(LV_PALETTE_RED, 2));

    lv_style_set_translate_y(&style_pr1, 5);
    lv_style_set_shadow_ofs_y(&style_pr1, 3);

    static lv_style_t style_pr2;
    lv_style_init(&style_pr2);

    lv_style_set_translate_y(&style_pr2, 5);
    lv_style_set_shadow_ofs_y(&style_pr2, 3);

    lv_style_set_transition(&style_pr, &trans);
    lv_style_set_transition(&style_pr1, &trans);


    lv_obj_add_style(ui_btnPW0, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnPW1, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnPW2, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnPW3, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnPW4, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnPW5, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnPW6, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnPW7, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnPW8, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnPW9, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnPW10, &style_pr2, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnPW11, &style_pr2, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnPW12, &style_pr2, LV_STATE_PRESSED);

    lv_obj_add_event_cb(ui_btnPW0, event_pw_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPW1, event_pw_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPW2, event_pw_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPW3, event_pw_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPW4, event_pw_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPW5, event_pw_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPW6, event_pw_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPW7, event_pw_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPW8, event_pw_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPW9, event_pw_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPW10, event_pw_btn_bs, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPW11, event_pw_btn_new, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnPW12, event_pw_btn_apply, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_logo_password, event_logo_password, LV_EVENT_ALL, NULL);
    // 패스워드 버튼에 기호넣기
    lv_label_set_text(ui_btnPWlbl10, LV_SYMBOL_BACKSPACE);
    lv_label_set_text(ui_btnPWlbl11, LV_SYMBOL_FILE);
    lv_label_set_text(ui_btnPWlbl12, LV_SYMBOL_OK);

    lv_textarea_set_password_show_time(ui_editPassword, 300);
    password_process();

/*
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ui_GrpPwBtn);
    lv_anim_set_values(&a, 14, 20);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    lv_anim_set_exec_cb(&a, row_gap_anim);
    lv_anim_set_time(&a, 500);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_start(&a);

    lv_anim_set_exec_cb(&a, column_gap_anim);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 1000);
    lv_anim_start(&a);
*/
//    createTimer(&firstTimerID, timer_handler, 500, 0);
//    createTimer(&secondTimerID, timer_handler, 2000, 1);
//    createTimer(&firstTimerID,0, 500);
//    createTimer(&secondTimerID,2, 0);


}
