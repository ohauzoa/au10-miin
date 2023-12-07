#ifndef _UI_CALIBRATION_H
#define _UI_CALIBRATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lv_ui/ui.h"


void event_calibration_init(void);


int convert_voltage(int type, int energy);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
