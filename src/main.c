
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

#include "lvgl/lvgl.h"
#include "lv_drivers/display/sunxifb.h"
#include "lv_drivers/indev/evdev.h"

#include <ui_main.h>

#include "main.h"
#include "functions.h"
#include "ezini.h"
#include "config.h"
#include "uart.h"

#include "cli.h"
#include "fb2bmp.h"
#include "mqtt.h"
#include "mqtt_client.h"
#include "gpio.h"
#include "ntp.h"
#include "uart.h"
#include "sqlite.h"
#include "sound.h"


static lv_color_t *membuf;
const int volume[5] = {0, 1, 3, 5, 7};
static pthread_t _power_key_thread;



static void *power_key_thread(void *arg)
{
    lv_res_t res;
    int cnt = 0;
    int flag = 0;
    
	while(1) {
        res = read_power_switch();
        if(res == 1){
            if(flag++ > 3){
                printf("Bye...\n");
                ds1307_write();
                quit_system();
                //pthread_exit(0);
                //sleep(1);
                gpio_power_off();
                sleep(10);
                exit(0);       
                //return 0;
            } 
        }
        else flag = 0;
  
        usleep(100000); // 100 ms
	} // while
}


int main(int argc, char *argv[]) 
{
    lv_disp_drv_t disp_drv;
    lv_disp_draw_buf_t disp_buf;
    lv_indev_drv_t indev_drv;
    uint32_t rotated = LV_DISP_ROT_NONE;
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE] = "\0";
    int val = 0;
    int flag = 0;

    init_cli();
    init_mqtt();
    init_mqtt_client();
    init_gpio();
    init_configuration();
    init_uart();
//    init_sound();
    ds1307_read();

    test_sqlite();

    system("amixer set 'Headphone' unmute");
    snprintf(buf, sizeof(buf), "amixer set 'Headphone volume' %d &", volume[config.volume]);
    system(buf);

    lv_disp_drv_init(&disp_drv);
    lv_init();

    /*Linux frame buffer device init*/
    sunxifb_init(rotated);

    /*A buffer for LittlevGL to draw the screen's content*/
    static uint32_t width, height;
    sunxifb_get_sizes(&width, &height);

    membuf = (lv_color_t*) sunxifb_alloc(width * height * sizeof(lv_color_t), "lv_g2d_test");

    if (membuf == NULL) {
        sunxifb_exit();
        printf("malloc draw buffer fail\n");
        return 0;
    }

    /*Initialize a descriptor for the buffer*/
    lv_disp_draw_buf_init(&disp_buf, membuf, NULL, width * height);

    /*Initialize and register a display driver*/
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = sunxifb_flush;
    disp_drv.hor_res = width;
    disp_drv.ver_res = height;
    disp_drv.rotated = rotated;
    disp_drv.screen_transp = 0;
    lv_disp_drv_register(&disp_drv);

    evdev_init();
    lv_indev_drv_init(&indev_drv); /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER; /*See below.*/
    indev_drv.read_cb = evdev_read; /*See below.*/

    /*Register the driver in LVGL and save the created input device object*/
    lv_indev_t *evdev_indev = lv_indev_drv_register(&indev_drv);
    ui_init();

    pthread_create(&_power_key_thread, NULL, power_key_thread, NULL);

    /*Handle LitlevGL tasks (tickless mode)*/
    while (1) {
        //printf("switch->%d\n", val);
        lv_timer_handler();
        usleep(5 * 1000);
    }

    /*lv_timer_del(timer);*/
    /*sunxifb_free((void**) &membuf, "lv_g2d_test");*/
    /*sunxifb_exit();*/
    return 0;
}


/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if (start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}

void quit_system(void)
{
    //lv_timer_del(timer);
    close_uart();
    close_sound();
    //close_ui_main();
    sunxifb_free((void**) &membuf, "lv_g2d_test");
    sunxifb_exit();
}