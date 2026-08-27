#include <stdio.h>
#include "lvgl.h"
#include "image_conf.h"
#include "font_conf.h"
#include "page_conf.h"
#include <time.h>

static lv_obj_t *time_label;
//声明通用样式
static lv_style_t com_style;
//初始化通用样式
static void com_style_init(){
    //初始化样式
    lv_style_init(&com_style);
    //判断如果样式非空，那就先重置，再设置
    if(lv_style_is_empty(&com_style) == false)
        lv_style_reset(&com_style);
    //样式背景设置为黑色，圆角设置为0，边框宽度设置为0，填充区域设置为0
    lv_style_set_bg_color(&com_style,lv_color_hex(0x000000));
    lv_style_set_radius(&com_style,0);
    lv_style_set_border_width(&com_style,0);
    lv_style_set_pad_all(&com_style,0);
    lv_style_set_outline_width(&com_style,0);
}

//封装字库获取函数
static void obj_font_set(lv_obj_t *obj,int type, uint16_t weight){
    lv_font_t* font = get_font(type, weight);
    if(font != NULL)
        lv_obj_set_style_text_font(obj, font, LV_PART_MAIN);
}

/*
static lv_obj_t * init_info_view(lv_obj_t *parent){
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont,LV_SIZE_CONTENT,LV_SIZE_CONTENT);
    lv_obj_add_style(cont,&com_style,LV_PART_MAIN);

    time_label = lv_label_create(cont);
    obj_font_set(time_label,FONT_TYPE_CN,60);
    lv_label_set_text(time_label, "21:26");
    lv_obj_set_style_text_color(time_label,lv_color_hex(0xffffff),LV_PART_MAIN);
    lv_obj_align(time_label,LV_ALIGN_TOP_MID,0,0);

    lv_obj_t *weather_label = lv_label_create(cont);
    obj_font_set(weather_label,FONT_TYPE_CN,24);
    lv_label_set_text(weather_label, "广州 多云 25°C");
    lv_obj_set_style_text_color(weather_label,lv_color_hex(0xffffff),LV_PART_MAIN);
    lv_obj_align_to(weather_label,time_label,LV_ALIGN_OUT_BOTTOM_MID,0,10);

    return cont;
}
*/

 static lv_obj_t * init_info_view(lv_obj_t *parent){
      lv_obj_t * cont = lv_obj_create(parent);
      lv_obj_set_size(cont,LV_SIZE_CONTENT,LV_SIZE_CONTENT);
      lv_obj_add_style(cont,&com_style,LV_PART_MAIN);
      lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);                         // ← 新增
      lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);  // ← 新增

      time_label = lv_label_create(cont);
      obj_font_set(time_label,FONT_TYPE_CN,60);
      lv_label_set_text(time_label, "21:26");
      lv_obj_set_style_text_color(time_label,lv_color_hex(0xffffff),LV_PART_MAIN);
      // ← 删除 lv_obj_align(time_label,LV_ALIGN_TOP_MID,0,0)

      lv_obj_t *weather_label = lv_label_create(cont);
      obj_font_set(weather_label,FONT_TYPE_CN,24);
      lv_label_set_text(weather_label, "杨凌 多云 25°C");
      lv_obj_set_style_text_color(weather_label,lv_color_hex(0xffffff),LV_PART_MAIN);
      lv_obj_set_style_pad_top(weather_label,10,LV_PART_MAIN);                 // ← 替代原来的 lv_obj_align_to

      return cont;
  }

void timer_cb_func(lv_timer_t * timer)
{
    time_t seconds;
    time(&seconds);
    printf("Seconds since Epoch: %ld\n", seconds);
    struct tm *timeinfo;
    timeinfo = localtime(&seconds);
    printf("%d-%02d-%02d %02d:%02d:%02d\n",
           timeinfo->tm_year + 1900, // 需要加上 1900
           timeinfo->tm_mon + 1,     // 需要加上 1
           timeinfo->tm_mday,
           timeinfo->tm_hour,
           timeinfo->tm_min,
           timeinfo->tm_sec);
    lv_label_set_text_fmt(time_label,"%02d:%02d",timeinfo->tm_hour,timeinfo->tm_min);
}

static void init_timer(void) {
    lv_timer_t * timer = lv_timer_create(timer_cb_func, 1000,  NULL);
}

static void lv_event_cb_func(lv_event_t * e){
    char *str = (char *)lv_event_get_user_data(e);
    printf("%s click\n",str);
}

static void init_item(lv_obj_t *parent,const char * img_url,const char *str){
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont,LV_SIZE_CONTENT,LV_SIZE_CONTENT);
    lv_obj_add_style(cont,&com_style,LV_PART_MAIN);

    lv_obj_t * img = lv_img_create(cont);
    lv_img_set_src(img, img_url);
    lv_obj_align(img,LV_ALIGN_TOP_MID,0,0);

    lv_obj_t *label = lv_label_create(cont);
    obj_font_set(label,FONT_TYPE_CN,20);
    lv_label_set_text(label, str);
    lv_obj_align_to(label,img,LV_ALIGN_OUT_BOTTOM_MID,0,7);
    lv_obj_set_style_text_color(label,lv_color_hex(0xffffff),LV_PART_MAIN);

    lv_obj_add_flag(cont,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(cont,lv_event_cb_func,LV_EVENT_CLICKED,lv_label_get_text(label));
}

static lv_obj_t * init_item_view(lv_obj_t *parent){
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont,LV_PCT(100),LV_SIZE_CONTENT);
    lv_obj_add_style(cont,&com_style,LV_PART_MAIN);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);//设置 Flex 布局：水平排列不换行
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);//无论内容是否超出容器，滚动条都永远不会显示。
    
    init_item(cont,GET_IMAGE_PATH("icon_menu_tomato_time.png"),"番茄时钟");
    init_item(cont,GET_IMAGE_PATH("icon_menu_time.png"),"闹钟设置");
    init_item(cont,GET_IMAGE_PATH("icon_menu_dial.png"),"表盘设置");
    init_item(cont,GET_IMAGE_PATH("icon_stm32cubemx.png"),"快捷键");
    init_item(cont,GET_IMAGE_PATH("icon_menu_city.png"),"城市设置");
    init_item(cont,GET_IMAGE_PATH("icon_language.png"),"小游戏");
    init_item(cont,GET_IMAGE_PATH("icon_menu_tomato_time.png"),"番茄时钟");

    return cont;
}

void init_page_main(){
    com_style_init();//通用样式初始化
    lv_obj_t * cont = lv_obj_create(lv_scr_act());//创建容器cont其父对象是活动屏幕
    lv_obj_set_size(cont,LV_PCT(100),LV_PCT(100));//设置cont的大小占满活动屏幕的100%
    lv_obj_add_style(cont,&com_style,LV_PART_MAIN);//添加样式

    lv_obj_t * img = lv_img_create(cont);//创建图片
    lv_img_set_src(img, GET_IMAGE_PATH("icon_user1.png"));//设置图片
    lv_obj_align(img,LV_ALIGN_TOP_LEFT,30,30);//图片对齐cont的左上角

    lv_obj_t * info_view = init_info_view(cont);//时间和天气信息
    lv_obj_align_to(info_view,img,LV_ALIGN_OUT_RIGHT_MID,60,0);//设置控件位置

    lv_obj_t * item_view = init_item_view(cont);
    lv_obj_align_to(item_view,info_view,LV_ALIGN_OUT_RIGHT_MID,100,20);

    init_timer();

}
