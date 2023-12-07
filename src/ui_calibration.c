
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ui_calibration.h"
#include "lv_ui/ui.h"

#include "config.h"
#include "uart.h"


static const char * const info1_text[] = {"Press Start Button", "Set the Voltage", "Set the Voltage", "Set the Energy", "Set the Energy", "Set the Energy", "Set the Energy", "Set the Energy" };
static const char * const info2_text[] = {"SETP 0/7", "SETP 1/7", "SETP 2/7", "SETP 3/7", "SETP 4/7", "SETP 5/7", "SETP 6/7", "SETP 7/7", "Modify Voltage" };
static const char * const info_limit[5][2] = { {"100", "1400"}, {"300", "2500"}, {"500", "3000"}, {"20", "500"}, {"500", "3500"} };


lv_obj_t ** ui_table_voltage[5][15] = {
    { &ui_txt1064sVolt0, &ui_txt1064sVolt1, &ui_txt1064sVolt2, &ui_txt1064sVolt3, &ui_txt1064sVolt4, &ui_txt1064sVolt5, &ui_txt1064sVolt6, &ui_txt1064sVolt7, &ui_txt1064sVolt8, &ui_txt1064sVolt9, &ui_txt1064sVolt10, &ui_txt1064sVolt11, &ui_txt1064sVolt12, &ui_txt1064sVolt13 , &ui_txt1064sVolt13 },
    { &ui_txt1064dVolt0, &ui_txt1064dVolt1, &ui_txt1064dVolt2, &ui_txt1064dVolt3, &ui_txt1064dVolt4, &ui_txt1064dVolt5, &ui_txt1064dVolt6, &ui_txt1064dVolt7, &ui_txt1064dVolt8, &ui_txt1064dVolt9, &ui_txt1064dVolt10, &ui_txt1064dVolt11, &ui_txt1064dVolt11, &ui_txt1064dVolt11 , &ui_txt1064dVolt11 },
    { &ui_txtMiinVolt0, &ui_txtMiinVolt1, &ui_txtMiinVolt2, &ui_txtMiinVolt3, &ui_txtMiinVolt4, &ui_txtMiinVolt5, &ui_txtMiinVolt6, &ui_txtMiinVolt7, &ui_txtMiinVolt8, &ui_txtMiinVolt9, &ui_txtMiinVolt10, &ui_txtMiinVolt11, &ui_txtMiinVolt12, &ui_txtMiinVolt12 , &ui_txtMiinVolt12 },
    { &ui_txt532Volt0, &ui_txt532Volt1, &ui_txt532Volt2, &ui_txt532Volt3, &ui_txt532Volt4, &ui_txt532Volt5, &ui_txt532Volt6, &ui_txt532Volt7, &ui_txt532Volt8, &ui_txt532Volt9, &ui_txt532Volt10, &ui_txt532Volt11, &ui_txt532Volt12, &ui_txt532Volt13 , &ui_txt532Volt14 },
    { &ui_txtQuasiVolt0, &ui_txtQuasiVolt1, &ui_txtQuasiVolt2, &ui_txtQuasiVolt3, &ui_txtQuasiVolt4, &ui_txtQuasiVolt5, &ui_txtQuasiVolt6, &ui_txtQuasiVolt7, &ui_txtQuasiVolt8, &ui_txtQuasiVolt9, &ui_txtQuasiVolt10, &ui_txtQuasiVolt10, &ui_txtQuasiVolt10, &ui_txtQuasiVolt10 , &ui_txtQuasiVolt10 }
};

lv_obj_t ** ui_table_qdelay[7] = {
    &ui_txt1064sQ1, &ui_txt1064dQ1, &ui_txt1064dQ2, &ui_txtMiinQ1High, &ui_txtMiinQ2High, &ui_txtMiinQ1Low, &ui_txtMiinQ2Low
};


static lv_obj_t * calibrate_current_obj = NULL;
static lv_obj_t * mode_current_obj = NULL;
static lv_obj_t * qdelay_current_obj = NULL;

static lv_obj_t * voltage0_current_obj = NULL;
static lv_obj_t * voltage1_current_obj = NULL;
static lv_obj_t * voltage2_current_obj = NULL;
static lv_obj_t * voltage3_current_obj = NULL;
static lv_obj_t * voltage4_current_obj = NULL;

