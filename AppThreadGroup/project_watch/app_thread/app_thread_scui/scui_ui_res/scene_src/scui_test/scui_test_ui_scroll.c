/*实现目标:
 *    测试(widget scroll)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t scroll;       /* 滚动控件句柄 */
    uint8_t       build_idx;    /* 构建配置索引 */
} * scui_ui_res_local = NULL;

/*@brief 垂直滚动控件事件回调(打印滚动百分比)
 *@param event 事件
 */
void scui_test_ui_scroll_ver_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_scroll_layout:
    case scui_event_scroll_start:
    case scui_event_scroll_keep:
    case scui_event_scroll_over: {
        scui_coord_t scroll_pct = 0;
        scui_scroll_percent_get(event->object, &scroll_pct);
        SCUI_LOG_WARN("scroll ver percent:%d", scroll_pct);
        break;
    }
    default:
        break;
    }
}

/*@brief 纯色自定义子控件事件回调(点击播放淡出缩放动画)
 *@param event 事件
 */
void scui_test_ui_scroll_custom_anima_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_ptr_click: {
        scui_event_mask_over(event);
        
        /* 点击子控件播放两级动画: 淡出 + 水平缩放 */
        scui_widget_anima_create(event->object, 2);
        scui_handle_t anima1 = scui_widget_anima_fade_out(event->object, 300, 0);
        scui_handle_t anima2 = scui_widget_anima_zoom_out_h(event->object, 200, 50);
        scui_widget_anima_submit(event->object, anima1, 1); /* 第一步 */
        scui_widget_anima_submit(event->object, anima2, 2); /* 第二步 */
        scui_widget_anima_start(event->object);
        break;
    }
    default:
        break;
    }
}

/*@brief 纯色自定义子控件构造基座(随机背景颜色)
 *@param parent 父控件句柄
 *@retval 构造器
 */
static scui_custom_maker_t scui_test_ui_scroll_custom_base(scui_handle_t parent)
{
    scui_custom_maker_define(custom_maker);
    
    custom_maker.widget.style.fully_bg = true;
    custom_maker.widget.parent = parent;
    custom_maker.widget.color.color.ch.a = 0xFF;
    custom_maker.widget.color.color.ch.r = scui_rand(10) % 2 == 0 ? 0 : 0xFF;
    custom_maker.widget.color.color.ch.g = scui_rand(10) % 2 == 0 ? 0 : 0xFF;
    custom_maker.widget.color.color.ch.b = scui_rand(10) % 2 == 0 ? 0 : 0xFF;
    return custom_maker;
}

/*@brief 构建配置0: 垂直滚动(循环,翻1页,编码器/按键滚动一个条目)
 */
static void scui_test_ui_scroll_create_ver(void)
{
    scui_scroll_maker_define(scroll_maker);
    scui_handle_t scroll_handle = SCUI_HANDLE_INVALID;
    
    scroll_maker.widget.style.fully_bg     = true;
    scroll_maker.widget.style.sched_widget = true;
    scroll_maker.widget.clip.x = (SCUI_HOR_RES - SCUI_HOR_RES * 707 / 1000) / 2;
    scroll_maker.widget.clip.y = (SCUI_VER_RES - SCUI_VER_RES * 707 / 1000) / 2;
    scroll_maker.widget.clip.w = SCUI_HOR_RES * 707 / 1000;
    scroll_maker.widget.clip.h = SCUI_VER_RES * 707 / 1000;
    scroll_maker.widget.parent    = SCUI_UI_SCENE_TEST_UI_SCROLL;
    scroll_maker.widget.child_num = 20;
    scroll_maker.widget.color.color.full = 0xFF6F6F6F;   /* 亮灰 */
    scroll_maker.widget.event_cb = scui_test_ui_scroll_ver_event_proc;
    scroll_maker.dir          = scui_opt_dir_ver;
    scroll_maker.space        = 10;
    scroll_maker.loop         = true;                  /* 循环 */
    scroll_maker.fling_page   = 1;                     /* 翻1页 */
    scroll_maker.route_enc    = SCUI_HOR_RES / 6 + 10; /* 编码器滚动一个条目 */
    scroll_maker.route_key    = SCUI_HOR_RES / 6 + 10; /* 按键滚动一个条目 */
    scroll_maker.skip         = scui_opt_pos_all;
    
    scui_widget_create(&scroll_maker, &scroll_handle);
    scui_ui_res_local->scroll = scroll_handle;
    
    /* 子控件: 纯色 custom(随机颜色, 点击动画) */
    for (uint8_t idx = 0; idx < 10; idx++) {
        scui_custom_maker_t custom_maker = scui_test_ui_scroll_custom_base(scroll_handle);
        scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
        custom_maker.widget.style.indev_ptr = true;
        custom_maker.widget.event_cb  = scui_test_ui_scroll_custom_anima_event_proc;
        custom_maker.widget.clip.w = SCUI_HOR_RES * 707 / 1000;
        custom_maker.widget.clip.h = SCUI_HOR_RES / 6;
        scui_widget_create(&custom_maker, &custom_handle);
    }
}

