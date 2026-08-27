#include "lvgl.h"
#include <stdio.h>

#include "font_conf.h"
#include "image_conf.h"

static lv_obj_t * label;
static int count = 0;

static void lv_event_cb_func(lv_event_t * e){
    printf("obj click\n");
}

static void obj_font_set(lv_obj_t *obj,int type, uint16_t weight){
    lv_font_t* font = get_font(type, weight);
    if(font != NULL)
        lv_obj_set_style_text_font(obj, font, 0);
}



static void btn_sw_event_cb_func(lv_event_t * e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    switch (code)
    {
    case LV_EVENT_VALUE_CHANGED:
        printf("State: %s\n", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
        break; 
    default:
        break;
    }
}

static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if (code == LV_EVENT_READY) {
        const char * text = lv_textarea_get_text(ta);
        if (text != NULL) {
            printf("current text: %s\n", text);
        } else {
            printf("current text: (empty)\n");
        }
    }
}

static void btn_click_event_cb_func(lv_event_t * e){
    printf("btn click\n");
    lv_obj_t * ta = lv_event_get_user_data(e);
    lv_event_send(ta, LV_EVENT_READY, NULL);//向ta发送LV_EVENT_READY事件
}

static void img_click_event_cb_func(lv_event_t * e){
    printf("img_click_event_cb_func click\n");
}

static void anim_cb(void * img, int32_t value) {
    //每次动画回调时，设置对象的角度
    lv_img_set_angle(img,value);
}

void anim_size_cb(void * obj, int32_t value)
{
//设置对象的大小
    lv_obj_set_size(obj, value, value);
}

void anim_x_cb(void * obj, int32_t value)
{
//设置对象的x坐标
    lv_obj_set_x(obj, value);
}


static void set_temp(void * bar, int32_t temp)
{
    lv_bar_set_value(bar, temp, LV_ANIM_ON);
    int value = (int)lv_bar_get_value(bar);
    lv_label_set_text_fmt(label,"%d%%",value);
}

static void set_angle(void * obj, int32_t v)
{
    lv_arc_set_value(obj, v);
    int32_t value = lv_arc_get_value(obj);
    lv_label_set_text_fmt(label, "%d%%", value);
}


static void event_handler(lv_event_t * e)
{
    lv_obj_t * roller = lv_event_get_target(e);
    int id = lv_roller_get_selected(roller);
    char buf[32];
    lv_roller_get_selected_str(roller, buf, sizeof(buf));
    printf("Selected id:%d day: %s\n", id, buf);
}

void timer_cb_func(lv_timer_t * timer)
{
    count ++;
    lv_label_set_text_fmt(label,"Count = %d",count);
}

#if 0
void lv_creat_child(lv_obj_t *obj_father)
{
    // 创建一个obj的子对象
    lv_obj_t *obj0_child = lv_obj_create(obj_father);
    //设置对象宽&高
    lv_obj_set_size(obj0_child,50,50);    
    lv_obj_set_pos(obj0_child, 0, 0); 
    lv_obj_set_style_bg_color(obj0_child ,lv_color_hex(0x123456),LV_PART_MAIN);

    lv_obj_t * obj0 = lv_obj_get_parent(obj0_child); //找到obj0_child的父对象obj0
    lv_obj_t * scr = lv_obj_get_parent(obj0); //找到obj0的父对象scr
    lv_obj_t * scr_child2 = lv_obj_get_child(scr, 1);//找到scr的第二个子对象scr_child

    lv_obj_set_style_bg_color(scr_child2 ,lv_color_hex(0xFF00FF),LV_PART_MAIN);
 
}
#endif

