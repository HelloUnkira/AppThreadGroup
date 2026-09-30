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

/* 环形指示灯: 孔径上限由孔距/屏内两约束解出(N=18 -> <61.8, 取46) */
#define LED_NUM             (18)
#define LED_SIZE            (46)    /* 孔径(含光晕): 即控件宽高 */
#define LED_GLOW            (6)     /* 光晕环厚(px) */
#define LED_RING_MARGIN     (24)    /* 环形到屏幕边的留白(px) */
#define LED_FILL_STEP       (120)   /* 填充拍间隔(ms) */
#define LED_BREATH_STEP     (800)   /* 呼吸渐变时长(ms) */
#define LED_BREATH_HOLD     (600)   /* 呼吸保持时长(ms) */
#define LED_BTN_W           (90)    /* 开关按钮宽 */
#define LED_BTN_H           (36)    /* 开关按钮高 */

/* obj_arc窗口: 自动值动画控件数 */
#define ARC_ANIMA_NUM       (5)

/* obj_arc 窗口局部资源 */
static struct {
    scui_coord_t  obj_arc_w;                    /* 圆弧值方向 */
    scui_coord_t  obj_arc_v;                    /* 圆弧值 */
    scui_handle_t obj_arc_anima[ARC_ANIMA_NUM]; /* 自动值动画控件 */
    scui_coord_t  obj_arc_anima_num;            /* 已登记控件数 */
} * scui_ui_res_arc = NULL;

/* obj_bar 窗口局部资源 */
static struct {
    scui_coord_t  obj_bar_w1;       /* 条形值方向 */
    scui_coord_t  obj_bar_v1;       /* 条形值 */
    scui_handle_t obj_bar_anima;    /* 条形动画控件 */
} * scui_ui_res_bar = NULL;

/* obj_bar 窗口单元: 三行(控件)x四列(组合); bar行分两条(密集: 8项) */
#define BAR_COL_NUM         (4)
#define BAR_ROW_NUM         (3)
#define BAR_SUB_NUM         (2)     /* bar行子条数 */
#define BAR_CELL_W          (100)
#define BAR_CELL_H          (100)
#define BAR_SUB_H           (40)    /* bar行子条厚(=轨道厚) */
#define BAR_SUB_GAP         (8)     /* bar行子条间距 */
#define BAR_TH              (40)    /* 轨道厚(端点基准直径) */
#define BAR_ANIMA_Y         (400)   /* 底部动画条y */

/* 单元左上角 */
static const scui_coord_t scui_test_ui_object_bar_cell_x[BAR_COL_NUM] = {18, 128, 238, 348};
static const scui_coord_t scui_test_ui_object_bar_cell_y[BAR_ROW_NUM] = {46, 164, 282};

/* 单元组合: knob / radius(<0:最大) */
typedef struct {
    scui_sbitfd_t way:1;        /* 方向(水平:0;垂直:1) */
    scui_sbitfd_t rev:1;        /* 反向 */
    scui_sbitfd_t grad:1;       /* 渐变 */
    scui_sbitfd_t int_step:1;   /* 整数步进 */
    scui_coord_t  knob;         /* 端点样式(0:无端点; 1:仅base; 2:四层级) */
    scui_coord_t  radius;       /* 轨道圆角(<0:最大圆角; 0:方角) */
    scui_coord3_t value_lim;    /* 进度限制 */
    scui_coord3_t value;        /* 进度值 */
} scui_test_ui_object_bar_cell_t;

/* bar行: 两条x四列(8项, 无端点) */
static const scui_test_ui_object_bar_cell_t
scui_test_ui_object_bar_cell_bar[BAR_SUB_NUM * BAR_COL_NUM] = {
    {.way = 0, .rev = 0, .grad = 0, .int_step = 0, .knob = 0, .radius = -1, .value_lim = 100, .value = 30},
    {.way = 0, .rev = 1, .grad = 1, .int_step = 0, .knob = 0, .radius = 20, .value_lim = 100, .value = 70},
    {.way = 0, .rev = 1, .grad = 0, .int_step = 0, .knob = 0, .radius = 14, .value_lim = 100, .value = 85},
    {.way = 0, .rev = 0, .grad = 0, .int_step = 0, .knob = 0, .radius =  0, .value_lim = 100, .value = 60},
    {.way = 0, .rev = 0, .grad = 1, .int_step = 0, .knob = 0, .radius =  6, .value_lim = 100, .value = 45},
    {.way = 0, .rev = 1, .grad = 0, .int_step = 1, .knob = 0, .radius = 16, .value_lim =   8, .value = 5},
    {.way = 0, .rev = 0, .grad = 0, .int_step = 1, .knob = 0, .radius = 12, .value_lim =   5, .value = 3},
    {.way = 0, .rev = 1, .grad = 1, .int_step = 0, .knob = 0, .radius = -1, .value_lim = 100, .value = 88},
};

