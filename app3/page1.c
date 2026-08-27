#if 0
#include <stdio.h>
#include "lvgl.h"
#include "page_config.h"

static void btn_click_event_cb_func(lv_event_t * e){
    printf("page1 btn click\n");
    //获取活动屏幕对象
    lv_obj_t * act_scr = lv_scr_act();
    //获取显示屏幕对象
    lv_disp_t * d = lv_obj_get_disp(act_scr);
    //不在页面切换或者加载过程中才清除,避免页面触发过快，手指还停留着导致显示异常
    //页面加载完成后d->prev_scr会清空，d->scr_to_load会指向当前焦点界面
    if (d->prev_scr == NULL && (d->scr_to_load == NULL || d->scr_to_load == act_scr))
    {
        //则调用lv_obj_clean清掉lv_scr_act下所有对象
        lv_obj_clean(act_scr);
        page2_init();
    }
}

void page1_init()
{
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_obj_center(label);
    lv_label_set_text(label,"page 1");

    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btn, btn_click_event_cb_func, LV_EVENT_CLICKED, NULL);

    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Open page2");
    lv_obj_center(btn_label);

    lv_obj_align_to(btn,label,LV_ALIGN_OUT_BOTTOM_MID,0,20);

}
#endif 



#include <stdio.h>
#include "lvgl.h"
#include "image_conf.h"
#include "font_conf.h"

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
}

//封装字库获取函数
static void obj_font_set(lv_obj_t *obj,int type, uint16_t weight){
    lv_font_t* font = get_font(type, weight);//获取字库
    if(font != NULL)
        lv_obj_set_style_text_font(obj, font, LV_PART_MAIN);
}

/**
 * @brief 初始化一个带图标和文本的列表项视图
 * @param parent 父对象（容器将被添加到此对象下）
 * @param src    图标的图片源路径（例如 "S:/icon.png"）
 * @param str    显示的文本字符串
 * @return       返回创建好的容器对象（可用于后续布局或事件绑定）
 */
static lv_obj_t * init_item_view(lv_obj_t *parent, const char *src, const char *str)
{
    // 创建一个容器对象，作为图标准备的父级
    lv_obj_t * cont = lv_obj_create(parent);
    // 设置容器大小为内容自适应（宽度和高度根据内部子对象自动调整）
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    // 为容器添加自定义样式（com_style 需在外部定义），作用于主部分
    lv_obj_add_style(cont, &com_style, LV_PART_MAIN);

    // 创建图片对象（图标），容器作为其父对象
    lv_obj_t * icon = lv_img_create(cont);
    // 设置图片源
    lv_img_set_src(icon, src);
    // 将图标对齐到容器的左侧中间位置（不偏移）
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 0, 0);

    // 创建标签对象（文本）
    lv_obj_t * label = lv_label_create(cont);
    // 设置标签字体（自定义函数，FONT_TYPE_CN 表示中文字体，字号 20）
    obj_font_set(label, FONT_TYPE_CN, 20);
    // 设置标签文字颜色为白色（0xffffff）
    lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), 0);
    // 设置标签文本内容
    lv_label_set_text(label, str);
    // 将标签对齐到图标的右侧外部中间，X 方向偏移 14 像素，Y 方向向上偏移 5 像素
    lv_obj_align_to(label, icon, LV_ALIGN_OUT_RIGHT_MID, 14, -5);

    // 返回容器对象，以便外部进一步使用（如添加到列表、设置事件等）
    return cont;
}

static lv_obj_t * init_info_view(lv_obj_t *parent){
    //创建对象作为容器
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_add_style(cont, &com_style, LV_PART_MAIN);
    //设置线性布局，排序方式为列排序
    lv_obj_set_flex_flow(cont,LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont,LV_FLEX_ALIGN_CENTER,LV_FLEX_ALIGN_START,LV_FLEX_ALIGN_CENTER);

    init_item_view(cont,GET_IMAGE_PATH("test.png"),"测试文本");
    init_item_view(cont,GET_IMAGE_PATH("test.png"),"test-txt");
    init_item_view(cont,GET_IMAGE_PATH("test.png"),"ceshiwenben.com");
    
    return cont;
}