static int select_wave;
static int select_voltage;
static int select_qdelay;
static int calibrate_step;
static compute_t cnv[5];
static int tmp_voltage;
static int tmp_qdelay;


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

static void event_clickable_toggle_all(int flag)
{
    if(flag)
    {
        event_clickable_toggle(ui_Grp_CalibrateReady, true);
        event_clickable_toggle(ui_Grp_Qdelay, true);
        event_clickable_toggle(ui_Grp_Table_Mode, true);
        event_clickable_toggle(ui_Grp_Table_Volt0, false);
        event_clickable_toggle(ui_Grp_Table_Volt1, false);
        event_clickable_toggle(ui_Grp_Table_Volt2, false);
        event_clickable_toggle(ui_Grp_Table_Volt3, false);
        event_clickable_toggle(ui_Grp_Table_Volt4, false);
        switch(select_wave)
        {
            case 0: event_clickable_toggle(ui_Grp_Table_Volt0, true); break;
            case 1: event_clickable_toggle(ui_Grp_Table_Volt1, true); break;
            case 2: event_clickable_toggle(ui_Grp_Table_Volt2, true); break;
            case 3: event_clickable_toggle(ui_Grp_Table_Volt3, true); break;
            case 4: event_clickable_toggle(ui_Grp_Table_Volt4, true); break;
        }

    }
    else
    {
        event_clickable_toggle(ui_Grp_Qdelay, false);
        event_clickable_toggle(ui_Grp_Table_Mode, false);
        event_clickable_toggle(ui_Grp_Table_Volt0, false);
        event_clickable_toggle(ui_Grp_Table_Volt1, false);
        event_clickable_toggle(ui_Grp_Table_Volt2, false);
        event_clickable_toggle(ui_Grp_Table_Volt3, false);
        event_clickable_toggle(ui_Grp_Table_Volt4, false);
    }
}

static void event_check_status(lv_obj_t * obj, int id)
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
    event_check_toggle(ui_Grp_Table_Volt0, false);
    event_check_toggle(ui_Grp_Table_Volt1, false);
    event_check_toggle(ui_Grp_Table_Volt2, false);
    event_check_toggle(ui_Grp_Table_Volt3, false);
    event_check_toggle(ui_Grp_Table_Volt4, false);
}

int convert_voltage(int type, int energy)
{
    if((energy < calibrate.energy[type][0]) || (energy > calibrate.energy[type][number_map[type]-1])){
        printf("error --->%d, %d\n", type, energy);
    } 
//        printf("\ntype->%d, energy->%d, min->%d, max->%d\n", type, energy, calibrate.energy[type][0], calibrate.energy[type][number_map[type]-1]);

    int i; double cal = 0;
// ** 주의 ** cal 계산시  전단계를([type][i-1]) 참조하기때문에 i=1 부터 시작하여야한다.
    for(i = 1; i < number_map[type] ; i++) if(energy <= calibrate.energy[type][i]) break;
//    printf("cal->%d, v1->%d, v2->%d, e1->%d, e2->%d\n", cal, calibrate.voltage[type][i], calibrate.voltage[type][i-1], calibrate.energy[type][i], calibrate.energy[type][i-1]);

    cal = (double)((double)(calibrate.voltage[type][i] - calibrate.voltage[type][i-1]) / (double)(calibrate.energy[type][i] - calibrate.energy[type][i-1]));
    int val = (int)(cal * (energy - calibrate.energy[type][i-1])) + calibrate.voltage[type][i-1];

    return val;
}

int calculate(int type, int energy)
{
    if((energy < cnv[type].energy[0]) || (energy > cnv[type].energy[6])) printf("error --->%d\n", energy);

    int i; double cal = 0;

    for(i = 0; i < 7 ; i++) if(energy <= cnv[type].energy[i]) break;

    cal = (double)((double)(cnv[type].voltage[i] - cnv[type].voltage[i-1]) / (double)(cnv[type].energy[i] - cnv[type].energy[i-1]));

    int val = (int)(cal * (energy - cnv[type].energy[i-1])) + cnv[type].voltage[i-1];

    return val;
}