/* slr行: 端点base(前3列)/四层级(第4列); 含垂直反向 */
static const scui_test_ui_object_bar_cell_t
scui_test_ui_object_bar_cell_knob[BAR_COL_NUM] = {
    {.way = 0, .rev = 0, .grad = 0, .int_step = 0, .knob = 1, .radius = -1, .value_lim = 100, .value = 30},
    {.way = 0, .rev = 1, .grad = 1, .int_step = 0, .knob = 1, .radius = -1, .value_lim = 100, .value = 70},
    {.way = 1, .rev = 1, .grad = 0, .int_step = 1, .knob = 1, .radius = -1, .value_lim =   5, .value = 3},
    {.way = 0, .rev = 0, .grad = 0, .int_step = 0, .knob = 2, .radius = -1, .value_lim = 100, .value = 60},
};

/* swt行: 两稳态(0/上限), 初值须落端点 */
static const scui_test_ui_object_bar_cell_t
scui_test_ui_object_bar_cell_swt[BAR_COL_NUM] = {
    {.way = 0, .rev = 0, .grad = 0, .int_step = 0, .knob = 1, .radius = -1, .value_lim = 100, .value =   0},
    {.way = 0, .rev = 1, .grad = 1, .int_step = 0, .knob = 1, .radius = -1, .value_lim = 100, .value = 100},
    {.way = 1, .rev = 1, .grad = 0, .int_step = 1, .knob = 1, .radius = -1, .value_lim =   5, .value =   5},
    {.way = 0, .rev = 0, .grad = 0, .int_step = 0, .knob = 2, .radius = -1, .value_lim = 100, .value = 100},
};

/* obj_led 窗口局部资源 */
static struct {
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
} * scui_ui_res_led = NULL;

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
        case SCUI_UI_SCENE_TEST_UI_OBJ_BTN_TITLE:
            text = "Test Btn/Chk";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJ_ARC_TITLE:
            text = "Test Arc/Spn";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJ_BAR_TITLE:
            text = "Test Bar/Slr/Swt";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJ_CHT_TITLE:
            text = "Test Chart";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJ_LED_TITLE:
            text = "Test Led";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJ_NONE_1_TITLE:
            text = "Test Empty";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJ_NONE_2_TITLE:
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

/*@brief obj_btn 控件事件响应回调(Test Btn)
 *@param event 事件
 */
