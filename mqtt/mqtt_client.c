#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "client.h"
#include "queue.h"
#include "utils.h"

#include <mosquitto.h>
#include <pthread.h>

#if defined _WIN32 || defined _WIN64
#include <Windows.h>
#define strcasecmp stricmp
#define sleep(n) Sleep(n * 1000)
#else
#include <unistd.h>
#include <strings.h>
#endif

#include "cli.h"
#include "mqtt_client.h"

#include "../lv_ui/ui.h"

// ==========================================================================
// MQTT Config
// ==========================================================================

#define BROKER_ADDRESS "test.mosquitto.org"
#define BROKER_PORT 1883
#define KEEP_ALIVE 60
#define QOS 0
#define RETAIN_MESSAGE 1
#define TOPIC_MQTT "/testTo"
static const char *MESSAGE_MQTT = "this is a test!";

struct mosquitto *receiver_mqtt, *publisher_mqtt;

// ==========================================================================
// Prototypes
// ==========================================================================

void *publish_messages(void *args);

void on_connect_mqtt(struct mosquitto *mosq, void *obj, int result);

void on_message_mqtt(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);

// ==========================================================================
// Implementations
// ==========================================================================

void *publish_messages(void *args)
{
    while (1)
    {
        puts("Ltra Global IoT!");
        mosquitto_publish(publisher_mqtt, NULL, TOPIC_MQTT, strlen(MESSAGE_MQTT), MESSAGE_MQTT, QOS, RETAIN_MESSAGE);
        sleep(10);
    }
    return NULL;
}

void on_connect_mqtt(struct mosquitto *mosq, void *obj, int result)
{
    if (!result)
    {
        mosquitto_subscribe(mosq, NULL, TOPIC_MQTT, QOS);
    }
    else
    {
        fprintf(stderr, "%s\n", mosquitto_connack_string(result));
    }
}

void on_message_mqtt(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    printf("receive: %s\n", (char *)message->payload);
	if (!strcasecmp((char *)message->payload, "cali")) {
        printf("~~~~~~~~~~~~~~~~~~~ok!\n");
        _ui_screen_change(&ui_ScreenCalibration, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenCalibration_screen_init);
    }
	else if (!strcasecmp((char *)message->payload, "main")) {
        printf("~~~~~~~~~~~~~~~~~~~ok!\n");
        _ui_screen_change(&ui_ScreenMain, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenMain_screen_init);
    }
	else if (!strcasecmp((char *)message->payload, "load")) {
        printf("~~~~~~~~~~~~~~~~~~~ok!\n");
        _ui_screen_change(&ui_ScreenLoading, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenLoading_screen_init);
    }
	else if (!strcasecmp((char *)message->payload, "easy")) {
        printf("~~~~~~~~~~~~~~~~~~~ok!\n");
        _ui_screen_change(&ui_ScreenPreset, LV_SCR_LOAD_ANIM_NONE, 0, 0, &ui_ScreenPreset_screen_init);
    }
}

static void do_mqtt_client(void)
{
//    int    argc = args->argc;
//    char **argv = args->argv;

    mosquitto_lib_init();

    publisher_mqtt = mosquitto_new(NULL, true, NULL);
    receiver_mqtt = mosquitto_new(NULL, true, publisher_mqtt);

    mosquitto_connect_callback_set(receiver_mqtt, on_connect_mqtt);
    mosquitto_message_callback_set(receiver_mqtt, on_message_mqtt);

    mosquitto_connect(publisher_mqtt, BROKER_ADDRESS, BROKER_PORT, KEEP_ALIVE);
    mosquitto_connect(receiver_mqtt, BROKER_ADDRESS, BROKER_PORT, KEEP_ALIVE);

    mosquitto_loop_start(publisher_mqtt);

    pthread_t threads[2];
    pthread_create(&(threads[0]), NULL, &publish_messages, NULL);

    mosquitto_loop_forever(receiver_mqtt, -1, 1);

    mosquitto_destroy(receiver_mqtt);
    mosquitto_destroy(publisher_mqtt);
    mosquitto_lib_cleanup();

}

void init_mqtt_client()
{
//    cliAdd("mq", do_mqtt_client);
}