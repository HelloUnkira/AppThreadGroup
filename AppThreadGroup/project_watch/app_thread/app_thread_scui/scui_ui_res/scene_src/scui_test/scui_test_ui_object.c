/*实现目标:
 *    测试(widget object)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 指示灯流水灯: 赤橙黄绿青蓝紫 */
static const uint32_t led_color[7] = {
    0xFFFF0000,
    0xFFFF8000,
    0xFFFFFF00,
    0xFF00FF00,
    0xFF00FFFF,
    0xFF0000FF,
    0xFF8B00FF,
};
/* 指示灯熄灭色(淡白) */
static const uint32_t led_color_off = 0xFFC8C8C8;

#define LED_NUM             (18)
#define LED_SIZE            (46)
#define LED_GLOW            (12)
#define LED_FILL_STEP       (120)   /* 填充拍间隔(ms) */
#define LED_BREATH_STEP     (800)   /* 呼吸渐变时长(ms) */
#define LED_BREATH_HOLD     (600)   /* 呼吸保持时长(ms) */
#define LED_BTN_W           (90)    /* 开关按钮宽 */
#define LED_BTN_H           (36)    /* 开关按钮高 */

static struct {
    scui_coord_t  obj_arc_w;        /* 圆弧值方向 */
    scui_coord_t  obj_arc_v;        /* 圆弧值 */
    scui_handle_t obj_arc_anima;    /* 圆弧动画控件 */
    scui_coord_t  obj_bar_w1;       /* 条形值方向 */
    scui_coord_t  obj_bar_v1;       /* 条形值 */
    scui_handle_t obj_bar_anima;    /* 条形动画控件 */
    scui_handle_t led[LED_NUM];     /* 指示灯控件 */
    scui_coord_t  led_fill_ms;      /* 填充拍计时 */
    scui_coord_t  led_fill_idx;     /* 填充进度(0-17) */
    scui_coord_t  led_color_idx;    /* 当前颜色(填充:0-6; 呼吸:已渐变完成色) */
    scui_coord_t  led_breath_pct;   /* 呼吸渐变进度(0-100) */
    scui_coord_t  led_hold_ms;      /* 呼吸保持计时 */
    scui_handle_t led_btn;          /* 指示灯测试开关按钮 */
    scui_handle_t led_btn_txt;      /* 开关按钮文本 */
    scui_sbitfd_t led_on:1;         /* 指示灯动画开关 */
    scui_sbitfd_t led_breath:1;     /* 呼吸阶段标记 */
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_btn_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_button_click:
        SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_arc_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_update_value: {
        scui_coord3_t angle = 0.0f;
        scui_obj_arc_current_angle(event->object, &angle);
        SCUI_LOG_INFO("arc angle:%.2f", angle);
        break;
    }
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_bar_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_update_value: {
        scui_coord3_t value = 0.0f;
        scui_obj_bar_current_value(event->object, &value);
        SCUI_LOG_INFO("bar value:%.2f", value);
        break;
    }
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_cht_event_proc(scui_event_t *event)
{
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_scroll_event_proc(scui_event_t *event)
{
}

/*@brief 页面标题事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_title_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        scui_widget_t *widget = scui_handle_source_check(event->object);
        uint8_t *text = NULL;
        switch (widget->myself) {
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_1_TITLE:
            text = "Test Btn";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_2_TITLE:
            text = "Test Arc";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_3_TITLE:
            text = "Test Bar";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_4_TITLE:
            text = "Test Slider";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_5_TITLE:
            text = "Test Switch";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_6_TITLE:
            text = "Test Spinner";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_7_TITLE:
            text = "Test Chart";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_8_TITLE:
            text = "Test Led";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_9_TITLE:
            text = "Test Empty";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJECT_PAGE_10_TITLE:
            text = "Test Empty";
            break;
        default:
            break;
        }
        scui_string_update_str(event->object, text);
        break;
    }
    default:
        break;
    }
}

/*@brief page_1 控件事件响应回调(Test Btn)
 *@param event 事件
 */
void scui_test_ui_object_page_1_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_obj_btn_maker_define(obj_btn_maker);
        obj_btn_maker.widget.color.color.full = 0xFF808080;
        obj_btn_maker.widget.style.fully_bg = 1;
        

        scui_handle_t obj_btn_handle = SCUI_HANDLE_INVALID;
        obj_btn_maker.widget.parent = event->object;
        obj_btn_maker.widget.event_cb = scui_test_ui_object_btn_event_proc;
        
        scui_obj_btn_res_t obj_btn_res = {0};
        obj_btn_res.alpha = scui_alpha_cover;
        obj_btn_res.align = scui_opt_pos_c;
        scui_coord_t btn_w = 140;
        scui_coord_t btn_h = 100;
        
        /* 第1个: 默认(不做任何额外配置, apply默认样式) */
        obj_btn_maker.widget.clip       = SCUI_AREA_MAKE_BM(13, 48, btn_w, btn_h);
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        scui_ui_apply(obj_btn_handle);
        
        /* 第2个: fixed + check + 上色 */
        obj_btn_maker.fixed = 1;
        obj_btn_maker.check = 1;
        obj_btn_res.color[0].color_s.full = 0xFF00FF00;
        obj_btn_res.color[1].color_s.full = 0xFF008000;
        obj_btn_res.color[2].color_s.full = 0xFFFF0000;
        obj_btn_res.color[3].color_s.full = 0xFF008000;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = 20;
        obj_btn_maker.widget.clip.x = 163;
        obj_btn_maker.widget.clip.y = 48;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        obj_btn_res.form = scui_object_form_rect_base;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        /* 第3个: 不fixed + check + 上色 */
        obj_btn_maker.fixed = 0;
        obj_btn_maker.check = 1;
        obj_btn_res.color[0].color_s.full = 0xFFFF0000;
        obj_btn_res.color[1].color_s.full = 0xFF800000;
        obj_btn_res.color[2].color_s.full = 0xFFFF0000;
        obj_btn_res.color[3].color_s.full = 0xFF800000;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = -1;
        obj_btn_maker.widget.clip.x = 313;
        obj_btn_maker.widget.clip.y = 48;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        obj_btn_res.form = scui_object_form_rect_base;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        /* 第4个: 四样式同显(bg/edge/box/sha) */
        obj_btn_maker.fixed = 0;
        obj_btn_maker.check = 1;
        obj_btn_res.color[0].color_s.full = 0xFF87CEFA;
        obj_btn_res.color[1].color_s.full = 0xFF4682B4;
        obj_btn_res.color[2].color_s.full = 0xFF87CEFA;
        obj_btn_res.color[3].color_s.full = 0xFF4682B4;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = 30;
        obj_btn_maker.widget.clip.x = 13;
        obj_btn_maker.widget.clip.y = 158;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        
        scui_obj_btn_res_t obj_btn_res4[4] = {0};
        scui_coord_t stroke[4] = {-1, 4, 4, 12};   /* base填充/edge/box/sha */
        scui_obj_aux_res_rect(obj_btn_res4, (scui_point_t){.x = btn_w, .y = btn_h},
            stroke, (scui_point_t){.x = 30});
        
        /* base: 天蓝 */
        obj_btn_res4[0].color[0].color_s.full = 0xFF87CEFA;
        obj_btn_res4[0].color[1].color_s.full = 0xFF4682B4;
        obj_btn_res4[0].color[2].color_s.full = 0xFF87CEFA;
        obj_btn_res4[0].color[3].color_s.full = 0xFF4682B4;
        /* edge: 白色 */
        obj_btn_res4[1].color[0].color_s.full = 0xFFFFFFFF;
        obj_btn_res4[1].color[1].color_s.full = 0xFFFFFFFF;
        obj_btn_res4[1].color[2].color_s.full = 0xFFFFFFFF;
        obj_btn_res4[1].color[3].color_s.full = 0xFFFFFFFF;
        /* box: 红色 */
        obj_btn_res4[2].color[0].color_s.full = 0xFFFF0000;
        obj_btn_res4[2].color[1].color_s.full = 0xFFFF0000;
        obj_btn_res4[2].color[2].color_s.full = 0xFFFF0000;
        obj_btn_res4[2].color[3].color_s.full = 0xFFFF0000;
        /* sha: 绿色+阴影 */
        obj_btn_res4[3].color[0].color_s.full = 0xFF00FF00;
        obj_btn_res4[3].color[1].color_s.full = 0xFF00FF00;
        obj_btn_res4[3].color[2].color_s.full = 0xFF00FF00;
        obj_btn_res4[3].color[3].color_s.full = 0xFF00FF00;
        obj_btn_res4[3].shadow = 1;
        
        for (scui_coord_t idx = 0; idx < 4; idx++)
            scui_obj_btn_style(obj_btn_handle, &obj_btn_res4[idx]);
        
        /* 第5个: fixed + 不check */
        obj_btn_maker.fixed = 1;
        obj_btn_maker.check = 0;
        obj_btn_res.color[0].color_s.full = 0xFF0000FF;
        obj_btn_res.color[1].color_s.full = 0xFF000080;
        obj_btn_res.color[2].color_s.full = 0xFF0000FF;
        obj_btn_res.color[3].color_s.full = 0xFF000080;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = 20;
        obj_btn_maker.widget.clip.x = 163;
        obj_btn_maker.widget.clip.y = 158;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        obj_btn_res.form = scui_object_form_rect_base;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        /* 第6个: bg+edge+box组合(edge透明隔离bg/box) */
        obj_btn_maker.fixed = 0;
        obj_btn_maker.check = 1;
        obj_btn_res.color[0].color_s.full = 0xFF87CEFA;
        obj_btn_res.color[1].color_s.full = 0xFF4682B4;
        obj_btn_res.color[2].color_s.full = 0xFF87CEFA;
        obj_btn_res.color[3].color_s.full = 0xFF4682B4;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = 30;
        obj_btn_maker.widget.clip.x = 313;
        obj_btn_maker.widget.clip.y = 158;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        
        scui_obj_btn_res_t obj_btn_res6[4] = {0};
        scui_coord_t stroke6[4] = {-1, 4, 8, 0};   /* base填充/edge/box/sha(不绘) */
        scui_obj_aux_res_rect(obj_btn_res6, (scui_point_t){.x = btn_w, .y = btn_h},
            stroke6, (scui_point_t){.x = 30});
        
        /* base: 天蓝 */
        obj_btn_res6[0].color[0].color_s.full = 0xFF87CEFA;
        obj_btn_res6[0].color[1].color_s.full = 0xFF4682B4;
        obj_btn_res6[0].color[2].color_s.full = 0xFF87CEFA;
        obj_btn_res6[0].color[3].color_s.full = 0xFF4682B4;
        /* edge: 透明(alpha=0隔开bg/box) */
        obj_btn_res6[1].alpha = scui_alpha_trans;
        /* box: 红色 */
        obj_btn_res6[2].color[0].color_s.full = 0xFFFF0000;
        obj_btn_res6[2].color[1].color_s.full = 0xFFFF0000;
        obj_btn_res6[2].color[2].color_s.full = 0xFFFF0000;
        obj_btn_res6[2].color[3].color_s.full = 0xFFFF0000;
        
        for (scui_coord_t idx = 0; idx < 4; idx++)
            scui_obj_btn_style(obj_btn_handle, &obj_btn_res6[idx]);
        
        /* 第7/8/9个: obj_chk选中器 */
        scui_obj_chk_maker_define(obj_chk_maker);
        obj_chk_maker.widget.parent = event->object;
        obj_chk_maker.widget.event_cb = scui_test_ui_object_btn_event_proc;
        /* 符号配置: 选中态对勾; def默认空(不绘制) */
        obj_chk_maker.font      = SCUI_FONT_IDX_X24;
        obj_chk_maker.lang      = scui_lang_type_symbol;
        obj_chk_maker.sym_chk   = "\xEF\x80\x8C";
        obj_chk_maker.sym_color.color_s.full = 0xFFFFFFFF;
        
        scui_handle_t obj_chk_handle = SCUI_HANDLE_INVALID;
        /* 第7个: 默认(apply默认样式, 支持check可点击切换, 初始未选) */
        obj_btn_maker.check = 1;
        obj_chk_maker.widget.clip       = SCUI_AREA_MAKE_BM(13, 268, btn_w, btn_h);
        scui_widget_create(&obj_chk_maker, &obj_chk_handle);
        
        /* 第8个: checked(初始选中chk, fixed不响应点击) */
        obj_chk_maker.fixed = 1;
        obj_chk_maker.state = scui_object_state_chk;
        obj_chk_maker.widget.clip.x = 163;
        scui_widget_create(&obj_chk_maker, &obj_chk_handle);
        
        /* 第9个: uncheck(初始未选, fixed不响应点击, 初始def) */
        obj_chk_maker.fixed = 1;
        obj_chk_maker.state = scui_object_state_def;
        obj_chk_maker.widget.clip.x = 313;
        scui_widget_create(&obj_chk_maker, &obj_chk_handle);
        break;
    }
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_2 控件事件响应回调(Test Arc)
 *@param event 事件
 */
void scui_test_ui_object_page_2_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_obj_arc_maker_define(obj_arc_maker);
        obj_arc_maker.widget.color.color.full = 0xFF808080;
        obj_arc_maker.widget.style.fully_bg = 1;
        