int populate_table(int type)
{
    int i;

    for(i = 0; i < number_map[type] ; i++)
    {
        calibrate.energy[type][i] = energy_map[type][i];
        calibrate.voltage[type][i] = calculate(type, energy_map[type][i]);
        lv_label_set_text_fmt(*ui_table_voltage[type][i], "%d", calibrate.voltage[type][i]);
//        printf("energy->%d, voltage->%d\n", calibrate.energy[type][i], calibrate.voltage[type][i]);
    }
}

int populate_temp(int type)
{
    // 최대값에서 최소값을 뺀후 8단계로 나눈다.
    double quotient = (double)((cnv[type].voltage[6]-cnv[type].voltage[0]) / 8);

    cnv[type].energy[1] = 0;
    cnv[type].voltage[1] = cnv[type].voltage[0] + (quotient * 1);
    
    cnv[type].energy[2] = 0;
    cnv[type].voltage[2] = cnv[type].voltage[0] + (quotient * 2);
    
    cnv[type].energy[3] = 0;
    cnv[type].voltage[3] = cnv[type].voltage[0] + (quotient * 3);
    
    cnv[type].energy[4] = 0;
    // 532는 200mj까지 세밀한게 한다. 
    if(type == 3) cnv[type].voltage[4] = cnv[type].voltage[0] + (quotient * 4);
    else cnv[type].voltage[4] = cnv[type].voltage[0] + (quotient * 5);

    cnv[type].energy[5] = 0;
    cnv[type].voltage[5] = cnv[type].voltage[0] + (quotient * 6);
}

static void input_voltage(struct _lv_obj_t *obj, int lv)
{
    // 호출한 obj 저장
    calibrate_current_obj = obj;
    lv_label_set_text(ui_txtCalibrateEnergy, info_limit[select_wave][lv]);
    // 다른 obj click 금지
    //event_clickable_toggle_all(false);
}

static void input_energy(struct _lv_obj_t *obj, int lv)
{
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";
    // 호출한 obj 저장
    calibrate_current_obj = obj;
    // integer -> string 형 변환
    // populate_temp()에서 cnv[select_table_mode].voltage[lv] 값을 채워넣음.
    lv_snprintf(buf, sizeof(buf), "%d",  cnv[select_wave].voltage[lv]);
    lv_label_set_text(ui_txtCalibrateVoltage, buf);

    // 다른 obj click 금지
    //event_clickable_toggle_all(false);
}

const int tmp_map[5][8] = {
                                { 500, 1000, 300, 500, 800, 1200, 1250, 0} ,
                                { 500, 1000, 500, 800, 1000, 1600, 2000, 0 },
                                { 590, 1000, 700, 1000, 1300, 1900, 2300, 0 },
                                { 550, 900, 90, 150, 200, 300, 400, 0 },
                                { 590, 1000, 800, 1100, 1500, 2300, 2700, 0 },
                               };


static void disp_cali_ready(void)
{
    static int cal_old;

    if(cal_old != config.ready){
        _ui_state_modify(ui_btnCaliReady, LV_STATE_CHECKED, _UI_MODIFY_STATE_REMOVE);
        config.energy = atoi(lv_label_get_text(ui_txtCalibrateEnergy));
        config.voltage = atoi(lv_label_get_text(ui_txtCalibrateVoltage));
        config.ready = lv_obj_get_state(ui_btnCaliReady) % 2;
        if(config.ready > 1) config.ready = 0;
        cal_old = config.ready;
    printf("disp_standby-->%d\n", config.ready);
        send_txd(PACKET_COMMAND_STANDBY);
    }
}