/*@brief 构建配置1: 水平滚动(无回弹,翻1条目,编码器/按键滚动一个条目)
 */
static void scui_test_ui_scroll_create_hor(void)
{
    scui_scroll_maker_define(scroll_maker);
    scui_handle_t scroll_handle = SCUI_HANDLE_INVALID;
    
    scroll_maker.widget.style.fully_bg     = true;
    scroll_maker.widget.style.sched_widget = true;
    scroll_maker.widget.clip.x = (SCUI_HOR_RES - SCUI_HOR_RES * 707 / 1000) / 2;
    scroll_maker.widget.clip.y = (SCUI_VER_RES - SCUI_VER_RES * 707 / 1000) / 2;
    scroll_maker.widget.clip.w = SCUI_HOR_RES * 707 / 1000;
    scroll_maker.widget.clip.h = SCUI_VER_RES * 707 / 1000;
    scroll_maker.widget.parent    = SCUI_UI_SCENE_TEST_UI_SCROLL;
    scroll_maker.widget.child_num = 10;
    scroll_maker.widget.color.color.full = 0xFF6F6F6F;   /* 亮灰 */
    scroll_maker.dir          = scui_opt_dir_hor;
    scroll_maker.space        = 10;
    scroll_maker.springback   = 0;                  /* 无回弹 */
    scroll_maker.fling_page   = 1;                  /* 翻1条目 */
    scroll_maker.route_enc    = SCUI_HOR_RES / 4 + 10;    /* 编码器滚动一个条目 */
    scroll_maker.route_key    = SCUI_HOR_RES / 4 + 10;    /* 按键滚动一个条目 */
    scroll_maker.skip         = scui_opt_pos_all;
    
    scui_widget_create(&scroll_maker, &scroll_handle);
    scui_ui_res_local->scroll = scroll_handle;
    
    /* 子控件: 纯色 custom(随机颜色) */
    for (uint8_t idx = 0; idx < 8; idx++) {
        scui_custom_maker_t custom_maker = scui_test_ui_scroll_custom_base(scroll_handle);
        scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
        custom_maker.widget.clip.w = SCUI_HOR_RES / 4;
        custom_maker.widget.clip.h = SCUI_VER_RES * 707 / 1000;
        scui_widget_create(&custom_maker, &custom_handle);
    }
}

/*@brief 构建配置2: 自由滚动(子控件为小正方形,有间隔)
 */
static void scui_test_ui_scroll_create_free(void)
{
    scui_scroll_maker_define(scroll_maker);
    scui_handle_t scroll_handle = SCUI_HANDLE_INVALID;
    scui_coord_t        scroll_w = SCUI_HOR_RES * 707 / 1000;
    scui_coord_t        scroll_h = SCUI_VER_RES * 707 / 1000;
    scui_coord_t        custom_w = SCUI_HOR_RES / 8;
    scui_coord_t        custom_h = SCUI_VER_RES / 8;
    
    scroll_maker.widget.style.fully_bg     = true;
    scroll_maker.widget.style.sched_widget = true;
    scroll_maker.widget.clip.x = (SCUI_HOR_RES - scroll_w) / 2;
    scroll_maker.widget.clip.y = (SCUI_VER_RES - scroll_h) / 2;
    scroll_maker.widget.clip.w = scroll_w;
    scroll_maker.widget.clip.h = scroll_h;
    scroll_maker.widget.parent    = SCUI_UI_SCENE_TEST_UI_SCROLL;
    scroll_maker.widget.child_num = 30;
    scroll_maker.widget.color.color.full = 0xFF6F6F6F;   /* 亮灰 */
    scroll_maker.dir          = scui_opt_dir_all;    /* 自由布局(全局滚动) */
    scroll_maker.freedom      = true;
    scroll_maker.edge.x       = custom_w / 2;        /* 滚动边距(间隔) */
    scroll_maker.edge.y       = custom_h / 2;
    scroll_maker.skip         = scui_opt_pos_all;
    
    scui_widget_create(&scroll_maker, &scroll_handle);
    scui_ui_res_local->scroll = scroll_handle;
    
    /* 子控件: 小正方形(随机颜色), 网格摆放有间隔 */
    for (uint8_t idx = 0; idx < 25; idx++) {
        scui_custom_maker_t custom_maker = scui_test_ui_scroll_custom_base(scroll_handle);
        scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
        custom_maker.widget.clip.x = scui_map(idx % 5, 0, 5, - custom_w, scroll_w + custom_w);
        custom_maker.widget.clip.y = scui_map(idx / 5, 0, 5, - custom_h, scroll_h + custom_h);
        custom_maker.widget.clip.w = custom_w;
        custom_maker.widget.clip.h = custom_h;
        scui_widget_create(&custom_maker, &custom_handle);
    }
}