        scui_handle_t obj_arc_handle = SCUI_HANDLE_INVALID;
        obj_arc_maker.widget.parent = event->object;
        obj_arc_maker.widget.event_cb = scui_test_ui_object_arc_event_proc;
        
        scui_obj_arc_res_t obj_arc_res = {0};
        obj_arc_res.alpha = scui_alpha_cover;
        /* 统一渐变色调: bg 深蓝->深绿, fg 亮蓝->亮绿 */
        scui_coord_t arc_w = 100;
        scui_coord_t arc_h = 100;
        
        /* 第1行: 普通/整圆/反向 */
        obj_arc_res.center.x = arc_w / 2;
        obj_arc_res.center.y = arc_h / 2;
        obj_arc_res.radius   = arc_w / 2;
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 270;
        obj_arc_res.time     = 0;
        
        obj_arc_maker.anti = 0;
        obj_arc_maker.touch = 0;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip = SCUI_AREA_MAKE_BM(28, 45, arc_w, arc_h);
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 360;
        obj_arc_res.width    = 8;
        obj_arc_res.round    = 0;
        obj_arc_res.gradw    = 1;
        obj_arc_res.grad     = 1;
        obj_arc_maker.widget.clip.x = 168;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 270;
        obj_arc_maker.anti = 1;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 308;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        /* 第2行: 跟手/细弧/粗弧 */
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 270;
        obj_arc_maker.anti = 0;
        obj_arc_maker.touch = 1;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 1;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 28;
        obj_arc_maker.widget.clip.y = 165;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        obj_arc_maker.touch = 0;
        obj_arc_res.width   = 4;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 0;
        obj_arc_maker.widget.clip.x = 168;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 300;
        obj_arc_res.width   = 16;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 1;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 308;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        /* 第3行: 动画/渐变/圆头粗弧 */
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 360;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 28;
        obj_arc_maker.widget.clip.y = 285;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        scui_ui_res_local->obj_arc_anima = obj_arc_handle;
        
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 270;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 1;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 168;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 270;
        obj_arc_res.width   = 12;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 308;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        break;
    }
    case scui_event_anima_elapse: {
        scui_ui_res_local->obj_arc_v += scui_ui_res_local->obj_arc_w;
        
        if (scui_ui_res_local->obj_arc_v == 0)
            scui_ui_res_local->obj_arc_w = +1;
        if (scui_ui_res_local->obj_arc_v == 100)
            scui_ui_res_local->obj_arc_w = -1;
        
        scui_obj_arc_update_value(scui_ui_res_local->obj_arc_anima,
            scui_ui_res_local->obj_arc_v, false);
        break;
    }
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_3 控件事件响应回调(Test Bar)
 *@param event 事件
 */
