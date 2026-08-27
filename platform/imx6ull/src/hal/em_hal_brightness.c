#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "em_hal_brightness.h"

#define MAX_BRIGHTNESS_CMD_LEN 200
#define HARDWARE_MAX_BRIGHTNESS 8  // 硬件实际支持的最大亮度等级

void em_hal_brightness_set_value(int value)
{
    char cmd[MAX_BRIGHTNESS_CMD_LEN];
    
    // 1. 安全保护，防止 UI 传进来的值异常
    if (value < 0) value = 0;
    if (value > 100) value = 100;

    // 2. 数学映射：将 UI 的 0-100 映射到硬件的 0-8
    int brightness = (value * HARDWARE_MAX_BRIGHTNESS) / 100;

    if(brightness < 2)
        brightness = 2;
    
    printf("em_hal_brightness_set_value: UI=%d -> HW=%d\n", value, brightness);
    
    // 3. 执行系统命令写入节点
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "echo %d > /sys/class/backlight/backlight/brightness", brightness);
    system(cmd);
}

int em_hal_brightness_get_value()
{
    char command[MAX_BRIGHTNESS_CMD_LEN];
    char output[MAX_BRIGHTNESS_CMD_LEN];
    int brightness = 0;
    
    sprintf(command, "cat /sys/class/backlight/backlight/brightness");
    FILE* fp = popen(command, "r");
    if (fp == NULL) {
        return 0;
    }

    if(fgets(output, MAX_BRIGHTNESS_CMD_LEN, fp) != NULL){
        brightness = atoi(output);
        printf("em_hal_brightness_get_value: HW=%d\n", brightness);
    }
    pclose(fp);
    
    // 1. 安全保护
    if (brightness < 0) brightness = 0;
    if (brightness > HARDWARE_MAX_BRIGHTNESS) brightness = HARDWARE_MAX_BRIGHTNESS;

    // 2. 数学映射：将硬件的 0-8 映射回 UI 的 0-100
    int ui_value = (brightness * 100) / HARDWARE_MAX_BRIGHTNESS;
    return ui_value;
}