static void calibrate_process(void)
{
    disp_cali_ready();
    switch(calibrate_step)
    {
        case 0: // step 0/7
            lv_label_set_text(ui_lblCalibrateStart, "Start");
            lv_label_set_text(ui_lblCalibrateBack, "-----");
            lv_label_set_text(ui_lblCalibrateInfo1, info1_text[calibrate_step]);
            lv_label_set_text(ui_txtCalibrateVoltage, "---");
            lv_label_set_text(ui_txtCalibrateEnergy, "---");
            lv_obj_clear_state(ui_txtCalibrateVoltage, LV_STATE_CHECKED);
            lv_obj_clear_state(ui_txtCalibrateEnergy, LV_STATE_CHECKED);
            break;

        case 1: // step 1/7
            lv_label_set_text(ui_lblCalibrateStart, "Next");
            lv_label_set_text(ui_lblCalibrateBack, "Previous");
            lv_label_set_text(ui_lblCalibrateInfo1, info2_text[calibrate_step]);
            lv_obj_add_state(ui_txtCalibrateVoltage, LV_STATE_CHECKED);
            lv_obj_clear_state(ui_txtCalibrateEnergy, LV_STATE_CHECKED);

            lv_label_set_text_fmt(ui_txtCalibrateVoltage, "%d", tmp_map[select_wave][0]);
            tmp_voltage = atoi(lv_label_get_text(ui_txtCalibrateVoltage));
            input_voltage(ui_txtCalibrateVoltage, 0);  // keypad load - input voltage mim
            break;

        case 2: // step 2/7
            lv_obj_add_state(ui_txtCalibrateVoltage, LV_STATE_CHECKED);
            lv_obj_clear_state(ui_txtCalibrateEnergy, LV_STATE_CHECKED);
            cnv[select_wave].voltage[0] = atoi(lv_label_get_text(ui_txtCalibrateVoltage));
            cnv[select_wave].energy[0] = atoi(info_limit[select_wave][0]);
            lv_label_set_text(ui_lblCalibrateInfo1, info2_text[calibrate_step]);

            lv_label_set_text_fmt(ui_txtCalibrateVoltage, "%d", tmp_map[select_wave][1]);
            tmp_voltage = atoi(lv_label_get_text(ui_txtCalibrateVoltage));
            input_voltage(ui_txtCalibrateVoltage, 1);  // keypad load - input voltage max
            break;

        case 3: // step 3/7
            lv_obj_clear_state(ui_txtCalibrateVoltage, LV_STATE_CHECKED);
            lv_obj_add_state(ui_txtCalibrateEnergy, LV_STATE_CHECKED);
            // load on keypad data    
            cnv[select_wave].voltage[6] = atoi(lv_label_get_text(ui_txtCalibrateVoltage));
            cnv[select_wave].energy[6] = atoi(info_limit[select_wave][1]);
            // Min Max 값을 가지고 voltage 5개 값을 만든다.
            populate_temp(select_wave);
            lv_label_set_text(ui_lblCalibrateInfo1, info2_text[calibrate_step]);

            lv_label_set_text_fmt(ui_txtCalibrateEnergy, "%d", tmp_map[select_wave][2]);
            tmp_voltage = atoi(lv_label_get_text(ui_txtCalibrateEnergy));
            input_energy(ui_txtCalibrateEnergy, 1);  // keypad load - input energy
            break;

        case 4:
            lv_obj_clear_state(ui_txtCalibrateVoltage, LV_STATE_CHECKED);
            lv_obj_add_state(ui_txtCalibrateEnergy, LV_STATE_CHECKED);
            // load on keypad data for  energy data
            cnv[select_wave].energy[1] = atoi(lv_label_get_text(ui_txtCalibrateEnergy));
            lv_label_set_text(ui_lblCalibrateInfo1, info2_text[calibrate_step]);
            lv_label_set_text_fmt(ui_txtCalibrateEnergy, "%d", tmp_map[select_wave][3]);
            tmp_voltage = atoi(lv_label_get_text(ui_txtCalibrateEnergy));
            input_energy(ui_txtCalibrateEnergy, 2);  // kepad load - input energy
            break;

        case 5:
            lv_obj_clear_state(ui_txtCalibrateVoltage, LV_STATE_CHECKED);
            lv_obj_add_state(ui_txtCalibrateEnergy, LV_STATE_CHECKED);
            // load on keypad data for  energy data
            cnv[select_wave].energy[2] = atoi(lv_label_get_text(ui_txtCalibrateEnergy));
            lv_label_set_text(ui_lblCalibrateInfo1, info2_text[calibrate_step]);
            lv_label_set_text_fmt(ui_txtCalibrateEnergy, "%d", tmp_map[select_wave][4]);
            tmp_voltage = atoi(lv_label_get_text(ui_txtCalibrateEnergy));
            input_energy(ui_txtCalibrateEnergy, 3);  // kepad load - input energy
            break;

        case 6:
            lv_obj_clear_state(ui_txtCalibrateVoltage, LV_STATE_CHECKED);
            lv_obj_add_state(ui_txtCalibrateEnergy, LV_STATE_CHECKED);
            // load on keypad data for  energy data
            cnv[select_wave].energy[3] = atoi(lv_label_get_text(ui_txtCalibrateEnergy));
            lv_label_set_text(ui_lblCalibrateInfo1, info2_text[calibrate_step]);
            lv_label_set_text_fmt(ui_txtCalibrateEnergy, "%d", tmp_map[select_wave][5]);
            tmp_voltage = atoi(lv_label_get_text(ui_txtCalibrateEnergy));
            input_energy(ui_txtCalibrateEnergy, 4);  // kepad load - input energy
            break;

        case 7:
            lv_obj_clear_state(ui_txtCalibrateVoltage, LV_STATE_CHECKED);
            lv_obj_add_state(ui_txtCalibrateEnergy, LV_STATE_CHECKED);
            // load on keypad data for  energy data
            cnv[select_wave].energy[4] = atoi(lv_label_get_text(ui_txtCalibrateEnergy));
            lv_label_set_text(ui_lblCalibrateInfo1, info2_text[calibrate_step]);
            lv_label_set_text_fmt(ui_txtCalibrateEnergy, "%d", tmp_map[select_wave][6]);
            tmp_voltage = atoi(lv_label_get_text(ui_txtCalibrateEnergy));
            input_energy(ui_txtCalibrateEnergy, 5);  // kepad load - input energy
            break;

        case 8:
            lv_obj_clear_state(ui_txtCalibrateVoltage, LV_STATE_CHECKED);
            lv_obj_clear_state(ui_txtCalibrateEnergy, LV_STATE_CHECKED);
            lv_label_set_text(ui_lblCalibrateStart, "Complete!");
            // load on keypad data for  energy data
            cnv[select_wave].energy[5] = atoi(lv_label_get_text(ui_txtCalibrateEnergy));
            // populate for energy table
            populate_table(select_wave);
            //for(int i=0;i<7;i++) printf("energy->%d, voltage->%d\n", cnv[select_wave].energy[i], cnv[select_wave].voltage[i]);
            //printf("\n\n");
            break;

        case 9:
            lv_label_set_text(ui_lblCalibrateStart, "-----");
            lv_label_set_text(ui_lblCalibrateBack, "-----");
            lv_obj_add_state(ui_txtCalibrateVoltage, LV_STATE_CHECKED);
            lv_obj_clear_state(ui_txtCalibrateEnergy, LV_STATE_CHECKED);
//            lv_label_set_text(ui_lblCalibrateStart, "Modify");
            lv_label_set_text(ui_lblCalibrateInfo1, info2_text[8]);
            lv_label_set_text_fmt(ui_txtCalibrateEnergy, "%d", calibrate.energy[select_wave][select_voltage]);
            lv_label_set_text_fmt(ui_txtCalibrateVoltage, "%d", calibrate.voltage[select_wave][select_voltage]);
            
            tmp_voltage = atoi(lv_label_get_text(ui_txtCalibrateVoltage));
            break;

    }

}