void scui_test_ui_object_page_3_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_obj_bar_maker_define(obj_bar_maker);
        obj_bar_maker.widget.color.color.full = 0xFF808080;
        obj_bar_maker.widget.style.fully_bg = 1;
        

        scui_handle_t obj_bar_handle = SCUI_HANDLE_INVALID;
        obj_bar_maker.widget.parent = event->object;
        obj_bar_maker.widget.event_cb = scui_test_ui_object_bar_event_proc;
        
        scui_obj_bar_res_t obj_bar_res = {0};
        obj_bar_res.alpha = scui_alpha_cover;
        obj_bar_res.align = scui_opt_pos_l | scui_opt_pos_u;
        /* 统一渐变色调: bg 暗灰(暗色), fg 亮蓝->亮绿 */
        
        scui_coord_t cell_x[3] = {13, 163, 313};
        scui_coord_t cell_y[3] = {48, 158, 268};
        
        /* 第1行: 水平条(值30/70/100): 一半反向 */
        obj_bar_maker.value_lim = 100;
        obj_bar_maker.way = 0;
        obj_bar_res.radius = 7;
        obj_bar_res.grad = 0;
        for (uint32_t idx = 0; idx < 3; idx++) {
            obj_bar_maker.rev = (idx == 1);
            obj_bar_maker.widget.clip = SCUI_AREA_MAKE_BM(cell_x[idx], cell_y[0] + 38, 140, 24);
            scui_widget_create(&obj_bar_maker, &obj_bar_handle);
            obj_bar_res.part = scui_object_part_rect_bg;
            obj_bar_res.form = scui_object_form_rect_base;
            obj_bar_res.color.color_s.full = 0xFF303030;
            obj_bar_res.color.color_e.full = 0xFF505050;
            scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
            obj_bar_res.part = scui_object_part_rect_fg;
            obj_bar_res.form = scui_object_form_rect_base;
            obj_bar_res.color.color_s.full = 0xFF0000FF;
            obj_bar_res.color.color_e.full = 0xFF00FF00;
            scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
            scui_obj_bar_update_value(obj_bar_handle, 30.0f + idx * 40, false);
        }
        
        /* 第2行: 圆角/渐变/分段 */
        obj_bar_res.radius = 15;
        obj_bar_maker.widget.clip.y = cell_y[1] + 35;
        obj_bar_maker.widget.clip.w = 140;
        obj_bar_maker.widget.clip.h = 30;
        for (uint32_t idx = 0; idx < 3; idx++) {
            obj_bar_maker.value_lim = 100;
            obj_bar_maker.value_int = 0;
            obj_bar_res.grad = (idx == 0) ? 0 : 1;
            if (idx == 2) {
                obj_bar_maker.value_lim = 5;
                obj_bar_maker.value_int = 1;
            }
            obj_bar_maker.rev = (idx == 2);
            obj_bar_maker.widget.clip.x = cell_x[idx];
            scui_widget_create(&obj_bar_maker, &obj_bar_handle);
            obj_bar_res.part = scui_object_part_rect_bg;
            obj_bar_res.form = scui_object_form_rect_base;
            obj_bar_res.color.color_s.full = 0xFF303030;
            obj_bar_res.color.color_e.full = 0xFF505050;
            scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
            obj_bar_res.part = scui_object_part_rect_fg;
            obj_bar_res.form = scui_object_form_rect_base;
            obj_bar_res.color.color_s.full = 0xFF0000FF;
            obj_bar_res.color.color_e.full = 0xFF00FF00;
            scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
            scui_obj_bar_update_value(obj_bar_handle, 60.0f - idx * 10, false);
        }
        
        /* 第3行: 垂直条(值40/80/分段) */
        obj_bar_maker.way = 1;
        obj_bar_maker.value_lim = 100;
        obj_bar_maker.value_int = 0;
        obj_bar_res.radius = 7;
        for (uint32_t idx = 0; idx < 3; idx++) {
            obj_bar_res.grad = 0;
            if (idx == 2) {
                obj_bar_maker.value_lim = 5;
                obj_bar_maker.value_int = 1;
            }
            obj_bar_maker.rev = (idx == 1);
            obj_bar_maker.widget.clip = SCUI_AREA_MAKE_BM(cell_x[idx] + 58, cell_y[2], 24, 100);
            scui_widget_create(&obj_bar_maker, &obj_bar_handle);
            obj_bar_res.part = scui_object_part_rect_bg;
            obj_bar_res.form = scui_object_form_rect_base;
            obj_bar_res.color.color_s.full = 0xFF303030;
            obj_bar_res.color.color_e.full = 0xFF505050;
            scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
            obj_bar_res.part = scui_object_part_rect_fg;
            obj_bar_res.form = scui_object_form_rect_base;
            obj_bar_res.color.color_s.full = 0xFF0000FF;
            obj_bar_res.color.color_e.full = 0xFF00FF00;
            scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
            scui_obj_bar_update_value(obj_bar_handle, 40.0f + idx * 20, false);
        }
        
        /* 底部: 动画条 */
        obj_bar_maker.way = 0;
        obj_bar_maker.value_lim = 100;
        obj_bar_maker.value_int = 0;
        obj_bar_res.radius = 15;
        obj_bar_res.grad = 1;
        obj_bar_maker.widget.clip = SCUI_AREA_MAKE_BM(83, 375, 300, 30);
        scui_widget_create(&obj_bar_maker, &obj_bar_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFF303030;
        obj_bar_res.color.color_e.full = 0xFF505050;
        scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFF0000FF;
        obj_bar_res.color.color_e.full = 0xFF00FF00;
        scui_obj_bar_style(obj_bar_handle, &obj_bar_res);
        scui_ui_res_local->obj_bar_anima = obj_bar_handle;
        break;
    }
    case scui_event_anima_elapse: {
        scui_ui_res_local->obj_bar_v1 += scui_ui_res_local->obj_bar_w1;
        
        if (scui_ui_res_local->obj_bar_v1 == 0)
            scui_ui_res_local->obj_bar_w1 = +1;
        if (scui_ui_res_local->obj_bar_v1 == 100)
            scui_ui_res_local->obj_bar_w1 = -1;
        
        scui_obj_bar_update_value(scui_ui_res_local->obj_bar_anima,
            scui_ui_res_local->obj_bar_v1, false);
        break;
    }
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_4 控件事件响应回调(Test Slider)
 *@param event 事件
 */