void scui_test_ui_object_obj_btn_event_proc(scui_event_t *event)
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
        obj_btn_res.color[0].color_s.full = 0xFF00C000;
        obj_btn_res.color[1].color_s.full = 0xFF006000;
        obj_btn_res.color[2].color_s.full = 0xFFFF8000;
        obj_btn_res.color[3].color_s.full = 0xFF804000;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = 20;
        obj_btn_maker.widget.clip.x = 163;
        obj_btn_maker.widget.clip.y = 48;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        obj_btn_res.form = scui_object_form_rect_all;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        obj_btn_res.form = scui_object_form_rect_base;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        /* 第3个: 不fixed + check + 上色 */
        obj_btn_maker.fixed = 0;
        obj_btn_maker.check = 1;
        obj_btn_res.color[0].color_s.full = 0xFFFF0000;
        obj_btn_res.color[1].color_s.full = 0xFF800000;
        obj_btn_res.color[2].color_s.full = 0xFF00C000;
        obj_btn_res.color[3].color_s.full = 0xFF006000;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = -1;
        obj_btn_maker.widget.clip.x = 313;
        obj_btn_maker.widget.clip.y = 48;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        obj_btn_res.form = scui_object_form_rect_all;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        obj_btn_res.form = scui_object_form_rect_base;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        /* 第4个: 四样式同显(bg/edge/box/sha) */
        obj_btn_maker.fixed = 0;
        obj_btn_maker.check = 1;
        obj_btn_maker.widget.clip.x = 13;
        obj_btn_maker.widget.clip.y = 158;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        
        /* 统一基准(form_rect_all): 几何+缩放 */
        scui_obj_btn_res_t obj_btn_res4 = {0};
        obj_btn_res4.alpha  = scui_alpha_cover;
        obj_btn_res4.align  = scui_opt_pos_c;
        obj_btn_res4.radius = 30;
        obj_btn_res4.part = scui_object_part_rect_bg;
        obj_btn_res4.form = scui_object_form_rect_all;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res4);
        
        /* base: 天蓝(实心) */
        scui_obj_btn_res_t obj_btn_res4l = {0};
        obj_btn_res4l.alpha = scui_alpha_cover;
        obj_btn_res4l.part  = scui_object_part_rect_bg;
        obj_btn_res4l.form  = scui_object_form_rect_base;
        obj_btn_res4l.width = -1;
        obj_btn_res4l.color[0].color_s.full = 0xFF87CEFA;
        obj_btn_res4l.color[1].color_s.full = 0xFF4682B4;
        obj_btn_res4l.color[2].color_s.full = 0xFFFFA500;
        obj_btn_res4l.color[3].color_s.full = 0xFFB36B00;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res4l);
        /* edge: 白/灰白 */
        obj_btn_res4l.form  = scui_object_form_rect_edge;
        obj_btn_res4l.width = 4;
        obj_btn_res4l.color[0].color_s.full = 0xFFFFFFFF;
        obj_btn_res4l.color[1].color_s.full = 0xFFB0BEC5;
        obj_btn_res4l.color[2].color_s.full = 0xFFFFF0B0;
        obj_btn_res4l.color[3].color_s.full = 0xFFC0A860;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res4l);
        /* box: 红/绿 */
        obj_btn_res4l.form  = scui_object_form_rect_box;
        obj_btn_res4l.width = 4;
        obj_btn_res4l.color[0].color_s.full = 0xFFFF0000;
        obj_btn_res4l.color[1].color_s.full = 0xFF800000;
        obj_btn_res4l.color[2].color_s.full = 0xFF00C000;
        obj_btn_res4l.color[3].color_s.full = 0xFF006000;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res4l);
        /* sha: 绿/青(阴影恒开) */
        obj_btn_res4l.form  = scui_object_form_rect_sha;
        obj_btn_res4l.width = 12;
        obj_btn_res4l.color[0].color_s.full = 0xFF00FF00;
        obj_btn_res4l.color[1].color_s.full = 0xFF008000;
        obj_btn_res4l.color[2].color_s.full = 0xFF00FFFF;
        obj_btn_res4l.color[3].color_s.full = 0xFF008080;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res4l);
        
        /* 第5个: fixed + 不check */
        obj_btn_maker.fixed = 1;
        obj_btn_maker.check = 0;
        obj_btn_res.color[0].color_s.full = 0xFF0000FF;
        obj_btn_res.color[1].color_s.full = 0xFF000080;
        obj_btn_res.color[2].color_s.full = 0xFF00FFFF;
        obj_btn_res.color[3].color_s.full = 0xFF008080;
        obj_btn_res.width  = 0;
        obj_btn_res.radius = 20;
        obj_btn_maker.widget.clip.x = 163;
        obj_btn_maker.widget.clip.y = 158;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        obj_btn_res.part = scui_object_part_rect_bg;
        obj_btn_res.form = scui_object_form_rect_all;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        obj_btn_res.form = scui_object_form_rect_base;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res);
        
        /* 第6个: bg+edge+box组合(edge透明隔离bg/box) */
        obj_btn_maker.fixed = 0;
        obj_btn_maker.check = 1;
        obj_btn_maker.widget.clip.x = 313;
        obj_btn_maker.widget.clip.y = 158;
        scui_widget_create(&obj_btn_maker, &obj_btn_handle);
        
        /* 统一基准(form_rect_all): 几何+缩放 */
        scui_obj_btn_res_t obj_btn_res6 = {0};
        obj_btn_res6.alpha  = scui_alpha_cover;
        obj_btn_res6.align  = scui_opt_pos_c;
        obj_btn_res6.radius = 30;
        obj_btn_res6.part = scui_object_part_rect_bg;
        obj_btn_res6.form = scui_object_form_rect_all;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res6);
        
        /* base: 天蓝/橙 */
        scui_obj_btn_res_t obj_btn_res6l = {0};
        obj_btn_res6l.alpha = scui_alpha_cover;
        obj_btn_res6l.part  = scui_object_part_rect_bg;
        obj_btn_res6l.form  = scui_object_form_rect_base;
        obj_btn_res6l.width = -1;
        obj_btn_res6l.color[0].color_s.full = 0xFF87CEFA;
        obj_btn_res6l.color[1].color_s.full = 0xFF4682B4;
        obj_btn_res6l.color[2].color_s.full = 0xFFFFA500;
        obj_btn_res6l.color[3].color_s.full = 0xFFB36B00;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res6l);
        /* edge: 透明(alpha=0隔开bg/box) */
        obj_btn_res6l.form  = scui_object_form_rect_edge;
        obj_btn_res6l.width = 4;
        obj_btn_res6l.alpha = scui_alpha_trans;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res6l);
        /* box: 红/绿 */
        obj_btn_res6l.form  = scui_object_form_rect_box;
        obj_btn_res6l.width = 8;
        obj_btn_res6l.alpha = scui_alpha_cover;
        obj_btn_res6l.color[0].color_s.full = 0xFFFF0000;
        obj_btn_res6l.color[1].color_s.full = 0xFF800000;
        obj_btn_res6l.color[2].color_s.full = 0xFF00C000;
        obj_btn_res6l.color[3].color_s.full = 0xFF006000;
        scui_obj_btn_style(obj_btn_handle, &obj_btn_res6l);
        
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
        scui_ui_apply(obj_chk_handle);
        
        /* 第8个: checked(初始选中chk, fixed不响应点击) */
        obj_chk_maker.fixed = 1;
        obj_chk_maker.state = scui_object_state_chk;
        obj_chk_maker.widget.clip.x = 163;
        scui_widget_create(&obj_chk_maker, &obj_chk_handle);
        scui_ui_apply(obj_chk_handle);
        
        /* 第9个: uncheck(初始未选, fixed不响应点击, 初始def) */
        obj_chk_maker.fixed = 1;
        obj_chk_maker.state = scui_object_state_def;
        obj_chk_maker.widget.clip.x = 313;
        scui_widget_create(&obj_chk_maker, &obj_chk_handle);
        scui_ui_apply(obj_chk_handle);
        break;
    }
    default:
        break;
    }
}