static void event_btnCalibrateBack(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        if((calibrate_step > 0) && (calibrate_step < 8)) calibrate_step--;
        calibrate_process();
    }
}

static void event_btnCalibrateStart(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        if(calibrate_step < 8) calibrate_step++;
        calibrate_process();
    }
}

static int value_chack(int value)
{
    int min, max;

    if(select_voltage == 0) min = 100;  // 그린파워 스펙
    else min = calibrate.voltage[select_wave][select_voltage - 1];

    if(select_voltage == number_map[select_wave] - 1) max = 1400;  // 그린파워 스펙
    else max = calibrate.voltage[select_wave][select_voltage + 1];

    if((value > min) && (value < max)){
        calibrate.voltage[select_wave][select_voltage] = value;
        lv_label_set_text_fmt(ui_txtCalibrateVoltage, "%d", value);
        lv_label_set_text_fmt(*ui_table_voltage[select_wave][select_voltage], "%d", value);
    }
    else{
        return 1;
    }
    return 0;
}

static int updn_voltage(int value)
{
    disp_cali_ready();
    switch(calibrate_step)
    {
        case 1: case 2: // calibrate voltage
            lv_label_set_text_fmt(ui_txtCalibrateVoltage, "%d", value);
            break;

        case 3: case 4: case 5: case 6: case 7:  // calibrate energy
            lv_label_set_text_fmt(ui_txtCalibrateEnergy, "%d", value);
            break;
        case 9: // modify voltage
            if(value_chack(value) == 1) return 1;
//            calibrate.voltage[select_wave][select_voltage] = value;
//            lv_label_set_text_fmt(ui_txtCalibrateVoltage, "%d", value);
//            lv_label_set_text_fmt(*ui_table_voltage[select_wave][select_voltage], "%d", value);
            break;
    }
    return 0;
}