void scui_test_ui_object_page_4_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_obj_slr_maker_define(obj_slider_maker);
        obj_slider_maker.widget.color.color.full = 0xFF808080;
        obj_slider_maker.widget.style.fully_bg = 1;
        

        scui_handle_t obj_slider_handle = SCUI_HANDLE_INVALID;
        obj_slider_maker.widget.parent = event->object;
        obj_slider_maker.widget.event_cb = scui_test_ui_object_bar_event_proc;
        
        scui_obj_bar_res_t obj_bar_res = {0};
        obj_bar_res.alpha = scui_alpha_cover;
        obj_bar_res.align = scui_opt_pos_l | scui_opt_pos_u;
        /* 统一渐变色调: bg 暗灰(暗色), fg 亮蓝->亮绿 */
        
        scui_coord_t cell_x[3] = {13, 163, 313};
        scui_coord_t cell_y[3] = {48, 158, 268};
        
        /* 第1行: 水平滑条(值30/70/100) */
        obj_slider_maker.obj_bar.value_lim = 100;
        obj_slider_maker.obj_bar.way = 0;
        obj_bar_res.radius = 7;
        obj_bar_res.grad = 0;
        for (uint32_t idx = 0; idx < 3; idx++) {
            obj_slider_maker.obj_bar.rev = (idx == 1);
            obj_slider_maker.widget.clip = SCUI_AREA_MAKE_BM(cell_x[idx], cell_y[0] + 38, 140, 24);
            scui_widget_create(&obj_slider_maker, &obj_slider_handle);
            obj_bar_res.part = scui_object_part_rect_bg;
            obj_bar_res.form = scui_object_form_rect_base;
            obj_bar_res.color.color_s.full = 0xFF303030;
            obj_bar_res.color.color_e.full = 0xFF505050;
            scui_obj_bar_style(obj_slider_handle, &obj_bar_res);
            obj_bar_res.part = scui_object_part_rect_fg;
            obj_bar_res.form = scui_object_form_rect_base;
            obj_bar_res.color.color_s.full = 0xFF0000FF;
            obj_bar_res.color.color_e.full = 0xFF00FF00;
            scui_obj_bar_style(obj_slider_handle, &obj_bar_res);
            scui_obj_bar_update_value(obj_slider_handle, 30.0f + idx * 40, false);
        }
        
        /* 第2行: 圆角/渐变/分段 */
        obj_bar_res.radius = 15;
        obj_slider_maker.widget.clip.y = cell_y[1] + 35;
        obj_slider_maker.widget.clip.w = 140;
        obj_slider_maker.widget.clip.h = 30;
        for (uint32_t idx = 0; idx < 3; idx++) {
            obj_slider_maker.obj_bar.value_lim = 100;
            obj_slider_maker.obj_bar.value_int = 0;
            obj_bar_res.grad = (idx == 0) ? 0 : 1;
            if (idx == 2) {
                obj_slider_maker.obj_bar.value_lim = 5;
                obj_slider_maker.obj_bar.value_int = 1;
            }
            obj_slider_maker.obj_bar.rev = (idx == 2);
            obj_slider_maker.widget.clip.x = cell_x[idx];
            scui_widget_create(&obj_slider_maker, &obj_slider_handle);
            obj_bar_res.part = scui_object_part_rect_bg;
            obj_bar_res.form = scui_object_form_rect_base;
            obj_bar_res.color.color_s.full = 0xFF303030;
            obj_bar_res.color.color_e.full = 0xFF505050;
            scui_obj_bar_style(obj_slider_handle, &obj_bar_res);
            obj_bar_res.part = scui_object_part_rect_fg;
            obj_bar_res.form = scui_object_form_rect_base;
            obj_bar_res.color.color_s.full = 0xFF0000FF;
            obj_bar_res.color.color_e.full = 0xFF00FF00;
            scui_obj_bar_style(obj_slider_handle, &obj_bar_res);
            scui_obj_bar_update_value(obj_slider_handle, 60.0f - idx * 10, false);
        }
        
        /* 第3行: 垂直滑条(值40/60/80) */
        obj_slider_maker.obj_bar.way = 1;
        obj_slider_maker.obj_bar.value_lim = 100;
        obj_slider_maker.obj_bar.value_int = 0;
        obj_bar_res.radius = 7;
        for (uint32_t idx = 0; idx < 3; idx++) {
            obj_bar_res.grad = (idx == 1) ? 1 : 0;
            obj_slider_maker.obj_bar.rev = (idx == 1);
            obj_slider_maker.widget.clip = SCUI_AREA_MAKE_BM(cell_x[idx] + 58, cell_y[2], 24, 100);
            scui_widget_create(&obj_slider_maker, &obj_slider_handle);
            obj_bar_res.part = scui_object_part_rect_bg;
            obj_bar_res.form = scui_object_form_rect_base;
            obj_bar_res.color.color_s.full = 0xFF303030;
            obj_bar_res.color.color_e.full = 0xFF505050;
            scui_obj_bar_style(obj_slider_handle, &obj_bar_res);
            obj_bar_res.part = scui_object_part_rect_fg;
            obj_bar_res.form = scui_object_form_rect_base;
            obj_bar_res.color.color_s.full = 0xFF0000FF;
            obj_bar_res.color.color_e.full = 0xFF00FF00;
            scui_obj_bar_style(obj_slider_handle, &obj_bar_res);
            scui_obj_bar_update_value(obj_slider_handle, 40.0f + idx * 20, false);
        }
        
        /* 底部: 渐变大滑条 */
        obj_slider_maker.obj_bar.way = 0;
        obj_slider_maker.obj_bar.value_lim = 100;
        obj_slider_maker.obj_bar.value_int = 0;
        obj_bar_res.radius = 15;
        obj_bar_res.grad = 1;
        obj_slider_maker.widget.clip     = SCUI_AREA_MAKE_BM(83, 375, 300, 30);
        scui_widget_create(&obj_slider_maker, &obj_slider_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFF303030;
        obj_bar_res.color.color_e.full = 0xFF505050;
        scui_obj_bar_style(obj_slider_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFF0000FF;
        obj_bar_res.color.color_e.full = 0xFF00FF00;
        scui_obj_bar_style(obj_slider_handle, &obj_bar_res);
        scui_obj_bar_update_value(obj_slider_handle, 60.0f, false);
        break;
    }
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_5 控件事件响应回调(Test Switch)
 *@param event 事件
 */
void scui_test_ui_object_page_5_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_obj_swt_maker_define(obj_switch_maker);
        obj_switch_maker.widget.color.color.full = 0xFF808080;
        obj_switch_maker.widget.style.fully_bg = 1;
        

        scui_handle_t obj_switch_handle = SCUI_HANDLE_INVALID;
        obj_switch_maker.widget.parent = event->object;
        obj_switch_maker.widget.event_cb = scui_test_ui_object_bar_event_proc;
        
        scui_obj_bar_res_t obj_bar_res = {0};
        obj_bar_res.alpha = scui_alpha_cover;
        obj_bar_res.align = scui_opt_pos_l | scui_opt_pos_u;
        /* 统一渐变色调: bg 暗灰(暗色), fg 亮蓝->亮绿 */
        
        scui_coord_t cell_x[3] = {13, 163, 313};
        scui_coord_t cell_y[3] = {48, 158, 268};
        scui_coord_t cell_w[3] = {80, 100, 120};
        scui_coord_t cell_h[3] = {40, 50, 60};
        
        /* 第1行: 尺寸变化(小/中/大) */
        obj_switch_maker.obj_bar.value_lim = 100;
        obj_switch_maker.obj_bar.way = 0;
        obj_bar_res.grad = 0;
        for (uint32_t idx = 0; idx < 3; idx++) {
            obj_bar_res.radius = 10;
            obj_switch_maker.obj_bar.rev = (idx == 1);
            obj_switch_maker.widget.clip = SCUI_AREA_MAKE_BM(cell_x[idx] + (140 - cell_w[idx]) / 2, cell_y[0] + (100 - cell_h[idx]) / 2, cell_w[idx], cell_h[idx]);
            scui_widget_create(&obj_switch_maker, &obj_switch_handle);
            obj_bar_res.part = scui_object_part_rect_bg;
            obj_bar_res.form = scui_object_form_rect_base;
            obj_bar_res.color.color_s.full = 0xFF303030;
            obj_bar_res.color.color_e.full = 0xFF505050;
            scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
            obj_bar_res.part = scui_object_part_rect_fg;
            obj_bar_res.form = scui_object_form_rect_base;
            obj_bar_res.color.color_s.full = 0xFF0000FF;
            obj_bar_res.color.color_e.full = 0xFF00FF00;
            scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
            obj_bar_res.part = scui_object_part_rect_knob;
            obj_bar_res.form = scui_object_form_rect_base;
            obj_bar_res.color.color_s.full = 0xFFFFFFFF;
            obj_bar_res.color.color_e.full = 0xFFFFFFFF;
            scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        }
        
        /* 第2行: 圆角/方角/渐变 */
        obj_bar_res.radius = 20;
        obj_switch_maker.widget.clip = SCUI_AREA_MAKE_BM(cell_x[0] + 20, cell_y[1] + 20, 100, 60);
        scui_widget_create(&obj_switch_maker, &obj_switch_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFF303030;
        obj_bar_res.color.color_e.full = 0xFF505050;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFF0000FF;
        obj_bar_res.color.color_e.full = 0xFF00FF00;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_knob;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFFFFFFFF;
        obj_bar_res.color.color_e.full = 0xFFFFFFFF;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        
        obj_bar_res.radius = -1;
        obj_switch_maker.widget.clip.x = cell_x[1] + 20;
        scui_widget_create(&obj_switch_maker, &obj_switch_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFF303030;
        obj_bar_res.color.color_e.full = 0xFF505050;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFF0000FF;
        obj_bar_res.color.color_e.full = 0xFF00FF00;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_knob;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFFFFFFFF;
        obj_bar_res.color.color_e.full = 0xFFFFFFFF;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        
        obj_bar_res.radius = 20;
        obj_bar_res.grad = 1;
        obj_switch_maker.widget.clip.x = cell_x[2] + 20;
        scui_widget_create(&obj_switch_maker, &obj_switch_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFF303030;
        obj_bar_res.color.color_e.full = 0xFF505050;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFF0000FF;
        obj_bar_res.color.color_e.full = 0xFF00FF00;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_knob;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFFFFFFFF;
        obj_bar_res.color.color_e.full = 0xFFFFFFFF;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        
        /* 第3行: 尺寸/渐变组合 */
        obj_bar_res.grad = 0;
        obj_bar_res.radius = 15;
        obj_switch_maker.widget.clip = SCUI_AREA_MAKE_BM(cell_x[0] + 20, cell_y[2] + 20, 100, 60);
        scui_widget_create(&obj_switch_maker, &obj_switch_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFF303030;
        obj_bar_res.color.color_e.full = 0xFF505050;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFF0000FF;
        obj_bar_res.color.color_e.full = 0xFF00FF00;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_knob;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFFFFFFFF;
        obj_bar_res.color.color_e.full = 0xFFFFFFFF;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        
        obj_bar_res.grad = 1;
        obj_switch_maker.widget.clip = SCUI_AREA_MAKE_BM(cell_x[1] + 10, cell_y[2] + 15, 120, 70);
        scui_widget_create(&obj_switch_maker, &obj_switch_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFF303030;
        obj_bar_res.color.color_e.full = 0xFF505050;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFF0000FF;
        obj_bar_res.color.color_e.full = 0xFF00FF00;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_knob;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFFFFFFFF;
        obj_bar_res.color.color_e.full = 0xFFFFFFFF;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        
        obj_bar_res.grad = 0;
        obj_bar_res.radius = 10;
        obj_switch_maker.widget.clip = SCUI_AREA_MAKE_BM(cell_x[2] + 30, cell_y[2] + 30, 80, 40);
        scui_widget_create(&obj_switch_maker, &obj_switch_handle);
        obj_bar_res.part = scui_object_part_rect_bg;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFF303030;
        obj_bar_res.color.color_e.full = 0xFF505050;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_fg;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFF0000FF;
        obj_bar_res.color.color_e.full = 0xFF00FF00;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        obj_bar_res.part = scui_object_part_rect_knob;
        obj_bar_res.form = scui_object_form_rect_base;
        obj_bar_res.color.color_s.full = 0xFFFFFFFF;
        obj_bar_res.color.color_e.full = 0xFFFFFFFF;
        scui_obj_bar_style(obj_switch_handle, &obj_bar_res);
        break;
    }
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_6 控件事件响应回调(Test Spinner)
 *@param event 事件
 */
void scui_test_ui_object_page_6_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_obj_spn_maker_define(obj_spinner_maker);
        obj_spinner_maker.widget.color.color.full = 0xFF808080;
        obj_spinner_maker.widget.style.fully_bg = 1;
        

        scui_handle_t obj_spinner_handle = SCUI_HANDLE_INVALID;
        obj_spinner_maker.widget.parent = event->object;
        obj_spinner_maker.widget.event_cb = scui_test_ui_object_arc_event_proc;
        
        scui_obj_arc_res_t obj_arc_res = {0};
        obj_arc_res.alpha = scui_alpha_cover;
        scui_coord_t arc_w = 90;
        scui_coord_t arc_h = 90;
        obj_arc_res.center.x = arc_w / 2;
        obj_arc_res.center.y = arc_h / 2;
        obj_arc_res.radius   = arc_w / 2;
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 360;
        
        /* 第1行: 慢速/快速/反向 */
        obj_spinner_maker.obj_arc.anti = 0;
        obj_spinner_maker.obj_arc.touch = 0;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 0;
        obj_arc_res.time    = 3000;
        obj_spinner_maker.widget.clip = SCUI_AREA_MAKE_BM(33, 50, arc_w, arc_h);
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        obj_arc_res.time    = 500;
        obj_spinner_maker.widget.clip.x = 173;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        obj_spinner_maker.obj_arc.anti = 1;
        obj_arc_res.time    = 1500;
        obj_spinner_maker.widget.clip.x = 313;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        /* 第2行: 圆头/渐变/粗弧 */
        obj_spinner_maker.obj_arc.anti = 0;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 0;
        obj_arc_res.time    = 1500;
        obj_spinner_maker.widget.clip.x = 33;
        obj_spinner_maker.widget.clip.y = 170;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 1;
        obj_arc_res.grad    = 1;
        obj_spinner_maker.widget.clip.x = 173;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        obj_arc_res.width   = 16;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 0;
        obj_spinner_maker.widget.clip.x = 313;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        /* 第3行: 细弧/渐变圆头/默认 */
        obj_arc_res.width   = 4;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 0;
        obj_spinner_maker.widget.clip.x = 33;
        obj_spinner_maker.widget.clip.y = 290;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        obj_arc_res.width   = 12;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 1;
        obj_arc_res.grad    = 1;
        obj_spinner_maker.widget.clip.x = 173;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 0;
        obj_arc_res.time    = 1500;
        obj_spinner_maker.widget.clip.x = 313;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.form = scui_object_form_arc_base;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        break;
    }
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_7 控件事件响应回调(Test Chart)
 *@param event 事件
 */
void scui_test_ui_object_page_7_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_obj_cht_maker_define(obj_chart_maker);
        obj_chart_maker.widget.color.color.full = 0xFF808080;
        obj_chart_maker.widget.style.fully_bg = 1;
        

        scui_handle_t obj_chart_handle = SCUI_HANDLE_INVALID;
        obj_chart_maker.widget.parent = event->object;
        obj_chart_maker.widget.event_cb = scui_test_ui_object_cht_event_proc;
        
        scui_coord_t vlist[100] = {0};
        scui_coord_t vlist_min[100] = {0};
        scui_coord_t vlist_max[100] = {0};
        for (uint32_t idx = 0; idx < 100; idx++) {
            vlist_min[idx] =  60 + (uint32_t)scui_rand(0xFF) % 40;
            vlist_max[idx] = 220 - (uint32_t)scui_rand(0xFF) % 40;
            vlist[idx] = 60 + (uint32_t)scui_rand(0xFF) % ((220 - 60));
        }
        
        scui_obj_cht_res_t obj_chart_res = {0};
        obj_chart_res.alpha = scui_alpha_cover;
        obj_chart_res.color.color.full = 0xFFFF0000;
        
        /* 第1行: 柱状/折线 */
        obj_chart_maker.type   = 0;
        obj_chart_maker.number = 19;
        obj_chart_maker.space  = 4;
        obj_chart_maker.value_min = 60;
        obj_chart_maker.value_max = 220;
        obj_chart_maker.area.x = 10;
        obj_chart_maker.area.y = 10;
        obj_chart_maker.widget.clip = SCUI_AREA_MAKE_BM(13, 50, 210, 180);
        obj_chart_maker.area.w = obj_chart_maker.widget.clip.w - 10 * 2;
        obj_chart_maker.area.h = obj_chart_maker.widget.clip.h - 10 * 2;
        obj_chart_res.width = 6;
        obj_chart_res.round = true;
        scui_widget_create(&obj_chart_maker, &obj_chart_handle);
        obj_chart_res.part = scui_object_part_rect_fg;
        obj_chart_res.form = scui_object_form_rect_base;
        scui_obj_cht_style(obj_chart_handle, &obj_chart_res);
        scui_obj_cht_hist_data(obj_chart_handle, vlist_min, vlist_max);
        
        obj_chart_maker.type   = 1;
        obj_chart_maker.number = 30;
        obj_chart_maker.space  = 4;
        obj_chart_maker.widget.clip.x = 243;
        obj_chart_res.width = 2;
        obj_chart_res.grad = true;
        obj_chart_res.color.color.full = 0xFF2196F3;
        scui_widget_create(&obj_chart_maker, &obj_chart_handle);
        obj_chart_res.part = scui_object_part_line_item;
        obj_chart_res.form = 0;
        scui_obj_cht_style(obj_chart_handle, &obj_chart_res);
        scui_obj_cht_line_data(obj_chart_handle, vlist);
        
        /* 第2行: 大柱状/渐变折线 */
        obj_chart_maker.type   = 0;
        obj_chart_maker.number = 8;
        obj_chart_maker.space  = 8;
        obj_chart_maker.widget.clip.x = 13;
        obj_chart_maker.widget.clip.y = 250;
        obj_chart_maker.area.w = obj_chart_maker.widget.clip.w - 10 * 2;
        obj_chart_maker.area.h = obj_chart_maker.widget.clip.h - 10 * 2;
        obj_chart_res.width = 16;
        obj_chart_res.round = true;
        obj_chart_res.color.color.full = 0xFF00FF00;
        scui_widget_create(&obj_chart_maker, &obj_chart_handle);
        obj_chart_res.part = scui_object_part_rect_fg;
        obj_chart_res.form = scui_object_form_rect_base;
        scui_obj_cht_style(obj_chart_handle, &obj_chart_res);
        scui_obj_cht_hist_data(obj_chart_handle, vlist_min, vlist_max);
        
        obj_chart_maker.type   = 1;
        obj_chart_maker.number = 50;
        obj_chart_maker.space  = 2;
        obj_chart_maker.widget.clip.x = 243;
        obj_chart_res.width = 2;
        obj_chart_res.grad = true;
        obj_chart_res.color.color.full = 0xFFFF8000;
        scui_widget_create(&obj_chart_maker, &obj_chart_handle);
        obj_chart_res.part = scui_object_part_line_item;
        obj_chart_res.form = 0;
        scui_obj_cht_style(obj_chart_handle, &obj_chart_res);
        scui_obj_cht_line_data(obj_chart_handle, vlist);
        break;
    }
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief LED 测试开关按钮事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_led_btn_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_button_click: {
        scui_ui_res_local->led_on = !scui_ui_res_local->led_on;
        if (scui_ui_res_local->led_on) {
            /* 开启: 重置动画状态, 从头开始填充 */
            scui_ui_res_local->led_fill_ms    = 0;
            scui_ui_res_local->led_fill_idx   = 0;
            scui_ui_res_local->led_color_idx  = 0;
            scui_ui_res_local->led_breath_pct = 0;
            scui_ui_res_local->led_hold_ms    = 0;
            scui_ui_res_local->led_breath     = false;
            scui_string_update_str(scui_ui_res_local->led_btn_txt, (uint8_t *)"ON");
        } else {
            /* 关闭: 全部熄灭 */
            for (scui_coord_t idx = 0; idx < LED_NUM; idx++)
                scui_obj_led_onoff(scui_ui_res_local->led[idx], false, false);
            scui_string_update_str(scui_ui_res_local->led_btn_txt, (uint8_t *)"OFF");
        }
        break;
    }
    default:
        break;
    }
}

/*@brief page_8 控件事件响应回调(Test Led)
 *@param event 事件
 */
void scui_test_ui_object_page_8_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        /* 整体尺寸(灯珠+光晕), 屏幕最大内切圆: 12点钟方向起始, 顺时针均匀排布 */
        const scui_coord_t  led_size  = LED_SIZE + LED_GLOW * 2;
        const scui_coord3_t center_x  = SCUI_HOR_RES / 2;
        const scui_coord3_t center_y  = SCUI_VER_RES / 2;
        const scui_coord_t  radius    = SCUI_HOR_RES / 2 - led_size / 2 - 10;
        const scui_coord3_t step      = 360.0f / LED_NUM;
        
        for (scui_coord_t idx = 0; idx < LED_NUM; idx++) {
            scui_obj_led_maker_define(led_maker);
            scui_handle_t led_handle = SCUI_HANDLE_INVALID;
            
            led_maker.widget.parent  = event->object;
            led_maker.widget.clip    = SCUI_AREA_MAKE_BM(0, 0, LED_SIZE, LED_SIZE);
            led_maker.color_on       = SCUI_COLOR32_MAKE32(led_color[0]);
            led_maker.color_off      = SCUI_COLOR32_MAKE32(led_color_off);
            led_maker.brightness     = 100;
            scui_widget_create(&led_maker, &led_handle);
            
            scui_obj_led_res_t led_res = {
                .area       = SCUI_AREA_MAKE_BM(0, 0, LED_SIZE, LED_SIZE),
                .color_on   = SCUI_COLOR_MAKE32(false, 0, led_color[0]),
                .color_off  = SCUI_COLOR_MAKE32(false, 0, led_color_off),
                .radius     = -1,                    /* 全圆 */
                .alpha      = scui_alpha_cover,
                .align      = scui_opt_pos_c,
                .glow       = LED_GLOW,
                .brightness = 100,
            };
            scui_obj_led_style(led_handle, &led_res);
            
            /* 12点钟(-90度)起始, 顺时针 */
            scui_coord3_t angle = -90.0f + idx * step;
            scui_point_t point = {
                .x = (scui_coord_t)(center_x + radius * scui_cos(SCUI_RAD_BY_A(angle)) - led_size / 2),
                .y = (scui_coord_t)(center_y + radius * scui_sin(SCUI_RAD_BY_A(angle)) - led_size / 2),
            };
            scui_widget_move_pos(led_handle, &point, true);
            
            /* 初始全灭 */
            scui_obj_led_onoff(led_handle, false, false);
            scui_ui_res_local->led[idx] = led_handle;
        }
        
        /* 居中 ON/OFF 开关按钮(控制测试动画, 默认OFF) */
        {
            scui_obj_btn_maker_define(led_btn_maker);
            led_btn_maker.widget.parent          = event->object;
            led_btn_maker.widget.event_cb        = scui_test_ui_object_led_btn_event_proc;
            led_btn_maker.widget.style.indev_ptr = true;
            led_btn_maker.widget.child_num       = 1;
            led_btn_maker.widget.clip            = SCUI_AREA_MAKE_BM(SCUI_HOR_RES / 2 - LED_BTN_W / 2,
                SCUI_VER_RES / 2 - LED_BTN_H / 2, LED_BTN_W, LED_BTN_H);
            scui_widget_create(&led_btn_maker, &scui_ui_res_local->led_btn);
            
            scui_obj_btn_res_t led_btn_res = {0};
            led_btn_res.alpha  = scui_alpha_cover;
            led_btn_res.align  = scui_opt_pos_c;
            led_btn_res.width  = 0;
            led_btn_res.radius = -1;
            led_btn_res.color[0].color_s.full = 0xFF2196F3;
            led_btn_res.color[1].color_s.full = 0xFF1565C0;
            led_btn_res.color[2].color_s.full = 0xFF2196F3;
            led_btn_res.color[3].color_s.full = 0xFF1565C0;
            led_btn_res.part = scui_object_part_rect_bg;
            led_btn_res.form = scui_object_form_rect_base;
            scui_obj_btn_style(scui_ui_res_local->led_btn, &led_btn_res);
            
            /* 按钮文本(按钮区域内居中) */
            scui_string_maker_define(led_btn_txt_maker);
            led_btn_txt_maker.widget.parent         = scui_ui_res_local->led_btn;
            led_btn_txt_maker.widget.clip           = SCUI_AREA_MAKE_BM(0, 0, LED_BTN_W, LED_BTN_H);
            led_btn_txt_maker.font_idx              = SCUI_FONT_IDX_X32;
            led_btn_txt_maker.args.lang             = scui_lang_type_ascii;
            led_btn_txt_maker.args.color.color.full = 0xFFFFFFFF;
            led_btn_txt_maker.args.align_hor        = 2;
            led_btn_txt_maker.args.align_ver        = 2;
            scui_widget_create(&led_btn_txt_maker, &scui_ui_res_local->led_btn_txt);
            scui_string_update_str(scui_ui_res_local->led_btn_txt, (uint8_t *)"OFF");
        }
        
        scui_ui_res_local->led_fill_ms    = 0;
        scui_ui_res_local->led_fill_idx   = 0;
        scui_ui_res_local->led_color_idx  = 0;
        scui_ui_res_local->led_breath_pct = 0;
        scui_ui_res_local->led_hold_ms    = 0;
        scui_ui_res_local->led_breath     = false;
        break;
    }
    case scui_event_anima_elapse: {
        if (!scui_ui_res_local->led_on)
            break;
        if (!scui_ui_res_local->led_breath) {
            /* 填充: 每拍点亮一个LED(当前颜色) */
            scui_ui_res_local->led_fill_ms += event->tick;
            while (scui_ui_res_local->led_fill_ms >= LED_FILL_STEP) {
                scui_ui_res_local->led_fill_ms -= LED_FILL_STEP;
                
                scui_obj_led_color(scui_ui_res_local->led[scui_ui_res_local->led_fill_idx],
                    SCUI_COLOR32_MAKE32(led_color[scui_ui_res_local->led_color_idx]),
                    SCUI_COLOR32_MAKE32(led_color_off));
                scui_obj_led_onoff(scui_ui_res_local->led[scui_ui_res_local->led_fill_idx], false, true);
                scui_ui_res_local->led_fill_idx++;
                
                if (scui_ui_res_local->led_fill_idx >= LED_NUM) {
                    scui_ui_res_local->led_fill_idx  = 0;
                    scui_ui_res_local->led_color_idx++;
                    if (scui_ui_res_local->led_color_idx >= 7) {
                        /* 整圈填满: 进入呼吸, 从最后一色渐变回第一色 */
                        scui_ui_res_local->led_color_idx = 6;
                        scui_ui_res_local->led_breath    = true;
                        break;
                    }
                }
            }
        } else {
            /* 呼吸: 整圈渐变到目标色, 保持, 再渐变下一色(循环) */
            if (scui_ui_res_local->led_hold_ms > 0) {
                scui_ui_res_local->led_hold_ms -= event->tick;
                break;
            }
            
            scui_ui_res_local->led_breath_pct += event->tick * 100 / LED_BREATH_STEP;
            if (scui_ui_res_local->led_breath_pct >= 100) {
                scui_ui_res_local->led_breath_pct = 0;
                scui_ui_res_local->led_hold_ms    = LED_BREATH_HOLD;
                scui_ui_res_local->led_color_idx++;
                if (scui_ui_res_local->led_color_idx >= 7)
                    scui_ui_res_local->led_color_idx = 0;
                break;
            }
            
            scui_color32_t color_cur = SCUI_COLOR32_MAKE32(led_color[scui_ui_res_local->led_color_idx]);
            scui_color32_t color_tar = SCUI_COLOR32_MAKE32(led_color[(scui_ui_res_local->led_color_idx + 1) % 7]);
            scui_color32_t color     = color_cur;
            scui_color32_mix_with(&color, &color_cur, &color_tar, 100 - scui_ui_res_local->led_breath_pct);
            
            for (scui_coord_t idx = 0; idx < LED_NUM; idx++)
                scui_obj_led_color(scui_ui_res_local->led[idx], color, SCUI_COLOR32_MAKE32(led_color_off));
        }
        break;
    }
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_9 控件事件响应回调(Test Empty)
 *@param event 事件
 */
void scui_test_ui_object_page_9_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief page_10 控件事件响应回调(Test Empty)
 *@param event 事件
 */
void scui_test_ui_object_page_10_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_draw_buffer: {
        /* 独立画布内容合成到父控件画布 */
        scui_handle_t surface_image = scui_widget_surface_image(event->object);
        scui_widget_draw_image(event->object, NULL, surface_image, NULL, SCUI_COLOR_UNUSED);
        break;
    }
    default:
        break;
    }
}

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_object_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        break;
    case scui_event_destroy:
        break;

    default:
        break;
    }
}