/*@brief obj_arc 控件事件响应回调(Test Arc + Spinner)
 *@param event 事件
 */
void scui_test_ui_object_obj_arc_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_arc));
        scui_window_local_res_get(event->object, (void **)&scui_ui_res_arc);
        scui_ui_res_arc->obj_arc_anima_num = 0;
        
        scui_coord_t arc_w = 100;
        scui_coord_t arc_h = 100;
        
        scui_obj_arc_maker_define(obj_arc_maker);
        obj_arc_maker.widget.color.color.full = 0xFF808080;
        obj_arc_maker.widget.style.fully_bg = 1;
        
        
        scui_handle_t obj_arc_handle = SCUI_HANDLE_INVALID;
        obj_arc_maker.widget.parent = event->object;
        obj_arc_maker.widget.event_cb = scui_test_ui_object_arc_event_proc;
        
        scui_obj_arc_res_t obj_arc_res = {0};
        obj_arc_res.alpha = scui_alpha_cover;
        /* 统一渐变色调: bg 深蓝->深绿, fg 亮蓝->亮绿 */
        obj_arc_res.center.x = arc_w / 2;
        obj_arc_res.center.y = arc_h / 2;
        obj_arc_res.radius   = arc_w / 2;
        obj_arc_res.time     = 0;
        
        /* 第1行: arc 默认/渐变/跟手 */
        obj_arc_maker.anti  = 0;
        obj_arc_maker.touch = 0;
        obj_arc_res.angle_s = 0;
        obj_arc_res.angle_e = 270;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 0;
        obj_arc_maker.widget.clip = SCUI_AREA_MAKE_BM(28, 45, arc_w, arc_h);
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        scui_ui_res_arc->obj_arc_anima[scui_ui_res_arc->obj_arc_anima_num++] = obj_arc_handle;
        
        obj_arc_res.angle_s = 0;
        obj_arc_res.angle_e = 360;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 1;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 168;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        scui_ui_res_arc->obj_arc_anima[scui_ui_res_arc->obj_arc_anima_num++] = obj_arc_handle;
        
        obj_arc_res.angle_s = 0;
        obj_arc_res.angle_e = 270;
        obj_arc_maker.touch = 1;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 308;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        /* 第2行: arc 细弧(自动值动画)/圆头渐变/粗弧反向 */
        obj_arc_maker.touch = 0;
        obj_arc_res.angle_s = 0;
        obj_arc_res.angle_e = 270;
        obj_arc_res.width   = 4;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 0;
        obj_arc_maker.widget.clip.x = 28;
        obj_arc_maker.widget.clip.y = 165;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        scui_ui_res_arc->obj_arc_anima[scui_ui_res_arc->obj_arc_anima_num++] = obj_arc_handle;
        
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 1;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 168;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        scui_ui_res_arc->obj_arc_anima[scui_ui_res_arc->obj_arc_anima_num++] = obj_arc_handle;
        
        obj_arc_res.angle_s = 0;
        obj_arc_res.angle_e = 360;
        obj_arc_maker.anti  = 1;
        obj_arc_res.width   = 16;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 1;
        obj_arc_res.grad    = 1;
        obj_arc_maker.widget.clip.x = 308;
        scui_widget_create(&obj_arc_maker, &obj_arc_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_arc_handle, &obj_arc_res);
        
        scui_ui_res_arc->obj_arc_anima[scui_ui_res_arc->obj_arc_anima_num++] = obj_arc_handle;
        
        /* 第3行: spinner 默认/圆头渐变/粗弧反向 */
        scui_obj_spn_maker_define(obj_spinner_maker);
        obj_spinner_maker.widget.color.color.full = 0xFF808080;
        obj_spinner_maker.widget.style.fully_bg = 1;
        
        
        scui_handle_t obj_spinner_handle = SCUI_HANDLE_INVALID;
        obj_spinner_maker.widget.parent = event->object;
        obj_spinner_maker.widget.event_cb = scui_test_ui_object_arc_event_proc;
        
        /* 旋转器: 整圈轨道 */
        obj_arc_res.center.x = arc_w / 2;
        obj_arc_res.center.y = arc_h / 2;
        obj_arc_res.radius   = arc_w / 2;
        obj_arc_res.angle_s  = 0;
        obj_arc_res.angle_e  = 360;
        obj_arc_res.time     = 0;
        
        obj_spinner_maker.obj_arc.anti = 0;
        obj_arc_res.width   = 8;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 0;
        obj_spinner_maker.widget.clip = SCUI_AREA_MAKE_BM(28, 285, arc_w, arc_h);
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        obj_arc_res.time    = 1500;
        obj_arc_res.round   = 1;
        obj_arc_res.gradw   = 1;
        obj_arc_res.grad    = 1;
        obj_spinner_maker.widget.clip.x = 168;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        
        obj_spinner_maker.obj_arc.anti = 1;
        obj_arc_res.time    = 800;
        obj_arc_res.width   = 16;
        obj_arc_res.round   = 0;
        obj_arc_res.gradw   = 0;
        obj_arc_res.grad    = 0;
        obj_spinner_maker.widget.clip.x = 308;
        scui_widget_create(&obj_spinner_maker, &obj_spinner_handle);
        obj_arc_res.part = scui_object_part_arc_bg;
        obj_arc_res.color.color_s.full = 0xFF000080;
        obj_arc_res.color.color_e.full = 0xFF008000;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.part = scui_object_part_arc_fg;
        obj_arc_res.color.color_s.full = 0xFF0000FF;
        obj_arc_res.color.color_e.full = 0xFF00FF00;
        obj_arc_res.form = scui_object_form_arc_all;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        obj_arc_res.form = scui_object_form_arc_base;
        scui_obj_arc_style(obj_spinner_handle, &obj_arc_res);
        break;
    }
    case scui_event_anima_elapse: {
        scui_ui_res_arc->obj_arc_v += scui_ui_res_arc->obj_arc_w;
        
        if (scui_ui_res_arc->obj_arc_v == 0)
            scui_ui_res_arc->obj_arc_w = +1;
        if (scui_ui_res_arc->obj_arc_v == 100)
            scui_ui_res_arc->obj_arc_w = -1;
        
        for (scui_coord_t idx = 0; idx < scui_ui_res_arc->obj_arc_anima_num; idx++)
            scui_obj_arc_update_value(scui_ui_res_arc->obj_arc_anima[idx],
                scui_ui_res_arc->obj_arc_v, false);
        break;
    }
    default:
        break;
    }
}