static void event_btnCalibrateDown(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if((event_code == LV_EVENT_CLICKED) || (event_code == LV_EVENT_LONG_PRESSED_REPEAT)){
        if(updn_voltage(tmp_voltage - 1) == 0) tmp_voltage -= 1;
    }
}

static void event_btnCalibrateUp(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if((event_code == LV_EVENT_CLICKED) || (event_code == LV_EVENT_LONG_PRESSED_REPEAT)){
        if(updn_voltage(tmp_voltage + 1) == 0) tmp_voltage += 1;
    }
}

//////////////////////////////////////////////////////////////////////////////////////

static int updn_qdelay(struct _lv_obj_t *obj, int value)
{
    int min = 0;
    int max = 250;
    if(obj != NULL){
        disp_cali_ready();
        if((value >= min) && (value <= max)){
            calibrate.qdelay[select_qdelay] = value;
            lv_label_set_text_fmt(obj, "%d", value);
            printf("updn_qdelay(%d)(%d)\n", select_qdelay, calibrate.qdelay[select_qdelay]);
            return 0;
        }
    }
    return 1;
}

static void event_btnQdelayDown(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if((event_code == LV_EVENT_CLICKED) || (event_code == LV_EVENT_LONG_PRESSED_REPEAT)){
//        tmp_qdelay -= 1; 
        if(updn_qdelay(qdelay_current_obj, tmp_qdelay - 1) == 0) tmp_qdelay -= 1;
//        updn_qdelay(qdelay_current_obj, tmp_qdelay);
    }
}

static void event_btnQdelayUp(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if((event_code == LV_EVENT_CLICKED) || (event_code == LV_EVENT_LONG_PRESSED_REPEAT)){
//        tmp_qdelay += 1; 
        if(updn_qdelay(qdelay_current_obj, tmp_qdelay + 1) == 0) tmp_qdelay += 1;
//        updn_qdelay(qdelay_current_obj, tmp_qdelay);
    }
}

static void event_ddCalibrateFrequency_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        disp_cali_ready();
        printf("Option: %s\n", buf);
    }
}


static void event_tablemode_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        mode_current_obj = obj;

        lv_obj_t * parent = lv_obj_get_parent(obj);
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == mode_current_obj) {
                select_wave = i; // 오브젝트 등록순서에 따른다
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else {
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }
        event_check_status(ui_Grp_Table_Title, select_wave);
        event_clickable_toggle_all(true);

        calibrate_step = 0;
        calibrate_process();
        event_check_clear_all();
    }
}


///////////////////////////////////////////////////////////////////////////////////////
static void event_qdelay_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        qdelay_current_obj = obj;

        lv_obj_t * parent = lv_obj_get_parent(obj);
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == qdelay_current_obj) {
                select_qdelay = i; // 오브젝트 등록순서에 따른다
                tmp_qdelay = atoi(lv_label_get_text(qdelay_current_obj));
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else {
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }

    }
}

static void event_voltage0_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        voltage0_current_obj = obj;

        lv_obj_t * parent = lv_obj_get_parent(obj);
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == voltage0_current_obj) {
                select_voltage = i; // 오브젝트 등록순서에 따른다
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else {
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }
        calibrate_step = 9;
        calibrate_process();
    }
}


