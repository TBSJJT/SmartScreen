#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "lvgl.h"
#include "font_conf.h"
#include "page_conf.h"
#include "audio_player_async.h"

#include "wpa_manager.h"
#include "http_manager.h"

extern void lv_port_disp_init(bool is_disp_orientation);
extern void lv_port_indev_init(void);

void my_connect_status_callback_fun(WPA_WIFI_CONNECT_STATUS_E status)
{
    printf("connectstatus = %d\n ",status);
}

void my_wifi_status_callback_fun(WPA_WIFI_STATUS_E status)
{
    printf("wifi status = %d\n ",status);
}

void my_weather_callback_fun(char* str)
{
    printf("weather_callback_func = %s\n ",str);
}


int main() {
    //LVGL框架初始化
    lv_init();
    //LVGL显示屏幕初始化
    lv_port_disp_init(true);
    //LVGL输入设备初始化
    lv_port_indev_init();

    font_init();
    init_async_audio_player();
    init_page_main();
/*
    wpa_manager_open();    
    wpa_manager_add_callback(my_wifi_status_callback_fun,my_connect_status_callback_fun);
    wpa_ctrl_wifi_info_t wifi_info;
    memcpy(wifi_info.ssid,"iqooneo10",strlen("iqooneo10"));
    memcpy(wifi_info.psw,"88888888",strlen("88888888"));

    wpa_manager_wifi_connect(&wifi_info);
*/
    http_request_create();
    
    http_set_weather_callback(my_weather_callback_fun);

    http_get_weather_async("S8uW9BlypsENoMKBL","Yangling");
    //init_page_alarm();
    init_page_setting();
    while (1) {
        lv_task_handler();
        //延时，保证cpu占有率不会过高
        usleep(1000);
    }
    return 0;
}