void lv_example_hello_world(void) {
    #if 0
 //定义并初始化样式
    static lv_style_t com_style;
    lv_style_init(&com_style);
    lv_style_set_bg_color(&com_style,lv_color_hex(0x0000FF));
    lv_style_set_pad_all(&com_style,0);
    lv_style_set_border_width(&com_style,0);
    lv_style_set_outline_width(&com_style,0);
//设置按下时的样式属性，例如放大尺寸
    lv_style_set_width(&com_style, 150);   // 将宽度设置为150像素
    lv_style_set_height(&com_style, 150);  // 将高度设置为150像素
    
    // 创建一个对象
    lv_obj_t *obj0 = lv_obj_create(lv_scr_act());
    //设置对象宽&高&偏移
    lv_obj_set_size(obj0,200,200);    
    lv_obj_set_pos(obj0, 412, 200); 

    lv_obj_center(obj0);//设置对象在父对象中心
    lv_obj_set_align(obj0, LV_ALIGN_TOP_LEFT);//参照其父对象左对齐
    lv_obj_align(obj0, LV_ALIGN_TOP_LEFT, 50, 0);//左对齐且x偏移50
    //lv_obj_set_style_radius(obj0 ,0,LV_PART_MAIN);//去圆角

    // 创建一个对象
    lv_obj_t *obj1 = lv_obj_create(lv_scr_act());
    //设置对象宽&高&偏移
    lv_obj_set_size(obj1,100,100);    
    lv_obj_set_pos(obj1, 512, 200); 
    //lv_obj_set_style_radius(obj1 ,0,LV_PART_MAIN);//去圆角

    lv_obj_align_to(obj0, obj1, LV_ALIGN_OUT_TOP_LEFT, 0, 0);//obj0在obj1外部左下
    //lv_creat_child(obj0);

    lv_obj_add_style(obj0, &com_style, LV_PART_MAIN);
    lv_obj_add_style(obj1, &com_style, LV_PART_MAIN);
    //设置对象未按下时和按下时背景不一样
    lv_obj_add_flag(obj0,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(obj0,lv_color_hex(0x000000),LV_PART_MAIN);
    lv_obj_set_style_bg_color(obj0,lv_color_hex(0xFF0000),LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj0, &com_style, LV_STATE_PRESSED); 
    #endif

    #if 0
    /*开启点击标志位使得对象可以被点击*/
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    //LV_PCT(100)为100%占满父窗口
    //lv_obj_set_size(cont ,LV_PCT(100),LV_PCT(100));
    lv_obj_set_size(cont ,200,200);//比父对象尺寸大
    //背景颜色
    lv_obj_set_style_bg_color(cont ,lv_color_hex(0x000000),LV_PART_MAIN);
    lv_obj_set_style_pad_all(cont,0,LV_PART_MAIN);
    lv_obj_set_style_border_width(cont,0,LV_PART_MAIN);
    lv_obj_set_style_outline_width(cont,0,LV_PART_MAIN);
    lv_obj_set_style_radius(cont,0,LV_PART_MAIN);


    /*Hide on object*/
    //lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);   //清除父对象的滚动标志位
    lv_obj_add_flag(cont,LV_OBJ_FLAG_CLICKABLE);//开启点击标志位使得对象可以被点击
    lv_obj_add_event_cb(cont,lv_event_cb_func,LV_EVENT_CLICKED,NULL);
    #endif

    #if 0
    // 创建标签对象
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_obj_t *label2 = lv_label_create(lv_scr_act());
    // 将标签居中对齐到屏幕
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_pos(label2, 0, 0); 
    obj_font_set(label,FONT_TYPE_CN, 60);
    lv_label_set_text(label,"初始化 \n ");

    obj_font_set(label2,FONT_TYPE_CN_LIGHT, 60);
    lv_label_set_text(label2, "你好世界");

    #endif

    #if 0
    /* 按钮 */
    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(btn,50,LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn,LV_OPA_0,LV_PART_MAIN);
    lv_obj_set_style_border_width(btn,2,LV_PART_MAIN);
    lv_obj_set_style_border_color(btn,lv_color_hex(0x0000ff),LV_PART_MAIN);
    lv_obj_clear_state(btn,LV_STATE_FOCUS_KEY);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Button");
    lv_label_set_recolor(label,true);
    lv_label_set_text(label,"#000000 Button#");
    lv_obj_center(label);
    
    lv_obj_add_event_cb(btn, btn_click_event_cb_func, LV_EVENT_ALL, NULL);

    #endif

    #if 0
    /* 开关 */
    lv_obj_t * sw = lv_switch_create(lv_scr_act());
    lv_obj_center(sw);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x00FF00), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_clear_state(sw,LV_STATE_FOCUS_KEY);
    lv_obj_add_event_cb(sw, btn_sw_event_cb_func, LV_EVENT_ALL, NULL);
    #endif

    #if 0
    /* 文本框 */
    lv_obj_t * ta = lv_textarea_create(lv_scr_act());//创建文本框
    lv_textarea_set_one_line(ta, true);//单行输入模式
    lv_obj_center(ta);//中心对齐
    lv_obj_set_size(ta,400,50);
    //设置背景颜色
    lv_obj_set_style_bg_color(ta,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
    //设置边框颜色
    lv_obj_set_style_border_color(ta,lv_color_hex(0x000000),LV_PART_MAIN);
    //设置文字颜色
    lv_obj_set_style_text_color(ta,lv_color_hex(0xFF0000),LV_PART_MAIN);
    //设置光标颜色
    lv_obj_set_style_border_color(ta,lv_color_hex(0x00FF00),LV_PART_CURSOR | LV_STATE_FOCUSED);

    lv_textarea_set_password_mode(ta, true);
    lv_textarea_set_placeholder_text(ta, "Enter The PassWord:");

    lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_READY, NULL);

    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn, btn_click_event_cb_func, LV_EVENT_CLICKED, ta);
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);
    //放在label设置后才能居中对齐
    lv_obj_align_to(btn, ta, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    #endif

    #if 0
    /* 键盘 */
    lv_obj_t * ta = lv_textarea_create(lv_scr_act());//创建文本框
    lv_textarea_set_one_line(ta, true);//单行输入模式
    lv_obj_center(ta);//中心对齐
    lv_obj_set_size(ta,400,50);
    lv_obj_set_pos(ta, 0, -100);

    lv_obj_t *kb = lv_keyboard_create(lv_scr_act());
    //设置键盘大小
    lv_obj_set_size(kb, 700, 280);
    //设置键盘主体背景
    lv_obj_set_style_bg_color(kb,lv_color_hex(0x000000),LV_PART_MAIN);
    //清除对象焦点状态
    lv_obj_clear_state(kb,LV_STATE_FOCUS_KEY);
    lv_keyboard_set_textarea(kb, ta);
    #endif

    #if 0
    /* 图片 */
    lv_obj_t * img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, GET_IMAGE_PATH("test.png"));
    lv_obj_center(img);
    lv_obj_add_flag(img,LV_OBJ_FLAG_CLICKABLE);//添加点击标志位
    lv_obj_add_event_cb(img,img_click_event_cb_func,LV_EVENT_CLICKED,NULL);//添加事件
    #endif

    #if 0
    /* 动画 */
    /* 1.创建对象 */
    lv_obj_t * circle = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(circle,lv_color_hex(0x0000ff),0);//blue
    lv_obj_set_style_radius(circle,LV_RADIUS_CIRCLE, LV_PART_MAIN);//圆角
    lv_obj_center(circle);//居中对齐
    /* 2.创建动画 */
    lv_anim_t action;
    lv_anim_init(&action);
    /* 3.绑定对象与动画 */
    lv_anim_set_var(&action,circle);
    //动画每轮运行时间
    lv_anim_set_time(&action, 1000);
    //动画恢复时间
    lv_anim_set_playback_time(&action, 300);
    //动画重复次数
    lv_anim_set_repeat_count(&action, LV_ANIM_REPEAT_INFINITE);
    //动画运动算法：开始和结束都很慢
    lv_anim_set_path_cb(&action, lv_anim_path_ease_in_out);

    //设置大小变化的回调函数 10-50
    lv_anim_set_exec_cb(&action,anim_size_cb);
    lv_anim_set_values(&action,10,50);
    lv_anim_start(&action);
    lv_anim_set_exec_cb(&action,anim_x_cb);
    lv_anim_set_values(&action,10,300);
    lv_anim_start(&action);
    #endif

    #if 0
    lv_obj_t * bar = lv_bar_create(lv_scr_act());
    lv_obj_set_size(bar, 200, 20);
    lv_obj_center(bar);
    lv_obj_set_style_bg_color(bar,lv_color_hex(0x00ff00),LV_PART_INDICATOR);


    label = lv_label_create(lv_scr_act());
    lv_obj_align_to(label,bar,LV_ALIGN_OUT_RIGHT_MID,10,0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, bar);
    lv_anim_set_values(&a, 0, 100);
    //动画时长
    lv_anim_set_time(&a, 2000);
    lv_anim_set_exec_cb(&a, set_temp);
    //只执行一次
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
    #endif

    #if 0
    lv_obj_t * arc = lv_arc_create(lv_scr_act());
    lv_arc_set_rotation(arc, 270);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);   
    lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE); 
    lv_obj_center(arc);

    label = lv_label_create(lv_scr_act());
    lv_obj_align_to(label,arc,LV_ALIGN_OUT_RIGHT_MID,10,0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, arc);
    lv_anim_set_exec_cb(&a, set_angle);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);   
    lv_anim_set_repeat_delay(&a, 500);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_start(&a);
    #endif

    #if 0
    滚轮
    lv_obj_t *roller1 = lv_roller_create(lv_scr_act());
    lv_roller_set_options(roller1,
                        "Monday\n"
                        "Tuesday\n"
                        "Wednesday\n"
                        "Thursday\n"
                        "Friday\n"
                        "Saturday\n"
                        "Sunday",
                        LV_ROLLER_MODE_INFINITE);
    lv_obj_center(roller1);
    lv_obj_add_event_cb(roller1, event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    #endif

    #if 0
    定时器
    label = lv_label_create(lv_scr_act());
    lv_obj_center(label);
    lv_label_set_text(label,"test");

    lv_timer_t * timer = lv_timer_create(timer_cb_func,1000,NULL);
    #endif
    
}