static void event_voltage1_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        voltage1_current_obj = obj;

        lv_obj_t * parent = lv_obj_get_parent(obj);
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == voltage1_current_obj) {
                select_voltage = i; // 오브젝트 등록순서에 따른다
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else {
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }
        calibrate_step = 9;
        calibrate_process();
    }
}

static void event_voltage2_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        voltage2_current_obj = obj;

        lv_obj_t * parent = lv_obj_get_parent(obj);
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == voltage2_current_obj) {
                select_voltage = i; // 오브젝트 등록순서에 따른다
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else {
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }
        calibrate_step = 9;
        calibrate_process();
    }
}

static void event_voltage3_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        voltage3_current_obj = obj;

        lv_obj_t * parent = lv_obj_get_parent(obj);
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == voltage3_current_obj) {
                select_voltage = i; // 오브젝트 등록순서에 따른다
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else {
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }
        calibrate_step = 9;
        calibrate_process();
    }
}

static void event_voltage4_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        voltage4_current_obj = obj;

        lv_obj_t * parent = lv_obj_get_parent(obj);
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == voltage4_current_obj) {
                select_voltage = i; // 오브젝트 등록순서에 따른다
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else {
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }
        calibrate_step = 9;
        calibrate_process();
    }
}


static void event_btnCalibrateOpen(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_ScreenProduct, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenProduct_screen_init);
        //_ui_screen_change(&ui_ScreenProduct, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 500, 0, &ui_ScreenProduct_screen_init);
    }
}

static void event_btnCalibrateExit(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_ScreenMain, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenMain_screen_init);
        //_ui_screen_change(&ui_ScreenMain, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, &ui_ScreenMain_screen_init);
    }
}



void trans_parameter(void)
{
    send_txd(PACKET_COMMAND_QDELAY);
    usleep(100000);
    send_txd(PACKET_COMMAND_V1064S);
    usleep(100000);
    send_txd(PACKET_COMMAND_V1064D);
    usleep(100000);
    send_txd(PACKET_COMMAND_V1064M);
    usleep(100000);
    send_txd(PACKET_COMMAND_V532);
    usleep(100000);
    send_txd(PACKET_COMMAND_VQUASI);
    usleep(100000);
}

static void event_btnCalibrateApply(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        make_calibrate_modify();
        trans_parameter();
        _ui_screen_change(&ui_ScreenMain, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenMain_screen_init);
        //_ui_screen_change( &ui_ScreenMain, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, &ui_ScreenMain_screen_init);
    }
}


void event_btnCaliReady( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_CLICKED) {
       
        config.frequency = lv_dropdown_get_selected(ui_ddCalibrateFrequency);
        config.wavelength = select_wave;
        //config.qdelay1 = 0;
        //config.qdelay2 = 0;
        //config.aiming = 0;
        config.energy = atoi(lv_label_get_text(ui_txtCalibrateEnergy));
        config.voltage = atoi(lv_label_get_text(ui_txtCalibrateVoltage));
        config.ready = lv_obj_get_state(ui_btnCaliReady) % 2;
        send_txd(PACKET_COMMAND_READY);
    }
}