static lv_obj_t * init_project_view(lv_obj_t *parent){
    //创建对象作为容器
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_add_style(cont, &com_style, LV_PART_MAIN);
    //设置线性布局，排序方式为列排序
    lv_obj_set_flex_flow(cont,LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont,LV_FLEX_ALIGN_CENTER,LV_FLEX_ALIGN_CENTER,LV_FLEX_ALIGN_CENTER);

    lv_obj_t * project1 = lv_img_create(cont);
    lv_img_set_src(project1,GET_IMAGE_PATH("test.png"));
    lv_obj_t * project2 = lv_img_create(cont);
    lv_img_set_src(project2,GET_IMAGE_PATH("test.png"));
    lv_obj_t * project3 = lv_img_create(cont);
    lv_img_set_src(project3,GET_IMAGE_PATH("test.png"));
    return cont;
}

static lv_obj_t * init_select_btn(lv_obj_t *parent){
    //初始化按钮控件
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_add_style(btn,&com_style,LV_PART_MAIN);
    //设置按钮大小
    lv_obj_set_size(btn,171,66);
    //清除焦点状态
    lv_obj_clear_state(btn,LV_STATE_FOCUS_KEY);
    //设置边框、阴影为0
    lv_obj_set_style_border_width(btn, 0,LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0,LV_PART_MAIN);
    //设置圆角为35
    lv_obj_set_style_radius(btn,35,LV_PART_MAIN);
    //设置背景颜色为蓝色
    lv_obj_set_style_bg_color(btn,lv_color_hex(0x1F94D2),0);
    //初始化按钮显示文字
    lv_obj_t  * btn_label = lv_label_create(btn);
    obj_font_set(btn_label,FONT_TYPE_CN, 24);
    lv_obj_set_style_text_color(btn_label,lv_color_hex(0xffffff),0);
    lv_label_set_text(btn_label,"设置");
    //进行偏移对齐
    lv_obj_align(btn_label,LV_ALIGN_CENTER,0,-5);
    return btn;
}

void page1_init()
{
    com_style_init();
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont ,LV_PCT(100),LV_PCT(100));
    lv_obj_set_style_bg_color(cont ,lv_color_hex(0x000000),LV_PART_MAIN);
    lv_obj_set_style_pad_all(cont,0,LV_PART_MAIN);
    lv_obj_set_style_border_width(cont,0,LV_PART_MAIN);
    lv_obj_set_style_outline_width(cont,0,LV_PART_MAIN);
    lv_obj_set_style_radius(cont,0,LV_PART_MAIN);

    lv_obj_t * img_bg = lv_img_create(cont);
    lv_img_set_src(img_bg,GET_IMAGE_PATH("test.png"));
    lv_obj_align(img_bg,LV_ALIGN_LEFT_MID,50,0);

    lv_obj_t * img_user = lv_img_create(cont);
    lv_img_set_src(img_user,GET_IMAGE_PATH("test.png"));
    lv_obj_align_to(img_user,img_bg,LV_ALIGN_OUT_RIGHT_MID,70,0);

    lv_obj_t * info_view = init_info_view(cont);
    lv_obj_align_to(info_view,img_user,LV_ALIGN_OUT_RIGHT_MID,85,0);

    lv_obj_t * project_name = init_item_view(cont,GET_IMAGE_PATH("test.png"),"我的作品");
    lv_obj_align_to(project_name,info_view,LV_ALIGN_OUT_RIGHT_MID,50,0);

    lv_obj_t * project_view = init_project_view(cont);
    lv_obj_align_to(project_view,project_name,LV_ALIGN_OUT_RIGHT_MID,40,0);

    lv_obj_t * btn = init_select_btn(cont);
    lv_obj_align_to(btn,project_view,LV_ALIGN_OUT_RIGHT_MID,40,0);

}