#ifndef _MIIN_PROJECT_UI_MSG_H
#define _MIIN_PROJECT_UI_MSG_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../lvgl/lvgl.h"

#include <stdbool.h>    // bool, true, false가 정의된 헤더 파일

////////////////////////////////////////////////////
////////////////////////////////////////////////////
#define CTRL_B      2
#define CTRL_C      3
#define CTRL_F      6

#define EOL         '\0'
#define ENTER       '\n'
#define BS          '\b'
#define ESC         0x1B
#define ESC2        '[' 
#define DELIMIT     '~'
#define DELETE      0x7F
#define UP_ARROW    'A'
#define DN_ARROW    'B'
#define RI_ARROW    'C'
#define LE_ARROW    'D'

#define BD_PROMPT       "Just4Fun"
#define BD_CMD_BUF_SZ   128
#define BD_MAX_ARGS     10
#define BD_ARG_BUF_SZ   32


void lv_msgbox_show(int type);

void init_command(void);
void *wait_command(void);
void parse_command(const char *command, int *argc, char **argv);

extern lv_obj_t * msg_box;







#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