/*@brief obj_bar系(bar/slr/swt)轨道样式
 *@param handle 控件句柄
 *@param res    样式资源(调用后还原)
 */
static void scui_test_ui_object_bar_track_cfg(scui_handle_t handle, scui_obj_bar_res_t *res)
{
    scui_obj_bar_res_t res_bak = *res;
    
    /* 轨道: 暗灰底 */
    res->part = scui_object_part_rect_bg;
    res->color.color_s.full = 0xFF303030;
    res->color.color_e.full = 0xFF505050;
    res->form = scui_object_form_rect_all;
    scui_obj_bar_style(handle, res);
    res->form = scui_object_form_rect_base;
    scui_obj_bar_style(handle, res);
    
    /* 进度: 亮蓝->亮绿 */
    res->part = scui_object_part_rect_fg;
    res->color.color_s.full = 0xFF0000FF;
    res->color.color_e.full = 0xFF00FF00;
    res->form = scui_object_form_rect_all;
    scui_obj_bar_style(handle, res);
    res->form = scui_object_form_rect_base;
    scui_obj_bar_style(handle, res);
    
    *res = res_bak;
}

/*@brief obj_bar系(bar/slr/swt)端点样式
 *@param handle 控件句柄
 *@param res    样式资源(调用后还原)
 *@note  只写base(白色实心全圆); 几何(点/宽高)由update_value统一按
 *       「最大显示区域」推演; 未配其余层时base即整个显示区
 */
static void scui_test_ui_object_bar_knob_cfg(scui_handle_t handle, scui_obj_bar_res_t *res)
{
    scui_obj_bar_res_t res_bak = *res;
    
    res->alpha = scui_alpha_cover;
    res->radius = -1;
    res->color.color_s.full = 0xFFFFFFFF;
    res->color.color_e.full = 0xFFFFFFFF;
    res->part = scui_object_part_rect_knob;
    res->form = scui_object_form_rect_all;
    scui_obj_bar_style(handle, res);
    res->form = scui_object_form_rect_base;
    scui_obj_bar_style(handle, res);
    
    *res = res_bak;
}

/*@brief obj_bar系(bar/slr/swt)端点四层级样式
 *@param handle 控件句柄
 *@param res    样式资源(调用后还原)
 *@note  对齐btn的"四样式同显": 端点base/edge/box/sha同时上色;
 *       各层由form在端点显示区内自居(sha/box/edge成环, base为核心)
 */
static void scui_test_ui_object_bar_knob4_cfg(scui_handle_t handle, scui_obj_bar_res_t *res)
{
    scui_obj_bar_res_t res_bak = *res;
    
    res->part  = scui_object_part_rect_knob;
    res->alpha = scui_alpha_cover;
    res->radius = -1;
    res->grad  = 0;
    res->gradw = 0;
    
    /* 统一基准: 端点整体几何 */
    res->form = scui_object_form_rect_all;
    scui_obj_bar_style(handle, res);
    
    /* base: 白色实心(核心) */
    res->width = 0;
    res->color.color_s.full = 0xFFFFFFFF;
    res->color.color_e.full = 0xFFFFFFFF;
    res->form = scui_object_form_rect_base;
    scui_obj_bar_style(handle, res);
    
    /* edge: 天蓝环 */
    res->width = 3;
    res->color.color_s.full = 0xFF2196F3;
    res->color.color_e.full = 0xFF2196F3;
    res->form = scui_object_form_rect_edge;
    scui_obj_bar_style(handle, res);
    
    /* box: 红环 */
    res->width = 4;
    res->color.color_s.full = 0xFFFF0000;
    res->color.color_e.full = 0xFFFF0000;
    res->form = scui_object_form_rect_box;
    scui_obj_bar_style(handle, res);
    
    /* sha: 绿光晕(末端色须与起始色有差+透明) */
    res->width = 6;
    res->color.color_s.full = 0xFF00FF00;
    res->color.color_e.full = 0x00000000;
    res->form = scui_object_form_rect_sha;
    scui_obj_bar_style(handle, res);
    
    *res = res_bak;
}