void event_calibration_init(void)
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


    lv_obj_add_style(ui_btnQdelayDown, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnQdelayUp, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnCalibrateDown, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnCalibrateUp, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(ui_btnCaliReady, &style_pr, LV_STATE_PRESSED);

    calibrate_step = 10;
    calibrate_process();

    event_clickable_toggle(ui_Grp_MinMax, false);
//    event_clickable_toggle(ui_Grp_Qdelay, false);
    event_clickable_toggle(ui_Grp_CalibrateReady, false);
    event_clickable_toggle(ui_Grp_Table_Volt0, false);
    event_clickable_toggle(ui_Grp_Table_Volt1, false);
    event_clickable_toggle(ui_Grp_Table_Volt2, false);
    event_clickable_toggle(ui_Grp_Table_Volt3, false);
    event_clickable_toggle(ui_Grp_Table_Volt4, false);

  
    lv_dropdown_set_selected(ui_ddCalibrateFrequency, 2);
   
    lv_obj_add_event_cb(ui_ddCalibrateFrequency, event_ddCalibrateFrequency_handler, LV_EVENT_ALL, NULL);


    lv_obj_add_event_cb(ui_btnCaliReady, event_btnCaliReady, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnCalibrateBack, event_btnCalibrateBack, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnCalibrateStart, event_btnCalibrateStart, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_btnCalibrateDown, event_btnCalibrateDown, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnCalibrateUp, event_btnCalibrateUp, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnQdelayDown, event_btnQdelayDown, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnQdelayUp, event_btnQdelayUp, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_lblTableMode0, event_tablemode_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_lblTableMode1, event_tablemode_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_lblTableMode2, event_tablemode_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_lblTableMode3, event_tablemode_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_lblTableMode4, event_tablemode_handler, LV_EVENT_ALL, NULL);



/// Grp infomation
    for(int i = 0; i < 5 ; i++)
        for(int j = 0; j < number_map[i] ; j++)
        lv_label_set_text_fmt(*ui_table_voltage[i][j], "%d", calibrate.voltage[i][j]);

    for(int i = 0; i < 7 ; i++)
        lv_label_set_text_fmt(*ui_table_qdelay[i], "%d", calibrate.qdelay[i]);



    lv_obj_add_event_cb(ui_txt1064sQ1, event_qdelay_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064dQ1, event_qdelay_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064dQ2, event_qdelay_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMiinQ1High, event_qdelay_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMiinQ2High, event_qdelay_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMiinQ1Low, event_qdelay_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMiinQ2Low, event_qdelay_handler, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_txt1064sVolt0, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt1, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt2, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt3, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt4, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt5, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt6, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt7, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt8, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt9, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt10, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt11, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt12, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt13, event_voltage0_handler, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_txt1064sVolt0, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt1, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt2, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt3, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt4, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt5, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt6, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt7, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt8, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt9, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt10, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt11, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt12, event_voltage0_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064sVolt13, event_voltage0_handler, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_txt1064dVolt0, event_voltage1_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064dVolt1, event_voltage1_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064dVolt2, event_voltage1_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064dVolt3, event_voltage1_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064dVolt4, event_voltage1_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064dVolt5, event_voltage1_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064dVolt6, event_voltage1_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064dVolt7, event_voltage1_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064dVolt8, event_voltage1_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064dVolt9, event_voltage1_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064dVolt10, event_voltage1_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt1064dVolt11, event_voltage1_handler, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_txtMiinVolt0, event_voltage2_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMiinVolt1, event_voltage2_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMiinVolt2, event_voltage2_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMiinVolt3, event_voltage2_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMiinVolt4, event_voltage2_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMiinVolt5, event_voltage2_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMiinVolt6, event_voltage2_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMiinVolt7, event_voltage2_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMiinVolt8, event_voltage2_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMiinVolt9, event_voltage2_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMiinVolt10, event_voltage2_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMiinVolt11, event_voltage2_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtMiinVolt12, event_voltage2_handler, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_txt532Volt0, event_voltage3_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt532Volt1, event_voltage3_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt532Volt2, event_voltage3_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt532Volt3, event_voltage3_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt532Volt4, event_voltage3_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt532Volt5, event_voltage3_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt532Volt7, event_voltage3_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt532Volt6, event_voltage3_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt532Volt8, event_voltage3_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt532Volt9, event_voltage3_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt532Volt10, event_voltage3_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt532Volt11, event_voltage3_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt532Volt12, event_voltage3_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt532Volt13, event_voltage3_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txt532Volt14, event_voltage3_handler, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_txtQuasiVolt0, event_voltage4_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtQuasiVolt1, event_voltage4_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtQuasiVolt2, event_voltage4_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtQuasiVolt3, event_voltage4_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtQuasiVolt4, event_voltage4_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtQuasiVolt5, event_voltage4_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtQuasiVolt6, event_voltage4_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtQuasiVolt7, event_voltage4_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtQuasiVolt8, event_voltage4_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtQuasiVolt9, event_voltage4_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_txtQuasiVolt10, event_voltage4_handler, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_btnCalibrateOpen, event_btnCalibrateOpen, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnCalibrateExit, event_btnCalibrateExit, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnCalibrateApply, event_btnCalibrateApply, LV_EVENT_ALL, NULL);

}