/*@brief 构建配置3: 滚动套滚动(垂直滚动内嵌水平滚动,子控件宽度不一,边缘对齐粘连)
 */
static void scui_test_ui_scroll_create_nest(void)
{
    scui_scroll_maker_define(scroll_maker);
    scui_handle_t scroll_handle = SCUI_HANDLE_INVALID;
    scui_coord_t        scroll_w = SCUI_HOR_RES * 707 / 1000;
    scui_coord_t        item_h = SCUI_HOR_RES / 6;
    
    scroll_maker.widget.style.fully_bg     = true;
    scroll_maker.widget.style.sched_widget = true;
    scroll_maker.widget.clip.x = (SCUI_HOR_RES - scroll_w) / 2;
    scroll_maker.widget.clip.y = (SCUI_VER_RES - SCUI_VER_RES * 707 / 1000) / 2;
    scroll_maker.widget.clip.w = scroll_w;
    scroll_maker.widget.clip.h = SCUI_VER_RES * 707 / 1000;
    scroll_maker.widget.parent    = SCUI_UI_SCENE_TEST_UI_SCROLL;
    scroll_maker.widget.child_num = 10;
    scroll_maker.widget.color.color.full = 0xFF6F6F6F;   /* 亮灰 */
    scroll_maker.dir          = scui_opt_dir_ver;
    scroll_maker.space        = 10;
    scroll_maker.springback   = 70;
    scroll_maker.fling_page   = 1;
    scroll_maker.route_enc    = item_h + 10;         /* 编码器滚动一个条目 */
    scroll_maker.route_key    = item_h + 10;         /* 按键滚动一个条目 */
    scroll_maker.skip         = scui_opt_pos_all;
    
    scui_widget_create(&scroll_maker, &scroll_handle);
    scui_ui_res_local->scroll = scroll_handle;
    
    /* 子控件: 内嵌水平滚动(随机颜色, 边缘对齐粘连) */
    for (uint8_t idx = 0; idx < 6; idx++) {
        scui_scroll_maker_define(scroll2_maker);
        scui_handle_t scroll2_handle = SCUI_HANDLE_INVALID;
        
        scroll2_maker.widget.style.fully_bg     = true;
        scroll2_maker.widget.style.sched_widget = true;
        scroll2_maker.widget.clip.w = scroll_w;
        scroll2_maker.widget.clip.h = item_h;
        scroll2_maker.widget.parent    = scroll_handle;
        scroll2_maker.widget.child_num = 5;
        scroll2_maker.widget.color.color.ch.a = 0xFF;
        scroll2_maker.widget.color.color.ch.r = scui_rand(10) % 2 == 0 ? 0 : 0xFF;
        scroll2_maker.widget.color.color.ch.g = scui_rand(10) % 2 == 0 ? 0 : 0xFF;
        scroll2_maker.widget.color.color.ch.b = scui_rand(10) % 2 == 0 ? 0 : 0xFF;
        scroll2_maker.dir          = scui_opt_dir_hor;
        scroll2_maker.space        = 0;
        scroll2_maker.pos          = scui_opt_pos_hor;      /* 边缘对齐(粘连效果) */
        scroll2_maker.skip         = scui_opt_pos_all;
        scui_widget_create(&scroll2_maker, &scroll2_handle);
        
        /* 俩子控件: 宽度不一(全宽/半宽), 顺序随机, 颜色不一 */
        bool wide_first = scui_rand(10) % 2 == 0;
        for (uint8_t sub = 0; sub < 2; sub++) {
            scui_custom_maker_t custom_maker = scui_test_ui_scroll_custom_base(scroll2_handle);
            scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
            custom_maker.widget.clip.w = (sub == 0) == wide_first ? scroll_w : scroll_w / 2;
            custom_maker.widget.clip.h = item_h;
            scui_widget_create(&custom_maker, &custom_handle);
        }
    }
}

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_scroll_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        scui_test_ui_scroll_create_ver();
        break;
    case scui_event_destroy:
        break;
    case scui_event_key_click: {
        if (event->key_id != scui_event_key_val_enter)
            break;
        
        /* 接口不支持运行时变更参数, 销毁重建更替构建配置 */
        scui_widget_destroy(scui_ui_res_local->scroll);
        scui_ui_res_local->build_idx = (scui_ui_res_local->build_idx + 1) % 4;
        switch (scui_ui_res_local->build_idx) {
        case 0:
            scui_test_ui_scroll_create_ver();
            break;
        case 1:
            scui_test_ui_scroll_create_hor();
            break;
        case 2:
            scui_test_ui_scroll_create_free();
            break;
        case 3:
            scui_test_ui_scroll_create_nest();
            break;
        default:
            break;
        }
        scui_event_mask_over(event);
        break;
    }
    default:
        break;
    }
}