/*@brief obj_bar系(bar/slr/swt)单元行
 *@param maker   控件构造器(bar/slr/swt; 直传给create)
 *@param widget  构造器的widget段
 *@param obj     构造器的obj_bar语义段(bar:本尊; slr/swt:内嵌obj_bar)
 *@param parent  父控件
 *@param cell    该行单元表(子条优先展开: 子条*BAR_COL_NUM + 列)
 *@param cell_y  该行首个子条的y
 *@param sub_h   子条高(bar行=BAR_SUB_H; slr/swt行=BAR_CELL_H)
 *@param sub_num 子条数
 *@note  列组合见scui_test_ui_object_bar_cell_bar/knob; knob=0时端点无样式(不绘制)
 */
static void scui_test_ui_object_bar_cells(void *maker, scui_widget_maker_t *widget,
    scui_obj_bar_maker_t *obj, scui_handle_t parent,
    const scui_test_ui_object_bar_cell_t *cell, scui_coord_t cell_y,
    scui_coord_t sub_h, scui_coord_t sub_num)
{
    widget->color.color.full = 0xFF808080;
    widget->style.fully_bg   = 1;
    widget->parent           = parent;
    widget->event_cb         = scui_test_ui_object_bar_event_proc;
    
    for (scui_coord_t sub = 0; sub < sub_num; sub++)
    for (scui_coord_t col = 0; col < BAR_COL_NUM; col++) {
        const scui_test_ui_object_bar_cell_t *unit = &cell[sub * BAR_COL_NUM + col];
        scui_handle_t  handle = SCUI_HANDLE_INVALID;
        scui_obj_bar_res_t res = {0};
        scui_coord_t   y = cell_y + sub * (sub_h + BAR_SUB_GAP);
        
        /* 复用构造器: 每单元重置全部语义字段 */
        obj->way       = unit->way;
        obj->rev       = unit->rev;
        obj->value_lim = unit->value_lim;
        obj->value_int = unit->int_step;
        
        /* 轨道厚恒为BAR_TH */
        if (unit->way) {
            widget->clip = SCUI_AREA_MAKE_BM(
                scui_test_ui_object_bar_cell_x[col] + (BAR_CELL_W - BAR_TH) / 2,
                y, BAR_TH, sub_h);
        } else {
            widget->clip = SCUI_AREA_MAKE_BM(
                scui_test_ui_object_bar_cell_x[col],
                y + (sub_h - BAR_TH) / 2, BAR_CELL_W, BAR_TH);
        }
        scui_widget_create(maker, &handle);
        
        res.alpha  = scui_alpha_cover;
        res.align  = scui_opt_pos_l | scui_opt_pos_u;
        res.radius = unit->radius;
        res.grad   = unit->grad;
        scui_test_ui_object_bar_track_cfg(handle, &res);
        
        /* 端点: 未配置端点样式时不绘制 */
        if      (unit->knob == 2) scui_test_ui_object_bar_knob4_cfg(handle, &res);
        else if (unit->knob == 1) scui_test_ui_object_bar_knob_cfg(handle, &res);
        
        scui_obj_bar_update_value(handle, unit->value, false);
    }
}

/*@brief obj_bar系(bar/slr/swt)控件事件响应回调(Test Bar/Slr/Swt)
 *@param event 事件
 */
