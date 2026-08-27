#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include "lv_port_disp.h"
#include "lvgl.h"
#include "port_conf.h"

/* Linux 标准 Framebuffer 全局变量 */
static int fbfd = -1;
static char *fbp = (char *)-1;
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;

/* LVGL 绘制缓冲区 */
static lv_color_t *draw_buf = NULL;

/* * 【最核心的刷屏函数】：把 LVGL 画好的像素，直接用 memcpy 拷进显存 
 */
static void fbdev_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p)
{
    if(fbp == (char *)-1 || 
       area->x2 < 0 || area->y2 < 0 || 
       area->x1 > (int32_t)vinfo.xres - 1 || area->y1 > (int32_t)vinfo.yres - 1) {
        lv_disp_flush_ready(drv);
        return;
    }

    /* 限制区域不超出屏幕边界 */
    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
    int32_t act_x2 = area->x2 > (int32_t)vinfo.xres - 1 ? (int32_t)vinfo.xres - 1 : area->x2;
    int32_t act_y2 = area->y2 > (int32_t)vinfo.yres - 1 ? (int32_t)vinfo.yres - 1 : area->y2;

    long int location = 0;
    int32_t y;
    uint32_t w = (act_x2 - act_x1 + 1);

    /* 按行将像素拷贝到 Framebuffer 映射的内存中 */
    for(y = act_y1; y <= act_y2; y++) {
        location = (act_x1 + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                   (y + vinfo.yoffset) * finfo.line_length;
        memcpy(fbp + location, (uint8_t *)color_p, w * (vinfo.bits_per_pixel / 8));
        color_p += w;
    }

    lv_disp_flush_ready(drv);
}

void lv_port_disp_init(bool is_disp_orientation)
{
    /* 1. 打开 Linux 标准 Framebuffer 设备 */
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        LV_LOG_ERROR("Error: cannot open /dev/fb0");
        return;
    }

    /* 2. 获取屏幕真实分辨率和位深 */
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        LV_LOG_ERROR("Error reading fixed information");
        return;
    }
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        LV_LOG_ERROR("Error reading variable information");
        return;
    }

    uint32_t width = vinfo.xres;
    uint32_t height = vinfo.yres;
    long int screensize = vinfo.yres_virtual * finfo.line_length;

    /* 3. 将显存映射到用户空间内存 */
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((intptr_t)fbp == -1) {
        LV_LOG_ERROR("Error: failed to map framebuffer device to memory");
        return;
    }

    /* 4. 使用标准 malloc 分配 LVGL 绘图缓冲区 (分配 1/4 屏幕大小，兼顾性能与内存) */
    uint32_t buf_size = width * height / 4;
    draw_buf = (lv_color_t *)malloc(buf_size * sizeof(lv_color_t));
    if (draw_buf == NULL) {
        LV_LOG_ERROR("malloc failed for draw_buf");
        return;
    }

    /* 5. 注册缓冲区 */
    static lv_disp_draw_buf_t draw_buf_dsc;                        
    lv_disp_draw_buf_init(&draw_buf_dsc, draw_buf, NULL, buf_size);   

    /* 6. 初始化并注册显示驱动 */
    static lv_disp_drv_t disp_drv;                    
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &draw_buf_dsc;
    disp_drv.flush_cb = fbdev_flush;   /* 使用我们刚写的标准刷屏函数 */
    disp_drv.hor_res = width;
    disp_drv.ver_res = height;

    /* 开启 LVGL 纯软件旋转功能 (替代全志的 G2D 硬件旋转) */
/* 【专治 7 寸横屏：关闭所有旋转，使用屏幕原生分辨率】 */
    disp_drv.rotated = LV_DISP_ROT_NONE;
    disp_drv.sw_rotate = 0;

    lv_disp_drv_register(&disp_drv);
}

void lv_port_disp_deinit(void)
{
    /* 释放资源，打扫战场 */
    if (draw_buf) {
        free(draw_buf);
        draw_buf = NULL;
    }
    if ((intptr_t)fbp != -1) {
        munmap(fbp, vinfo.yres_virtual * finfo.line_length);
        fbp = (char *)-1;
    }
    if (fbfd != -1) {
        close(fbfd);
        fbfd = -1;
    }
}
