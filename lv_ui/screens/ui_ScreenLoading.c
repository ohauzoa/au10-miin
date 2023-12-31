// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.3
// LVGL version: 8.3.6
// Project name: miin_Project

#include "../ui.h"

void ui_ScreenLoading_screen_init(void)
{
ui_ScreenLoading = lv_obj_create(NULL);
lv_obj_clear_flag( ui_ScreenLoading, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_ScreenLoading, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_ScreenLoading, lv_color_hex(0xBFBFBF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_ScreenLoading, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_ImageIntroBg0 = lv_img_create(ui_ScreenLoading);
lv_img_set_src(ui_ImageIntroBg0, &ui_img_intro_0_png);
lv_obj_set_height( ui_ImageIntroBg0, 600);
lv_obj_set_width( ui_ImageIntroBg0, LV_SIZE_CONTENT);  /// 1
lv_obj_set_align( ui_ImageIntroBg0, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_ImageIntroBg0, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_ImageIntroBg0, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_GRP_Intro = lv_obj_create(ui_ScreenLoading);
lv_obj_set_width( ui_GRP_Intro, 526);
lv_obj_set_height( ui_GRP_Intro, 409);
lv_obj_set_align( ui_GRP_Intro, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_GRP_Intro, LV_OBJ_FLAG_HIDDEN );   /// Flags
lv_obj_clear_flag( ui_GRP_Intro, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_GRP_Intro, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_GRP_Intro, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_GRP_Intro, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_GRP_Intro, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_ImageIntroLogo = lv_img_create(ui_GRP_Intro);
lv_img_set_src(ui_ImageIntroLogo, &ui_img_logo_ltra_png);
lv_obj_set_width( ui_ImageIntroLogo, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_ImageIntroLogo, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_ImageIntroLogo, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_ImageIntroLogo, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_ImageIntroLogo, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_BarIntro = lv_bar_create(ui_GRP_Intro);
lv_bar_set_value(ui_BarIntro,50,LV_ANIM_OFF);
lv_obj_set_width( ui_BarIntro, 352);
lv_obj_set_height( ui_BarIntro, 6);
lv_obj_set_x( ui_BarIntro, -3 );
lv_obj_set_y( ui_BarIntro, 184 );
lv_obj_set_align( ui_BarIntro, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_BarIntro, LV_OBJ_FLAG_CLICKABLE );   /// Flags
lv_obj_set_style_radius(ui_BarIntro, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_BarIntro, lv_color_hex(0xDDDDDD), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_BarIntro, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_set_style_radius(ui_BarIntro, 0, LV_PART_INDICATOR| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_BarIntro, lv_color_hex(0xFF3030), LV_PART_INDICATOR | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_BarIntro, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);

ui_Grp_Password = lv_obj_create(ui_ScreenLoading);
lv_obj_set_width( ui_Grp_Password, 809);
lv_obj_set_height( ui_Grp_Password, 455);
lv_obj_set_x( ui_Grp_Password, 0 );
lv_obj_set_y( ui_Grp_Password, 44 );
lv_obj_set_align( ui_Grp_Password, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_Grp_Password, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_Grp_Password, 5, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_Grp_Password, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Grp_Password, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_Grp_Password, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_GrpPwBtn = lv_obj_create(ui_Grp_Password);
lv_obj_set_width( ui_GrpPwBtn, 470);
lv_obj_set_height( ui_GrpPwBtn, 205);
lv_obj_set_x( ui_GrpPwBtn, 3 );
lv_obj_set_y( ui_GrpPwBtn, -37 );
lv_obj_set_align( ui_GrpPwBtn, LV_ALIGN_CENTER );
lv_obj_set_flex_flow(ui_GrpPwBtn,LV_FLEX_FLOW_ROW_WRAP);
lv_obj_set_flex_align(ui_GrpPwBtn, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
lv_obj_clear_flag( ui_GrpPwBtn, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_GrpPwBtn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_GrpPwBtn, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_GrpPwBtn, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_left(ui_GrpPwBtn, 25, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_right(ui_GrpPwBtn, 25, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_top(ui_GrpPwBtn, 25, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_bottom(ui_GrpPwBtn, 25, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_row(ui_GrpPwBtn, 16, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_column(ui_GrpPwBtn, 16, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_btnPW0 = lv_img_create(ui_GrpPwBtn);
lv_img_set_src(ui_btnPW0, &ui_img_button70_1_png);
lv_obj_set_width( ui_btnPW0, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPW0, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_btnPW0, 111 );
lv_obj_set_y( ui_btnPW0, 99 );
lv_obj_set_align( ui_btnPW0, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_btnPW0, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_btnPW0, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_btnPW0, 80, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_blend_mode(ui_btnPW0, LV_BLEND_MODE_NORMAL, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_opa(ui_btnPW0, 255, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor(ui_btnPW0, lv_color_hex(0xFFFDFD), LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor_opa(ui_btnPW0, 150, LV_PART_MAIN| LV_STATE_PRESSED);

ui_btnPWlbl0 = lv_label_create(ui_btnPW0);
lv_obj_set_width( ui_btnPWlbl0, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPWlbl0, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_btnPWlbl0, LV_ALIGN_CENTER );
lv_label_set_text(ui_btnPWlbl0,"0");
lv_obj_set_style_text_color(ui_btnPWlbl0, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_btnPWlbl0, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_btnPWlbl0, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_btnPWlbl0, &ui_font_EbrimaBold40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_btnPW1 = lv_img_create(ui_GrpPwBtn);
lv_img_set_src(ui_btnPW1, &ui_img_button70_1_png);
lv_obj_set_width( ui_btnPW1, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPW1, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_btnPW1, -247 );
lv_obj_set_y( ui_btnPW1, 11 );
lv_obj_set_align( ui_btnPW1, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_btnPW1, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_btnPW1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_btnPW1, 80, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_blend_mode(ui_btnPW1, LV_BLEND_MODE_NORMAL, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_opa(ui_btnPW1, 255, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor(ui_btnPW1, lv_color_hex(0xFFFDFD), LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor_opa(ui_btnPW1, 150, LV_PART_MAIN| LV_STATE_PRESSED);

ui_btnPWlbl1 = lv_label_create(ui_btnPW1);
lv_obj_set_width( ui_btnPWlbl1, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPWlbl1, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_btnPWlbl1, LV_ALIGN_CENTER );
lv_label_set_text(ui_btnPWlbl1,"1");
lv_obj_set_style_text_color(ui_btnPWlbl1, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_btnPWlbl1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_btnPWlbl1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_btnPWlbl1, &ui_font_EbrimaBold40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_btnPW2 = lv_img_create(ui_GrpPwBtn);
lv_img_set_src(ui_btnPW2, &ui_img_button70_1_png);
lv_obj_set_width( ui_btnPW2, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPW2, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_btnPW2, -158 );
lv_obj_set_y( ui_btnPW2, 11 );
lv_obj_set_align( ui_btnPW2, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_btnPW2, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_btnPW2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_btnPW2, 80, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_blend_mode(ui_btnPW2, LV_BLEND_MODE_NORMAL, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_opa(ui_btnPW2, 255, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor(ui_btnPW2, lv_color_hex(0xFFFDFD), LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor_opa(ui_btnPW2, 150, LV_PART_MAIN| LV_STATE_PRESSED);

ui_btnPWlbl2 = lv_label_create(ui_btnPW2);
lv_obj_set_width( ui_btnPWlbl2, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPWlbl2, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_btnPWlbl2, LV_ALIGN_CENTER );
lv_label_set_text(ui_btnPWlbl2,"2");
lv_obj_set_style_text_color(ui_btnPWlbl2, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_btnPWlbl2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_btnPWlbl2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_btnPWlbl2, &ui_font_EbrimaBold40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_btnPW3 = lv_img_create(ui_GrpPwBtn);
lv_img_set_src(ui_btnPW3, &ui_img_button70_1_png);
lv_obj_set_width( ui_btnPW3, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPW3, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_btnPW3, -67 );
lv_obj_set_y( ui_btnPW3, 11 );
lv_obj_set_align( ui_btnPW3, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_btnPW3, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_btnPW3, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_btnPW3, 80, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_blend_mode(ui_btnPW3, LV_BLEND_MODE_NORMAL, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_opa(ui_btnPW3, 255, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor(ui_btnPW3, lv_color_hex(0xFFFDFD), LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor_opa(ui_btnPW3, 150, LV_PART_MAIN| LV_STATE_PRESSED);

ui_btnPWlbl3 = lv_label_create(ui_btnPW3);
lv_obj_set_width( ui_btnPWlbl3, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPWlbl3, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_btnPWlbl3, LV_ALIGN_CENTER );
lv_label_set_text(ui_btnPWlbl3,"3");
lv_obj_set_style_text_color(ui_btnPWlbl3, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_btnPWlbl3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_btnPWlbl3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_btnPWlbl3, &ui_font_EbrimaBold40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_btnPW4 = lv_img_create(ui_GrpPwBtn);
lv_img_set_src(ui_btnPW4, &ui_img_button70_1_png);
lv_obj_set_width( ui_btnPW4, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPW4, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_btnPW4, 21 );
lv_obj_set_y( ui_btnPW4, 11 );
lv_obj_set_align( ui_btnPW4, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_btnPW4, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_btnPW4, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_btnPW4, 80, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_blend_mode(ui_btnPW4, LV_BLEND_MODE_NORMAL, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_opa(ui_btnPW4, 255, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor(ui_btnPW4, lv_color_hex(0xFFFDFD), LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor_opa(ui_btnPW4, 150, LV_PART_MAIN| LV_STATE_PRESSED);

ui_btnPWlbl4 = lv_label_create(ui_btnPW4);
lv_obj_set_width( ui_btnPWlbl4, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPWlbl4, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_btnPWlbl4, LV_ALIGN_CENTER );
lv_label_set_text(ui_btnPWlbl4,"4");
lv_obj_set_style_text_color(ui_btnPWlbl4, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_btnPWlbl4, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_btnPWlbl4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_btnPWlbl4, &ui_font_EbrimaBold40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_btnPW5 = lv_img_create(ui_GrpPwBtn);
lv_img_set_src(ui_btnPW5, &ui_img_button70_1_png);
lv_obj_set_width( ui_btnPW5, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPW5, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_btnPW5, 111 );
lv_obj_set_y( ui_btnPW5, 11 );
lv_obj_set_align( ui_btnPW5, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_btnPW5, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_btnPW5, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_btnPW5, 80, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_blend_mode(ui_btnPW5, LV_BLEND_MODE_NORMAL, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_opa(ui_btnPW5, 255, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor(ui_btnPW5, lv_color_hex(0xFFFDFD), LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor_opa(ui_btnPW5, 150, LV_PART_MAIN| LV_STATE_PRESSED);

ui_btnPWlbl5 = lv_label_create(ui_btnPW5);
lv_obj_set_width( ui_btnPWlbl5, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPWlbl5, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_btnPWlbl5, LV_ALIGN_CENTER );
lv_label_set_text(ui_btnPWlbl5,"5");
lv_obj_set_style_text_color(ui_btnPWlbl5, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_btnPWlbl5, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_btnPWlbl5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_btnPWlbl5, &ui_font_EbrimaBold40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_btnPW6 = lv_img_create(ui_GrpPwBtn);
lv_img_set_src(ui_btnPW6, &ui_img_button70_1_png);
lv_obj_set_width( ui_btnPW6, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPW6, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_btnPW6, -247 );
lv_obj_set_y( ui_btnPW6, 99 );
lv_obj_set_align( ui_btnPW6, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_btnPW6, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_btnPW6, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_btnPW6, 80, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_blend_mode(ui_btnPW6, LV_BLEND_MODE_NORMAL, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_opa(ui_btnPW6, 255, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor(ui_btnPW6, lv_color_hex(0xFFFDFD), LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor_opa(ui_btnPW6, 150, LV_PART_MAIN| LV_STATE_PRESSED);

ui_btnPWlbl6 = lv_label_create(ui_btnPW6);
lv_obj_set_width( ui_btnPWlbl6, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPWlbl6, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_btnPWlbl6, LV_ALIGN_CENTER );
lv_label_set_text(ui_btnPWlbl6,"6");
lv_obj_set_style_text_color(ui_btnPWlbl6, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_btnPWlbl6, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_btnPWlbl6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_btnPWlbl6, &ui_font_EbrimaBold40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_btnPW7 = lv_img_create(ui_GrpPwBtn);
lv_img_set_src(ui_btnPW7, &ui_img_button70_1_png);
lv_obj_set_width( ui_btnPW7, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPW7, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_btnPW7, -158 );
lv_obj_set_y( ui_btnPW7, 99 );
lv_obj_set_align( ui_btnPW7, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_btnPW7, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_btnPW7, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_btnPW7, 80, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_blend_mode(ui_btnPW7, LV_BLEND_MODE_NORMAL, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_opa(ui_btnPW7, 255, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor(ui_btnPW7, lv_color_hex(0xFFFDFD), LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor_opa(ui_btnPW7, 150, LV_PART_MAIN| LV_STATE_PRESSED);

ui_btnPWlbl7 = lv_label_create(ui_btnPW7);
lv_obj_set_width( ui_btnPWlbl7, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPWlbl7, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_btnPWlbl7, LV_ALIGN_CENTER );
lv_label_set_text(ui_btnPWlbl7,"7");
lv_obj_set_style_text_color(ui_btnPWlbl7, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_btnPWlbl7, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_btnPWlbl7, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_btnPWlbl7, &ui_font_EbrimaBold40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_btnPW8 = lv_img_create(ui_GrpPwBtn);
lv_img_set_src(ui_btnPW8, &ui_img_button70_1_png);
lv_obj_set_width( ui_btnPW8, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPW8, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_btnPW8, -67 );
lv_obj_set_y( ui_btnPW8, 99 );
lv_obj_set_align( ui_btnPW8, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_btnPW8, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_btnPW8, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_btnPW8, 80, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_blend_mode(ui_btnPW8, LV_BLEND_MODE_NORMAL, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_opa(ui_btnPW8, 255, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor(ui_btnPW8, lv_color_hex(0xFFFDFD), LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor_opa(ui_btnPW8, 150, LV_PART_MAIN| LV_STATE_PRESSED);

ui_btnPWlbl8 = lv_label_create(ui_btnPW8);
lv_obj_set_width( ui_btnPWlbl8, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPWlbl8, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_btnPWlbl8, LV_ALIGN_CENTER );
lv_label_set_text(ui_btnPWlbl8,"8");
lv_obj_set_style_text_color(ui_btnPWlbl8, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_btnPWlbl8, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_btnPWlbl8, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_btnPWlbl8, &ui_font_EbrimaBold40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_btnPW9 = lv_img_create(ui_GrpPwBtn);
lv_img_set_src(ui_btnPW9, &ui_img_button70_1_png);
lv_obj_set_width( ui_btnPW9, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPW9, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_btnPW9, 21 );
lv_obj_set_y( ui_btnPW9, 99 );
lv_obj_set_align( ui_btnPW9, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_btnPW9, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_btnPW9, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_btnPW9, 80, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_blend_mode(ui_btnPW9, LV_BLEND_MODE_NORMAL, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_opa(ui_btnPW9, 255, LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor(ui_btnPW9, lv_color_hex(0xFFFDFD), LV_PART_MAIN| LV_STATE_PRESSED);
lv_obj_set_style_img_recolor_opa(ui_btnPW9, 150, LV_PART_MAIN| LV_STATE_PRESSED);

ui_btnPWlbl9 = lv_label_create(ui_btnPW9);
lv_obj_set_width( ui_btnPWlbl9, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPWlbl9, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_btnPWlbl9, LV_ALIGN_CENTER );
lv_label_set_text(ui_btnPWlbl9,"9");
lv_obj_set_style_text_color(ui_btnPWlbl9, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_btnPWlbl9, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_btnPWlbl9, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_btnPWlbl9, &ui_font_EbrimaBold40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_ImagePassTitle = lv_img_create(ui_Grp_Password);
lv_img_set_src(ui_ImagePassTitle, &ui_img_password_title_png);
lv_obj_set_width( ui_ImagePassTitle, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_ImagePassTitle, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_ImagePassTitle, -188 );
lv_obj_set_y( ui_ImagePassTitle, -83 );
lv_obj_set_align( ui_ImagePassTitle, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_ImagePassTitle, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_ImagePassTitle, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_Panel2 = lv_obj_create(ui_Grp_Password);
lv_obj_set_width( ui_Panel2, 126);
lv_obj_set_height( ui_Panel2, 286);
lv_obj_set_x( ui_Panel2, 298 );
lv_obj_set_y( ui_Panel2, -73 );
lv_obj_set_align( ui_Panel2, LV_ALIGN_CENTER );
lv_obj_set_flex_flow(ui_Panel2,LV_FLEX_FLOW_COLUMN);
lv_obj_set_flex_align(ui_Panel2, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
lv_obj_clear_flag( ui_Panel2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_Panel2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Panel2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_Panel2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_left(ui_Panel2, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_right(ui_Panel2, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_top(ui_Panel2, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_bottom(ui_Panel2, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_row(ui_Panel2, 24, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_column(ui_Panel2, 20, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_btnPW10 = lv_btn_create(ui_Panel2);
lv_obj_set_width( ui_btnPW10, 90);
lv_obj_set_height( ui_btnPW10, 65);
lv_obj_set_x( ui_btnPW10, 300 );
lv_obj_set_y( ui_btnPW10, -173 );
lv_obj_set_align( ui_btnPW10, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_btnPW10, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_btnPW10, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_btnPW10, 3, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_btnPW10, lv_color_hex(0xD1D1D1), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_btnPW10, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_color(ui_btnPW10, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_shadow_opa(ui_btnPW10, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_width(ui_btnPW10, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_spread(ui_btnPW10, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_x(ui_btnPW10, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_y(ui_btnPW10, 4, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_btnPWlbl10 = lv_label_create(ui_btnPW10);
lv_obj_set_width( ui_btnPWlbl10, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPWlbl10, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_btnPWlbl10, 0 );
lv_obj_set_y( ui_btnPWlbl10, 3 );
lv_obj_set_align( ui_btnPWlbl10, LV_ALIGN_CENTER );
lv_label_set_text(ui_btnPWlbl10,"8");
lv_obj_set_style_text_color(ui_btnPWlbl10, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_btnPWlbl10, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_btnPWlbl10, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_btnPWlbl10, &lv_font_montserrat_40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_btnPW11 = lv_btn_create(ui_Panel2);
lv_obj_set_width( ui_btnPW11, 90);
lv_obj_set_height( ui_btnPW11, 65);
lv_obj_set_x( ui_btnPW11, 300 );
lv_obj_set_y( ui_btnPW11, -75 );
lv_obj_set_align( ui_btnPW11, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_btnPW11, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_btnPW11, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_btnPW11, 3, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_btnPW11, lv_color_hex(0xD1D1D1), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_btnPW11, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_color(ui_btnPW11, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_shadow_opa(ui_btnPW11, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_width(ui_btnPW11, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_spread(ui_btnPW11, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_x(ui_btnPW11, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_y(ui_btnPW11, 4, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_btnPWlbl11 = lv_label_create(ui_btnPW11);
lv_obj_set_width( ui_btnPWlbl11, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPWlbl11, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_btnPWlbl11, 0 );
lv_obj_set_y( ui_btnPWlbl11, 3 );
lv_obj_set_align( ui_btnPWlbl11, LV_ALIGN_CENTER );
lv_label_set_text(ui_btnPWlbl11,"8");
lv_obj_set_style_text_color(ui_btnPWlbl11, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_btnPWlbl11, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_btnPWlbl11, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_btnPWlbl11, &lv_font_montserrat_40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_btnPW12 = lv_btn_create(ui_Panel2);
lv_obj_set_width( ui_btnPW12, 90);
lv_obj_set_height( ui_btnPW12, 65);
lv_obj_set_x( ui_btnPW12, 300 );
lv_obj_set_y( ui_btnPW12, 14 );
lv_obj_set_align( ui_btnPW12, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_btnPW12, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_btnPW12, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_btnPW12, 3, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_btnPW12, lv_color_hex(0xD1D1D1), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_btnPW12, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_btnPW12, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_outline_width(ui_btnPW12, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_outline_pad(ui_btnPW12, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_color(ui_btnPW12, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_shadow_opa(ui_btnPW12, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_width(ui_btnPW12, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_spread(ui_btnPW12, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_x(ui_btnPW12, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_y(ui_btnPW12, 4, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_btnPWlbl12 = lv_label_create(ui_btnPW12);
lv_obj_set_width( ui_btnPWlbl12, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_btnPWlbl12, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_btnPWlbl12, 0 );
lv_obj_set_y( ui_btnPWlbl12, 3 );
lv_obj_set_align( ui_btnPWlbl12, LV_ALIGN_CENTER );
lv_label_set_text(ui_btnPWlbl12,"8");
lv_obj_set_style_text_color(ui_btnPWlbl12, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_btnPWlbl12, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_btnPWlbl12, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_btnPWlbl12, &lv_font_montserrat_40, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_editPassword = lv_textarea_create(ui_Grp_Password);
lv_obj_set_width( ui_editPassword, 423);
lv_obj_set_height( ui_editPassword, LV_SIZE_CONTENT);   /// 72
lv_obj_set_x( ui_editPassword, 0 );
lv_obj_set_y( ui_editPassword, 3 );
lv_obj_set_align( ui_editPassword, LV_ALIGN_TOP_MID );
lv_textarea_set_max_length(ui_editPassword,20);
lv_textarea_set_one_line(ui_editPassword,true);
lv_textarea_set_password_mode(ui_editPassword, true);
lv_obj_clear_flag( ui_editPassword, LV_OBJ_FLAG_CLICKABLE );    /// Flags
lv_obj_set_style_text_color(ui_editPassword, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_editPassword, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(ui_editPassword, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_editPassword, &lv_font_montserrat_40, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_radius(ui_editPassword, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_editPassword, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_editPassword, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_editPassword, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_left(ui_editPassword, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_right(ui_editPassword, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_top(ui_editPassword, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_bottom(ui_editPassword, 10, LV_PART_MAIN| LV_STATE_DEFAULT);



ui_logo_password = lv_img_create(ui_Grp_Password);
lv_img_set_src(ui_logo_password, &ui_img_logo_ltra_pass_png);
lv_obj_set_width( ui_logo_password, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_logo_password, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_logo_password, 0 );
lv_obj_set_y( ui_logo_password, -8 );
lv_obj_set_align( ui_logo_password, LV_ALIGN_BOTTOM_MID );
lv_obj_add_flag( ui_logo_password, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_clear_flag( ui_logo_password, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

#include "../../src/ui_loading.h"
event_loading_init();

}