void scui_test_ui_object_obj_bar_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_bar));
        scui_window_local_res_get(event->object, (void **)&scui_ui_res_bar);
        
        /* 三行x四列: 行=控件, 列=组合 */
        /* 第1行: bar(无端点) */
        {
            scui_obj_bar_maker_define(maker);
            scui_test_ui_object_bar_cells(&maker, &maker.widget, &maker, event->object,
                scui_test_ui_object_bar_cell_bar,
                scui_test_ui_object_bar_cell_y[0], BAR_SUB_H, BAR_SUB_NUM);
        }
        
        /* 第2行: slider(按压放大) */
        {
            scui_obj_slr_maker_define(maker);
            maker.press = 1;
            scui_test_ui_object_bar_cells(&maker, &maker.widget, &maker.obj_bar, event->object,
                scui_test_ui_object_bar_cell_knob,
                scui_test_ui_object_bar_cell_y[1], BAR_CELL_H, 1);
        }
        
        /* 第3行: switch(点击翻转; 初值取端点, 开关无中间态) */
        {
            scui_obj_swt_maker_define(maker);
            scui_test_ui_object_bar_cells(&maker, &maker.widget, &maker.obj_bar, event->object,
                scui_test_ui_object_bar_cell_swt,
                scui_test_ui_object_bar_cell_y[2], BAR_CELL_H, 1);
        }
        
        /* 底部: 动画条(值往复) */
        {
            scui_obj_bar_maker_define(maker);
            scui_handle_t handle = SCUI_HANDLE_INVALID;
            scui_obj_bar_res_t res = {0};
            
            maker.widget.parent   = event->object;
            maker.widget.event_cb = scui_test_ui_object_bar_event_proc;
            maker.widget.clip     = SCUI_AREA_MAKE_BM(83, BAR_ANIMA_Y, 300, 30);
            maker.way       = 0;
            maker.rev       = 0;
            maker.value_lim = 100;
            maker.value_int = 0;
            scui_widget_create(&maker, &handle);
            
            res.alpha  = scui_alpha_cover;
            res.align  = scui_opt_pos_l | scui_opt_pos_u;
            res.radius = 15;
            res.grad   = 1;
            scui_test_ui_object_bar_track_cfg(handle, &res);
            scui_ui_res_bar->obj_bar_anima = handle;
        }
        break;
    }
    case scui_event_anima_elapse: {
        scui_ui_res_bar->obj_bar_v1 += scui_ui_res_bar->obj_bar_w1;
        
        if (scui_ui_res_bar->obj_bar_v1 == 0)
            scui_ui_res_bar->obj_bar_w1 = +1;
        if (scui_ui_res_bar->obj_bar_v1 == 100)
            scui_ui_res_bar->obj_bar_w1 = -1;
        
        scui_obj_bar_update_value(scui_ui_res_bar->obj_bar_anima,
            scui_ui_res_bar->obj_bar_v1, false);
        break;
    }
    default:
        break;
    }
}

/*@brief obj_cht 控件事件响应回调(Test Chart)
 *@param event 事件
 */
void scui_test_ui_object_obj_cht_event_proc(scui_event_t *event)
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
        scui_ui_res_led->led_on = !scui_ui_res_led->led_on;
        if (scui_ui_res_led->led_on) {
            /* 开启: 重置动画状态, 从头开始填充 */
            scui_ui_res_led->led_fill_ms    = 0;
            scui_ui_res_led->led_fill_idx   = 0;
            scui_ui_res_led->led_color_idx  = 0;
            scui_ui_res_led->led_breath_pct = 0;
            scui_ui_res_led->led_hold_ms    = 0;
            scui_ui_res_led->led_breath     = false;
            scui_string_update_str(scui_ui_res_led->led_btn_txt, (uint8_t *)"ON");
        } else {
            /* 关闭: 全部熄灭 */
            for (scui_coord_t idx = 0; idx < LED_NUM; idx++)
                scui_obj_led_onoff(scui_ui_res_led->led[idx], false, false);
            scui_string_update_str(scui_ui_res_led->led_btn_txt, (uint8_t *)"OFF");
        }
        break;
    }
    default:
        break;
    }
}

/*@brief obj_led 控件事件响应回调(Test Led)
 *@param event 事件
 */
