// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.3
// LVGL version: 8.3.6
// Project name: miin_Project

#include "../ui.h"


// COMPONENT Grp Table Mode

lv_obj_t *ui_Grp_Table_Mode_create(lv_obj_t *comp_parent) {

lv_obj_t *cui_Grp_Table_Mode;
cui_Grp_Table_Mode = lv_obj_create(comp_parent);
lv_obj_set_width( cui_Grp_Table_Mode, 283);
lv_obj_set_height( cui_Grp_Table_Mode, 134);
lv_obj_set_x( cui_Grp_Table_Mode, -369 );
lv_obj_set_y( cui_Grp_Table_Mode, -190 );
lv_obj_set_align( cui_Grp_Table_Mode, LV_ALIGN_CENTER );
lv_obj_set_flex_flow(cui_Grp_Table_Mode,LV_FLEX_FLOW_ROW_WRAP);
lv_obj_set_flex_align(cui_Grp_Table_Mode, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
lv_obj_clear_flag( cui_Grp_Table_Mode, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(cui_Grp_Table_Mode, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(cui_Grp_Table_Mode, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(cui_Grp_Table_Mode, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_left(cui_Grp_Table_Mode, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_right(cui_Grp_Table_Mode, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_top(cui_Grp_Table_Mode, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_bottom(cui_Grp_Table_Mode, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_row(cui_Grp_Table_Mode, 9, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_column(cui_Grp_Table_Mode, 10, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_t *cui_lblTableMode0;
cui_lblTableMode0 = lv_label_create(cui_Grp_Table_Mode);
lv_obj_set_width( cui_lblTableMode0, 128);
lv_obj_set_height( cui_lblTableMode0, 34);
lv_obj_set_x( cui_lblTableMode0, -284 );
lv_obj_set_y( cui_lblTableMode0, -13 );
lv_obj_set_align( cui_lblTableMode0, LV_ALIGN_TOP_MID );
lv_label_set_text(cui_lblTableMode0,"1064 S");
lv_obj_add_flag( cui_lblTableMode0, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE );   /// Flags
lv_obj_set_style_text_color(cui_lblTableMode0, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(cui_lblTableMode0, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_letter_space(cui_lblTableMode0, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_line_space(cui_lblTableMode0, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(cui_lblTableMode0, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(cui_lblTableMode0, &ui_font_Medium18, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_radius(cui_lblTableMode0, 3, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(cui_lblTableMode0, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(cui_lblTableMode0, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(cui_lblTableMode0, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_color(cui_lblTableMode0, lv_color_hex(0x999999), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_shadow_opa(cui_lblTableMode0, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_width(cui_lblTableMode0, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_spread(cui_lblTableMode0, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_x(cui_lblTableMode0, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_y(cui_lblTableMode0, 6, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_left(cui_lblTableMode0, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_right(cui_lblTableMode0, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_top(cui_lblTableMode0, 6, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_bottom(cui_lblTableMode0, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_color(cui_lblTableMode0, lv_color_hex(0x222222), LV_PART_MAIN | LV_STATE_CHECKED );
lv_obj_set_style_text_opa(cui_lblTableMode0, 255, LV_PART_MAIN| LV_STATE_CHECKED);
lv_obj_set_style_bg_color(cui_lblTableMode0, lv_color_hex(0xFF1C1C), LV_PART_MAIN | LV_STATE_CHECKED );
lv_obj_set_style_bg_opa(cui_lblTableMode0, 255, LV_PART_MAIN| LV_STATE_CHECKED);

lv_obj_t *cui_lblTableMode1;
cui_lblTableMode1 = lv_label_create(cui_Grp_Table_Mode);
lv_obj_set_width( cui_lblTableMode1, 128);
lv_obj_set_height( cui_lblTableMode1, 34);
lv_obj_set_x( cui_lblTableMode1, -141 );
lv_obj_set_y( cui_lblTableMode1, -16 );
lv_obj_set_align( cui_lblTableMode1, LV_ALIGN_TOP_MID );
lv_label_set_text(cui_lblTableMode1,"1064 D");
lv_obj_add_flag( cui_lblTableMode1, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE );   /// Flags
lv_obj_set_style_text_color(cui_lblTableMode1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(cui_lblTableMode1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_letter_space(cui_lblTableMode1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_line_space(cui_lblTableMode1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(cui_lblTableMode1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(cui_lblTableMode1, &ui_font_Medium18, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_radius(cui_lblTableMode1, 3, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(cui_lblTableMode1, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(cui_lblTableMode1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(cui_lblTableMode1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_color(cui_lblTableMode1, lv_color_hex(0xB1B1B1), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_shadow_opa(cui_lblTableMode1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_width(cui_lblTableMode1, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_spread(cui_lblTableMode1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_x(cui_lblTableMode1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_y(cui_lblTableMode1, 6, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_left(cui_lblTableMode1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_right(cui_lblTableMode1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_top(cui_lblTableMode1, 6, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_bottom(cui_lblTableMode1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_color(cui_lblTableMode1, lv_color_hex(0x222222), LV_PART_MAIN | LV_STATE_CHECKED );
lv_obj_set_style_text_opa(cui_lblTableMode1, 255, LV_PART_MAIN| LV_STATE_CHECKED);
lv_obj_set_style_bg_color(cui_lblTableMode1, lv_color_hex(0xFF1C1C), LV_PART_MAIN | LV_STATE_CHECKED );
lv_obj_set_style_bg_opa(cui_lblTableMode1, 255, LV_PART_MAIN| LV_STATE_CHECKED);

lv_obj_t *cui_lblTableMode2;
cui_lblTableMode2 = lv_label_create(cui_Grp_Table_Mode);
lv_obj_set_width( cui_lblTableMode2, 128);
lv_obj_set_height( cui_lblTableMode2, 34);
lv_obj_set_x( cui_lblTableMode2, 2 );
lv_obj_set_y( cui_lblTableMode2, -13 );
lv_obj_set_align( cui_lblTableMode2, LV_ALIGN_TOP_MID );
lv_label_set_text(cui_lblTableMode2,"MiiN");
lv_obj_add_flag( cui_lblTableMode2, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE );   /// Flags
lv_obj_set_style_text_color(cui_lblTableMode2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(cui_lblTableMode2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_letter_space(cui_lblTableMode2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_line_space(cui_lblTableMode2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(cui_lblTableMode2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(cui_lblTableMode2, &ui_font_Medium18, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_radius(cui_lblTableMode2, 3, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(cui_lblTableMode2, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(cui_lblTableMode2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(cui_lblTableMode2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_color(cui_lblTableMode2, lv_color_hex(0xB1B1B1), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_shadow_opa(cui_lblTableMode2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_width(cui_lblTableMode2, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_spread(cui_lblTableMode2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_x(cui_lblTableMode2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_y(cui_lblTableMode2, 6, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_left(cui_lblTableMode2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_right(cui_lblTableMode2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_top(cui_lblTableMode2, 6, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_bottom(cui_lblTableMode2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_color(cui_lblTableMode2, lv_color_hex(0x222222), LV_PART_MAIN | LV_STATE_CHECKED );
lv_obj_set_style_text_opa(cui_lblTableMode2, 255, LV_PART_MAIN| LV_STATE_CHECKED);
lv_obj_set_style_bg_color(cui_lblTableMode2, lv_color_hex(0xFF1C1C), LV_PART_MAIN | LV_STATE_CHECKED );
lv_obj_set_style_bg_opa(cui_lblTableMode2, 255, LV_PART_MAIN| LV_STATE_CHECKED);

lv_obj_t *cui_lblTableMode3;
cui_lblTableMode3 = lv_label_create(cui_Grp_Table_Mode);
lv_obj_set_width( cui_lblTableMode3, 128);
lv_obj_set_height( cui_lblTableMode3, 34);
lv_obj_set_x( cui_lblTableMode3, 145 );
lv_obj_set_y( cui_lblTableMode3, -13 );
lv_obj_set_align( cui_lblTableMode3, LV_ALIGN_TOP_MID );
lv_label_set_text(cui_lblTableMode3,"532");
lv_obj_add_flag( cui_lblTableMode3, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE );   /// Flags
lv_obj_set_style_text_color(cui_lblTableMode3, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(cui_lblTableMode3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_letter_space(cui_lblTableMode3, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_line_space(cui_lblTableMode3, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(cui_lblTableMode3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(cui_lblTableMode3, &ui_font_Medium18, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_radius(cui_lblTableMode3, 3, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(cui_lblTableMode3, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(cui_lblTableMode3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(cui_lblTableMode3, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_color(cui_lblTableMode3, lv_color_hex(0xB1B1B1), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_shadow_opa(cui_lblTableMode3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_width(cui_lblTableMode3, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_spread(cui_lblTableMode3, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_x(cui_lblTableMode3, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_y(cui_lblTableMode3, 6, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_left(cui_lblTableMode3, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_right(cui_lblTableMode3, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_top(cui_lblTableMode3, 6, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_bottom(cui_lblTableMode3, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_color(cui_lblTableMode3, lv_color_hex(0x222222), LV_PART_MAIN | LV_STATE_CHECKED );
lv_obj_set_style_text_opa(cui_lblTableMode3, 255, LV_PART_MAIN| LV_STATE_CHECKED);
lv_obj_set_style_bg_color(cui_lblTableMode3, lv_color_hex(0xFF1C1C), LV_PART_MAIN | LV_STATE_CHECKED );
lv_obj_set_style_bg_opa(cui_lblTableMode3, 255, LV_PART_MAIN| LV_STATE_CHECKED);

lv_obj_t *cui_lblTableMode4;
cui_lblTableMode4 = lv_label_create(cui_Grp_Table_Mode);
lv_obj_set_width( cui_lblTableMode4, 128);
lv_obj_set_height( cui_lblTableMode4, 34);
lv_obj_set_x( cui_lblTableMode4, 288 );
lv_obj_set_y( cui_lblTableMode4, -13 );
lv_obj_set_align( cui_lblTableMode4, LV_ALIGN_TOP_MID );
lv_label_set_text(cui_lblTableMode4,"Quasi");
lv_obj_add_flag( cui_lblTableMode4, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE );   /// Flags
lv_obj_set_style_text_color(cui_lblTableMode4, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(cui_lblTableMode4, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_letter_space(cui_lblTableMode4, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_line_space(cui_lblTableMode4, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_align(cui_lblTableMode4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(cui_lblTableMode4, &ui_font_Medium18, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_radius(cui_lblTableMode4, 3, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(cui_lblTableMode4, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(cui_lblTableMode4, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(cui_lblTableMode4, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_color(cui_lblTableMode4, lv_color_hex(0xB1B1B1), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_shadow_opa(cui_lblTableMode4, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_width(cui_lblTableMode4, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_spread(cui_lblTableMode4, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_x(cui_lblTableMode4, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_ofs_y(cui_lblTableMode4, 6, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_left(cui_lblTableMode4, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_right(cui_lblTableMode4, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_top(cui_lblTableMode4, 6, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_bottom(cui_lblTableMode4, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_color(cui_lblTableMode4, lv_color_hex(0x222222), LV_PART_MAIN | LV_STATE_CHECKED );
lv_obj_set_style_text_opa(cui_lblTableMode4, 255, LV_PART_MAIN| LV_STATE_CHECKED);
lv_obj_set_style_bg_color(cui_lblTableMode4, lv_color_hex(0xFF1C1C), LV_PART_MAIN | LV_STATE_CHECKED );
lv_obj_set_style_bg_opa(cui_lblTableMode4, 255, LV_PART_MAIN| LV_STATE_CHECKED);

lv_obj_t ** children = lv_mem_alloc(sizeof(lv_obj_t *) * _UI_COMP_GRP_TABLE_MODE_NUM);
children[UI_COMP_GRP_TABLE_MODE_GRP_TABLE_MODE] = cui_Grp_Table_Mode;
children[UI_COMP_GRP_TABLE_MODE_LBLTABLEMODE0] = cui_lblTableMode0;
children[UI_COMP_GRP_TABLE_MODE_LBLTABLEMODE1] = cui_lblTableMode1;
children[UI_COMP_GRP_TABLE_MODE_LBLTABLEMODE2] = cui_lblTableMode2;
children[UI_COMP_GRP_TABLE_MODE_LBLTABLEMODE3] = cui_lblTableMode3;
children[UI_COMP_GRP_TABLE_MODE_LBLTABLEMODE4] = cui_lblTableMode4;
lv_obj_add_event_cb(cui_Grp_Table_Mode, get_component_child_event_cb, LV_EVENT_GET_COMP_CHILD, children);
lv_obj_add_event_cb(cui_Grp_Table_Mode, del_component_child_event_cb, LV_EVENT_DELETE, children);
ui_comp_Grp_Table_Mode_create_hook(cui_Grp_Table_Mode);
return cui_Grp_Table_Mode; 
}