void scui_test_ui_object_obj_led_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_led));
        scui_window_local_res_get(event->object, (void **)&scui_ui_res_led);
        
        /* 孔径=控件宽高: 12点起始顺时针排布 */
        const scui_coord3_t center_x  = SCUI_HOR_RES / 2;
        const scui_coord3_t center_y  = SCUI_VER_RES / 2;
        const scui_coord_t  radius    = SCUI_HOR_RES / 2 - LED_SIZE / 2 - LED_RING_MARGIN;
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
                .color_on   = SCUI_COLOR_MAKE32(false, 0, led_color[0]),
                .color_off  = SCUI_COLOR_MAKE32(false, 0, led_color_off),
                .radius     = -1,                    /* 全圆 */
                .alpha      = scui_alpha_cover,
                .align      = scui_opt_pos_c,
                .brightness = 100,
            };
            /* 统一基准(几何) */
            led_res.part = scui_object_part_rect_bg;
            led_res.form = scui_object_form_rect_all;
            scui_obj_led_style(led_handle, &led_res);
            /* sha: 光晕(描边, 初始隐藏) */
            led_res.form  = scui_object_form_rect_sha;
            led_res.width = LED_GLOW;
            led_res.alpha = scui_alpha_trans;
            scui_obj_led_style(led_handle, &led_res);
            /* base: 实心灯珠 */
            led_res.form  = scui_object_form_rect_base;
            led_res.width = 0;
            led_res.alpha = scui_alpha_cover;
            scui_obj_led_style(led_handle, &led_res);
            
            /* 12点(-90度)起始顺时针 */
            scui_coord3_t angle = -90.0f + idx * step;
            scui_point_t point = {
                .x = (scui_coord_t)(center_x + radius * scui_cos(SCUI_RAD_BY_A(angle)) - LED_SIZE / 2),
                .y = (scui_coord_t)(center_y + radius * scui_sin(SCUI_RAD_BY_A(angle)) - LED_SIZE / 2),
            };
            scui_widget_move_pos(led_handle, &point, true);
            
            /* 初始全灭 */
            scui_obj_led_onoff(led_handle, false, false);
            scui_ui_res_led->led[idx] = led_handle;
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
            scui_widget_create(&led_btn_maker, &scui_ui_res_led->led_btn);
            
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
            scui_obj_btn_style(scui_ui_res_led->led_btn, &led_btn_res);
            
            /* 按钮文本(按钮区域内居中) */
            scui_string_maker_define(led_btn_txt_maker);
            led_btn_txt_maker.widget.parent         = scui_ui_res_led->led_btn;
            led_btn_txt_maker.widget.clip           = SCUI_AREA_MAKE_BM(0, 0, LED_BTN_W, LED_BTN_H);
            led_btn_txt_maker.font_idx              = SCUI_FONT_IDX_X32;
            led_btn_txt_maker.args.lang             = scui_lang_type_ascii;
            led_btn_txt_maker.args.color.color.full = 0xFFFFFFFF;
            led_btn_txt_maker.args.align_hor        = 2;
            led_btn_txt_maker.args.align_ver        = 2;
            scui_widget_create(&led_btn_txt_maker, &scui_ui_res_led->led_btn_txt);
            scui_string_update_str(scui_ui_res_led->led_btn_txt, (uint8_t *)"OFF");
        }
        
        scui_ui_res_led->led_fill_ms    = 0;
        scui_ui_res_led->led_fill_idx   = 0;
        scui_ui_res_led->led_color_idx  = 0;
        scui_ui_res_led->led_breath_pct = 0;
        scui_ui_res_led->led_hold_ms    = 0;
        scui_ui_res_led->led_breath     = false;
        break;
    }
    case scui_event_anima_elapse: {
        if (!scui_ui_res_led->led_on)
            break;
        if (!scui_ui_res_led->led_breath) {
            /* 填充: 每拍点亮一个LED(当前颜色) */
            scui_ui_res_led->led_fill_ms += event->tick;
            while (scui_ui_res_led->led_fill_ms >= LED_FILL_STEP) {
                scui_ui_res_led->led_fill_ms -= LED_FILL_STEP;
                
                scui_obj_led_color(scui_ui_res_led->led[scui_ui_res_led->led_fill_idx],
                    SCUI_COLOR32_MAKE32(led_color[scui_ui_res_led->led_color_idx]),
                    SCUI_COLOR32_MAKE32(led_color_off));
                scui_obj_led_onoff(scui_ui_res_led->led[scui_ui_res_led->led_fill_idx], false, true);
                scui_ui_res_led->led_fill_idx++;
                
                if (scui_ui_res_led->led_fill_idx >= LED_NUM) {
                    scui_ui_res_led->led_fill_idx  = 0;
                    scui_ui_res_led->led_color_idx++;
                    if (scui_ui_res_led->led_color_idx >= 7) {
                        /* 整圈填满: 进入呼吸, 从最后一色渐变回第一色 */
                        scui_ui_res_led->led_color_idx = 6;
                        scui_ui_res_led->led_breath    = true;
                        break;
                    }
                }
            }
        } else {
            /* 呼吸: 整圈渐变到目标色, 保持, 再渐变下一色(循环) */
            if (scui_ui_res_led->led_hold_ms > 0) {
                scui_ui_res_led->led_hold_ms -= event->tick;
                break;
            }
            
            scui_ui_res_led->led_breath_pct += event->tick * 100 / LED_BREATH_STEP;
            if (scui_ui_res_led->led_breath_pct >= 100) {
                scui_ui_res_led->led_breath_pct = 0;
                scui_ui_res_led->led_hold_ms    = LED_BREATH_HOLD;
                scui_ui_res_led->led_color_idx++;
                if (scui_ui_res_led->led_color_idx >= 7)
                    scui_ui_res_led->led_color_idx = 0;
                break;
            }
            
            scui_color32_t color_cur = SCUI_COLOR32_MAKE32(led_color[scui_ui_res_led->led_color_idx]);
            scui_color32_t color_tar = SCUI_COLOR32_MAKE32(led_color[(scui_ui_res_led->led_color_idx + 1) % 7]);
            scui_color32_t color     = color_cur;
            scui_color32_mix_with(&color, &color_cur, &color_tar, 100 - scui_ui_res_led->led_breath_pct);
            
            for (scui_coord_t idx = 0; idx < LED_NUM; idx++)
                scui_obj_led_color(scui_ui_res_led->led[idx], color, SCUI_COLOR32_MAKE32(led_color_off));
        }
        break;
    }
    default:
        break;
    }
}

/*@brief obj_none_1 控件事件响应回调(Test Empty)
 *@param event 事件
 */
void scui_test_ui_object_obj_none_1_event_proc(scui_event_t *event)
{
    switch (event->type) {
    default:
        break;
    }
}

/*@brief obj_none_2 控件事件响应回调(Test Empty)
 *@param event 事件
 */
void scui_test_ui_object_obj_none_2_event_proc(scui_event_t *event)
{
    switch (event->type) {
    default:
        break;
    }
}
