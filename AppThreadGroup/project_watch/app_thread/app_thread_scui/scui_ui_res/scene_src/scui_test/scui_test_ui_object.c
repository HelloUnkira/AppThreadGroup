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

/* obj_cht 窗口: 两行(柱状/折线)x三列(AUTO_W/跟手滚动/循环) */
#define CHT_ROW_NUM         (2)
#define CHT_COL_NUM         (3)
#define CHT_CELL_W          (150)   /* 单元宽(即列宽) */
#define CHT_CELL_H          (207)   /* 单元高(即行高) */
#define CHT_CELL_X          (4)     /* 单元起点x */
#define CHT_CELL_Y          (44)    /* 单元起点y */
#define CHT_CELL_GAP        (4)     /* 单元间距 */

/* obj_cht 条目数量(环容量)与步进: 内容宽 = CHT_CELL_NUM * CHT_CELL_STEP */
#define CHT_CELL_NUM        (60)
#define CHT_CELL_STEP       (10)

/* obj_cht 测试波形: 标准ECG(窦性心律75bpm, RR=800ms)
 * 高斯合成模型(波中心ms/标准差ms/幅值mV): P150/25/+0.15 Q250/14/-0.12
 *                                        R300/14/+1.10 S350/14/-0.30 T500/50/+0.35
 * 按50ms采样(一周期16样本); 映射: 基线45, 1mV≈45单位(value_min..value_max=0..100)
 */
#define CHT_ECG_NUM         (16)
static const scui_coord_t scui_test_ui_object_cht_ecg[CHT_ECG_NUM] = {
    45, 45, 46, 52, 46, 40, 94, 32, 47, 55, 61, 55, 47, 45, 45, 45,
};

/* 列: 0=AUTO_W(自动宽, 值只给一次); 1=跟手滚动(值只给一次); 2=循环(每秒推进一格) */
static const struct {
    bool         auto_w;    /* 宽度交由AUTO_W解析(撑满父级可视区) */
    scui_coord_t number;    /* 环容量(条目数): 循环列取"数据+gap=整环宽" */
    scui_coord_t gap;       /* 写头留白宽度(像素); 0: 取步进为基准 */
    scui_coord_t feed_ms;   /* 推送节拍(ms); 0: 仅初始给一次值 */
} scui_test_ui_object_cht_col[CHT_COL_NUM] = {
    {.auto_w = true,  .number = CHT_CELL_NUM,               .gap = 0,
        .feed_ms = 0},
    {.auto_w = false, .number = CHT_CELL_NUM,               .gap = 0,
        .feed_ms = 0},
    {.auto_w = false, .number = CHT_CELL_W / CHT_CELL_STEP, .gap = CHT_CELL_STEP * 3 / 2,
        .feed_ms = 100},   /* 15格x10=150 铺满控件 */
};

/* 行: 0=柱状(随机数); 1=折线(ECG模拟) */
static const struct {
    scui_coord_t type;      /* 0:柱状; 1:折线 */
    uint32_t     color;     /* 条目颜色 */
} scui_test_ui_object_cht_row[CHT_ROW_NUM] = {
    {.type = 0, .color = 0xFFFF0000},
    {.type = 1, .color = 0xFF2196F3},
};

/* obj_cht 窗口局部资源 */
static struct {
    scui_handle_t list[CHT_ROW_NUM][CHT_COL_NUM];   /* 循环图控件(行:柱状/折线 列:AUTO_W/跟手/循环) */
    uint32_t      tick[CHT_COL_NUM];                /* 各列推送节拍累积(ms) */
    scui_coord_t  pos;                              /* 波表相位(0~CHT_ECG_NUM-1) */
} * scui_ui_res_cht = NULL;

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

/* obj_bmat 窗口: 条目数量上限(取用例最大条目数) */
#define BMAT_ITEM_MAX       (12)

/* 用例1: 3轨4列(等分) */
static scui_coord_t scui_test_ui_object_bmat_unit_1[12] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
static scui_coord_t scui_test_ui_object_bmat_row_1[12]  = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2};
/* 用例2: 2轨3列(单位1:2:1) */
static scui_coord_t scui_test_ui_object_bmat_unit_2[6]  = {1, 2, 1, 1, 2, 1};
static scui_coord_t scui_test_ui_object_bmat_row_2[6]   = {0, 0, 0, 1, 1, 1};
/* 用例3: 1轨5列(等分) */
static scui_coord_t scui_test_ui_object_bmat_unit_3[5]  = {1, 1, 1, 1, 1};
static scui_coord_t scui_test_ui_object_bmat_row_3[5]   = {0, 0, 0, 0, 0};

/* 条目序号 */
static const char * const scui_test_ui_object_bmat_text[BMAT_ITEM_MAX] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11",
};

/* 用例配置(控件区域在构建期现场给出: SCUI_AREA_MAKE_BM不是常量) */
static const struct {
    scui_coord_t  item_num;     /* 条目数量 */
    scui_coord_t  row_num;      /* 轨道数量 */
    scui_point_t  gap;          /* 条目间距 */
    scui_coord_t *item_unit;    /* 条目宽度(单位数) */
    scui_coord_t *item_row;     /* 条目轨道(行号) */
} scui_test_ui_object_bmat_cfg[3] = {
    {12, 3, {.x = 4, .y = 4}, scui_test_ui_object_bmat_unit_1, scui_test_ui_object_bmat_row_1},
    { 6, 2, {.x = 8, .y = 8}, scui_test_ui_object_bmat_unit_2, scui_test_ui_object_bmat_row_2},
    { 5, 1, {.x = 2, .y = 2}, scui_test_ui_object_bmat_unit_3, scui_test_ui_object_bmat_row_3},
};

/* obj_bmat 窗口局部资源 */
static struct {
    scui_handle_t bmat[3];  /* 矩阵控件 */
    bool          ready;    /* 条目文本构建标记 */
} * scui_ui_res_bmat = NULL;

/* obj_bmat_keyboard 窗口: 按键数量上限(取word/sym键盘按键数) */
#define BMAT_KEYBOARD_ITEM_MAX  (32)

/* obj_bmat_keyboard 窗口局部资源 */
static struct {
    scui_handle_t bmat;                                 /* 矩阵控件 */
    scui_handle_t string[BMAT_KEYBOARD_ITEM_MAX];       /* 键面控件 */
    char          text[BMAT_KEYBOARD_ITEM_MAX][2];      /* 键面文本 */
    scui_coord_t  type_idx;                             /* 布局序号 */
    bool          ready;                                /* 键面文本构建标记 */
} * scui_ui_res_bmat_keyboard = NULL;

/* obj_bmat_calendar 窗口: 单元格数量上限(月视图容量) */
#define BMAT_CALENDAR_ITEM_MAX  (42)

/* 锚定日期与今日(框架无RTC, 由调用方给定) */
#define BMAT_CALENDAR_YEAR      (2026)
#define BMAT_CALENDAR_MONTH     (10)
#define BMAT_CALENDAR_DAY       (1)

/* obj_bmat_calendar 窗口局部资源 */
static struct {
    scui_handle_t bmat;                                 /* 矩阵控件 */
    scui_handle_t string[BMAT_CALENDAR_ITEM_MAX];       /* 单元格控件 */
    char          text[BMAT_CALENDAR_ITEM_MAX][3];      /* 单元格文本 */
    scui_calendar_item_t item[BMAT_CALENDAR_ITEM_MAX];  /* 单元格条目 */
    scui_coord_t  type_idx;                             /* 布局序号 */
    bool          ready;                                /* 单元格文本构建标记 */
} * scui_ui_res_bmat_calendar = NULL;

/* obj_line 窗口: 轨迹图(坐标模式) */
#define LINE_TRACK_SEG_NUM  (4)
#define LINE_TRACK_DOT_NUM  (21)
static scui_coord_t scui_test_ui_object_line_track_seg[LINE_TRACK_SEG_NUM] = {12, 3, 2, 4};

/* obj_line 窗口: 径向图(极坐标模式) */
#define LINE_RADIAL_SEG_NUM (3)
#define LINE_RADIAL_DOT_NUM (63)
static scui_coord_t scui_test_ui_object_line_radial_seg[LINE_RADIAL_SEG_NUM] = {13, 25, 25};

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
        case SCUI_UI_SCENE_TEST_UI_OBJ_BMAT_TITLE:
            text = "Test Bmat";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJ_BMAT_KEYBOARD_TITLE:
            text = "Test Bmat Keyboard";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJ_BMAT_CALENDAR_TITLE:
            text = "Test Bmat Calendar";
            break;
        case SCUI_UI_SCENE_TEST_UI_OBJ_LINE_TITLE:
            text = "Test Line";
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
        
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_cht));
        scui_window_local_res_get(event->object, (void **)&scui_ui_res_cht);
        scui_ui_res_cht->pos = 0;
        
        /* 两行(柱状/折线)x三列(AUTO_W/跟手滚动/循环): 逗号即三个独立控件 */
        /* AUTO_W = 撑满父级可视宽(内容超出部即跟手行程, 见ofs_max) */
        scui_obj_cht_res_t obj_chart_res = {0};
        obj_chart_res.alpha = scui_alpha_cover;
        
        for (scui_coord_t row = 0; row < CHT_ROW_NUM; row++)
        for (scui_coord_t col = 0; col < CHT_COL_NUM; col++) {
            scui_handle_t cell_handle = SCUI_HANDLE_INVALID;
            scui_handle_t handle      = SCUI_HANDLE_INVALID;
            scui_coord_t  type        = scui_test_ui_object_cht_row[row].type;
            scui_coord_t  number      = scui_test_ui_object_cht_col[col].number;
            /* 循环列: 整环(数据+gap)刚好落在格内, 逐格推进 */
            bool          loop        = scui_test_ui_object_cht_col[col].feed_ms != 0;
            
            /* 单元容器: AUTO_W 须按列宽解析(父级可视宽 = 列宽) */
            scui_custom_maker_define(cell_maker);
            cell_maker.widget.parent    = event->object;
            cell_maker.widget.child_num = 1;
            cell_maker.widget.clip      = SCUI_AREA_MAKE_BM(
                CHT_CELL_X + col * (CHT_CELL_W + CHT_CELL_GAP),
                CHT_CELL_Y + row * (CHT_CELL_H + CHT_CELL_GAP),
                CHT_CELL_W, CHT_CELL_H);
            scui_widget_create(&cell_maker, &cell_handle);
            
            scui_obj_cht_maker_define(obj_chart_maker);
            obj_chart_maker.widget.color.color.full = 0xFF808080;
            obj_chart_maker.widget.style.fully_bg = 1;
            obj_chart_maker.widget.parent = cell_handle;
            obj_chart_maker.type          = type;
            obj_chart_maker.step          = CHT_CELL_STEP;
            obj_chart_maker.number        = number;
            obj_chart_maker.loop          = loop;
            /* 循环: 写头留白(像素宽; 0=取步进基准) */
            obj_chart_maker.gap           = scui_test_ui_object_cht_col[col].gap;
            /* AUTO_W列: 宽度交由布局解析; 其余列: 显式列宽 */
            obj_chart_maker.widget.clip   = SCUI_AREA_MAKE_BM(0, 0,
                scui_test_ui_object_cht_col[col].auto_w ? SCUI_WIDGET_AUTO_W : CHT_CELL_W,
                CHT_CELL_H);
            scui_widget_create(&obj_chart_maker, &handle);
            
            obj_chart_res.color.color.full = scui_test_ui_object_cht_row[row].color;
            if (type == 0) {
                /* 柱状: 条宽<步进 */
                obj_chart_res.part  = scui_object_part_rect_fg;
                obj_chart_res.form  = scui_object_form_rect_base;
                obj_chart_res.width = CHT_CELL_STEP / 2;
                obj_chart_res.round = true;
                obj_chart_res.grad  = false;
            } else {
                /* 折线: 线宽<<步进 */
                obj_chart_res.part  = scui_object_part_line_item;
                obj_chart_res.form  = 0;
                obj_chart_res.width = 2;
                obj_chart_res.round = true;
                obj_chart_res.grad  = true;
            }
            scui_obj_cht_style(handle, &obj_chart_res);
            
            if (loop) {
                /* 循环列: 预填整环(此后按节拍逐格推进, 拖动查看超界) */
                for (scui_coord_t idx = 0; idx < number; idx++)
                    if (type == 0)
                        scui_obj_cht_loop_push(handle, 0, scui_rand(100));
                    else
                        scui_obj_cht_loop_push(handle,
                            scui_test_ui_object_cht_ecg[idx % CHT_ECG_NUM], 0);
            } else {
                /* 其余列: 只初始给一次值(不参与循环) */
                scui_coord_t vlist_min[CHT_CELL_NUM] = {0};
                scui_coord_t vlist_max[CHT_CELL_NUM] = {0};
                for (scui_coord_t idx = 0; idx < number; idx++) {
                    if (type == 0)
                        vlist_max[idx] = scui_rand(100);
                    else
                        vlist_min[idx] = scui_test_ui_object_cht_ecg[idx % CHT_ECG_NUM];
                }
                if (type == 0)
                    scui_obj_cht_hist_data(handle, vlist_min, vlist_max);
                else
                    scui_obj_cht_line_data(handle, vlist_min);
            }
            
            scui_ui_res_cht->list[row][col] = handle;
        }
        break;
    }
    case scui_event_anima_elapse: {
        
        /* 按列节拍推进(0: 静态列, 不参与) */
        for (scui_coord_t col = 0; col < CHT_COL_NUM; col++) {
            if (scui_test_ui_object_cht_col[col].feed_ms == 0)
                continue;
            
            scui_ui_res_cht->tick[col] += event->tick;
            if (scui_ui_res_cht->tick[col] < scui_test_ui_object_cht_col[col].feed_ms)
                continue;
            scui_ui_res_cht->tick[col] = 0;
            
            /* 标准ECG: 波表按相位取值 */
            scui_coord_t ecg = scui_test_ui_object_cht_ecg[scui_ui_res_cht->pos++];
            if (scui_ui_res_cht->pos >= scui_arr_len(scui_test_ui_object_cht_ecg))
                scui_ui_res_cht->pos = 0;
            
            for (scui_coord_t row = 0; row < CHT_ROW_NUM; row++) {
                scui_handle_t handle = scui_ui_res_cht->list[row][col];
                /* 柱状: 随机数(自0生长); 折线: ECG模拟(值即点高) */
                if (scui_test_ui_object_cht_row[row].type == 0)
                    scui_obj_cht_loop_push(handle, 0, scui_rand(100));
                else
                    scui_obj_cht_loop_push(handle, ecg, 0);
            }
        }
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

/*@brief obj_bmat 控件事件响应回调(Test Bmat)
 *@param event 事件
 */
void scui_test_ui_object_obj_bmat_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_bmat));
        scui_window_local_res_get(event->object, (void **)&scui_ui_res_bmat);
        scui_ui_res_bmat->ready = false;

        /* 控件区域: 纵向三等分区(避开标题栏) */
        scui_area_t bmat_clip[3] = {
            SCUI_AREA_MAKE_BM(10,  50, 446, 130),
            SCUI_AREA_MAKE_BM(10, 190, 446, 130),
            SCUI_AREA_MAKE_BM(10, 330, 446, 130),
        };

        for (scui_coord_t idx = 0; idx < scui_arr_len(scui_test_ui_object_bmat_cfg); idx++) {

            scui_obj_bmat_maker_define(bmat_maker);
            scui_handle_t bmat_handle = SCUI_HANDLE_INVALID;

            bmat_maker.widget.parent    = event->object;
            bmat_maker.widget.clip      = bmat_clip[idx];
            bmat_maker.widget.child_num = scui_test_ui_object_bmat_cfg[idx].item_num;
            bmat_maker.item_num         = scui_test_ui_object_bmat_cfg[idx].item_num;
            bmat_maker.row_num          = scui_test_ui_object_bmat_cfg[idx].row_num;
            bmat_maker.gap              = scui_test_ui_object_bmat_cfg[idx].gap;
            scui_widget_create(&bmat_maker, &bmat_handle);
            scui_ui_res_bmat->bmat[idx] = bmat_handle;

            /* 条目序列: 宽度与轨道(内部持有, 区域由bmat布局解析) */
            scui_obj_bmat_item_set(bmat_handle, scui_test_ui_object_bmat_cfg[idx].item_num,
                scui_test_ui_object_bmat_cfg[idx].row_num, scui_test_ui_object_bmat_cfg[idx].item_unit,
                scui_test_ui_object_bmat_cfg[idx].item_row);
        }
        break;
    }
    case scui_event_anima_elapse: {
        if (scui_ui_res_bmat->ready)
            break;
        /* 条目布局未解析(首帧)时跳过, 待bmat绘制解析出条目区域 */
        scui_area_t area_first = {0};
        scui_obj_bmat_item_area(scui_ui_res_bmat->bmat[0], 0, &area_first);
        if (area_first.w == 0)
            break;
        scui_ui_res_bmat->ready = true;

        for (scui_coord_t idx = 0; idx < scui_arr_len(scui_test_ui_object_bmat_cfg); idx++) {
            scui_coord_t item_num = scui_test_ui_object_bmat_cfg[idx].item_num;
            SCUI_LOG_INFO("bmat[%d] num:%d row_num:%d gap:%d",
                idx, item_num, scui_test_ui_object_bmat_cfg[idx].row_num,
                scui_test_ui_object_bmat_cfg[idx].gap.x);

            for (scui_coord_t item = 0; item < item_num; item++) {
                scui_area_t area = {0};
                scui_obj_bmat_item_area(scui_ui_res_bmat->bmat[idx], item, &area);
                SCUI_LOG_INFO("bmat[%d][%2d] x:%d y:%d w:%d h:%d",
                    idx, item, area.x, area.y, area.w, area.h);

                /* 条目序号: 条目区域由bmat布局给出(控件相对) */
                scui_string_maker_define(string_maker);
                scui_handle_t string_handle = SCUI_HANDLE_INVALID;

                string_maker.widget.parent  = scui_ui_res_bmat->bmat[idx];
                string_maker.widget.clip    = area;
                string_maker.font_idx       = SCUI_FONT_IDX_36;
                string_maker.args.lang      = scui_lang_type_en;
                string_maker.args.align_hor = 2;
                string_maker.args.align_ver = 2;
                string_maker.args.color     = SCUI_COLOR_MAKE32_SE(true, 0, 0xFFFFFFFF, 0xFFFFFFFF);
                scui_widget_create(&string_maker, &string_handle);
                scui_string_update_str(string_handle, (uint8_t *)scui_test_ui_object_bmat_text[item]);
            }
        }
        break;
    }
    default:
        break;
    }
}

/*@brief obj_bmat_keyboard 控件事件响应回调(Test Bmat Keyboard)
 *@param event 事件
 */
void scui_test_ui_object_obj_bmat_keyboard_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_bmat_keyboard));
        scui_window_local_res_get(event->object, (void **)&scui_ui_res_bmat_keyboard);
        scui_ui_res_bmat_keyboard->type_idx = scui_keyboard_type_num;
        scui_ui_res_bmat_keyboard->ready    = false;

        /* 条目与子控件按布局上限(word/sym按键数最多) */
        scui_obj_bmat_maker_define(bmat_maker);
        scui_handle_t bmat_handle = SCUI_HANDLE_INVALID;

        bmat_maker.widget.parent    = event->object;
        bmat_maker.widget.clip      = SCUI_AREA_MAKE_BM(10, 50, 446, 396);
        bmat_maker.widget.child_num = BMAT_KEYBOARD_ITEM_MAX;
        bmat_maker.item_num         = BMAT_KEYBOARD_ITEM_MAX;
        bmat_maker.row_num          = 4;
        bmat_maker.gap              = (scui_point_t){.x = 6, .y = 6};
        scui_widget_create(&bmat_maker, &bmat_handle);
        scui_ui_res_bmat_keyboard->bmat = bmat_handle;
        break;
    }
    case scui_event_anima_elapse: {
        if (scui_ui_res_bmat_keyboard->ready)
            break;

        /* keyboard只输出布局与辅助信息: 单位与轨道直接驱动bmat */
        scui_keyboard_layout_t layout = {0};
        scui_keyboard_layout(scui_ui_res_bmat_keyboard->type_idx, false, &layout);
        SCUI_LOG_INFO("keyboard type:%d num:%d row_num:%d",
            scui_ui_res_bmat_keyboard->type_idx, layout.num, layout.row_num);
        SCUI_ASSERT(layout.num <= BMAT_KEYBOARD_ITEM_MAX);

        scui_coord_t unit[BMAT_KEYBOARD_ITEM_MAX] = {0};
        scui_coord_t row[BMAT_KEYBOARD_ITEM_MAX]  = {0};

        for (scui_coord_t idx = 0; idx < layout.num; idx++) {
            unit[idx] = layout.item[idx].unit;
            row[idx]  = layout.item[idx].row;
            /* 键面: 可打印键取字符本身(键面本地化不属于keyboard职责) */
            uint32_t code = layout.item[idx].code;
            scui_ui_res_bmat_keyboard->text[idx][0] = (code >= 0x20 && code <= 0x7e) ? (char)code : '?';
            scui_ui_res_bmat_keyboard->text[idx][1] = '\0';
        }
        scui_obj_bmat_item_set(scui_ui_res_bmat_keyboard->bmat,
            layout.num, layout.row_num, unit, row);

        /* 条目布局未解析(首帧)时跳过, 待bmat绘制解析出条目区域 */
        scui_area_t area_first = {0};
        scui_obj_bmat_item_area(scui_ui_res_bmat_keyboard->bmat, 0, &area_first);
        if (area_first.w == 0)
            break;
        scui_ui_res_bmat_keyboard->ready = true;

        for (scui_coord_t idx = 0; idx < layout.num; idx++) {
            scui_area_t area = {0};
            scui_obj_bmat_item_area(scui_ui_res_bmat_keyboard->bmat, idx, &area);
            SCUI_LOG_INFO("keyboard[%2d] x:%d y:%d w:%d h:%d",
                idx, area.x, area.y, area.w, area.h);

            scui_string_maker_define(string_maker);
            scui_handle_t string_handle = SCUI_HANDLE_INVALID;

            string_maker.widget.parent  = scui_ui_res_bmat_keyboard->bmat;
            string_maker.widget.clip    = area;
            string_maker.font_idx       = SCUI_FONT_IDX_36;
            string_maker.args.lang      = scui_lang_type_en;
            string_maker.args.align_hor = 2;
            string_maker.args.align_ver = 2;
            string_maker.args.color     = SCUI_COLOR_MAKE32_SE(true, 0, 0xFFFFFFFF, 0xFFFFFFFF);
            scui_widget_create(&string_maker, &string_handle);
            scui_string_update_str(string_handle, (uint8_t *)scui_ui_res_bmat_keyboard->text[idx]);
            scui_ui_res_bmat_keyboard->string[idx] = string_handle;
        }
        break;
    }
    case scui_event_key_click: {
        if (event->key_id != scui_event_key_val_enter)
            break;
        scui_event_mask_over(event);

        /* 键面控件随布局条目数变化: 先销毁旧的, 待下一帧按新布局重建 */
        scui_keyboard_layout_t layout = {0};
        scui_keyboard_layout(scui_ui_res_bmat_keyboard->type_idx, false, &layout);
        for (scui_coord_t idx = 0; idx < layout.num; idx++)
            scui_widget_destroy(scui_ui_res_bmat_keyboard->string[idx]);

        scui_coord_t type_idx = scui_ui_res_bmat_keyboard->type_idx + 1;
        if (type_idx > scui_keyboard_type_sym)
            type_idx = scui_keyboard_type_num;
        scui_ui_res_bmat_keyboard->type_idx = type_idx;
        scui_ui_res_bmat_keyboard->ready    = false;
        break;
    }
    default:
        break;
    }
}

/*@brief obj_bmat_calendar 控件事件响应回调(Test Bmat Calendar)
 *@param event 事件
 */
void scui_test_ui_object_obj_bmat_calendar_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_bmat_calendar));
        scui_window_local_res_get(event->object, (void **)&scui_ui_res_bmat_calendar);
        scui_ui_res_bmat_calendar->type_idx = scui_calendar_type_week;
        scui_ui_res_bmat_calendar->ready    = false;

        /* 条目与子控件按布局上限(月视图单元格数最多) */
        scui_obj_bmat_maker_define(bmat_maker);
        scui_handle_t bmat_handle = SCUI_HANDLE_INVALID;

        bmat_maker.widget.parent    = event->object;
        bmat_maker.widget.clip      = SCUI_AREA_MAKE_BM(10, 50, 446, 396);
        bmat_maker.widget.child_num = BMAT_CALENDAR_ITEM_MAX;
        bmat_maker.item_num         = BMAT_CALENDAR_ITEM_MAX;
        bmat_maker.row_num          = 6;
        bmat_maker.gap              = (scui_point_t){.x = 4, .y = 4};
        scui_widget_create(&bmat_maker, &bmat_handle);
        scui_ui_res_bmat_calendar->bmat = bmat_handle;
        break;
    }
    case scui_event_anima_elapse: {
        if (scui_ui_res_bmat_calendar->ready)
            break;

        scui_calendar_date_t date = {0};
        scui_calendar_date_set(&date, BMAT_CALENDAR_YEAR,
            BMAT_CALENDAR_MONTH, BMAT_CALENDAR_DAY);

        /* calendar只输出布局与辅助信息: 轨道划分由列数决定 */
        scui_calendar_layout_t layout = {0};
        layout.cap  = BMAT_CALENDAR_ITEM_MAX;
        layout.item = scui_ui_res_bmat_calendar->item;
        scui_calendar_layout(scui_ui_res_bmat_calendar->type_idx, true, &date, &date, &layout);
        SCUI_LOG_INFO("calendar type:%d row_num:%d col_num:%d num:%d",
            scui_ui_res_bmat_calendar->type_idx, layout.row_num, layout.col_num, layout.num);
        SCUI_ASSERT(layout.num <= BMAT_CALENDAR_ITEM_MAX);

        scui_coord_t unit[BMAT_CALENDAR_ITEM_MAX] = {0};
        scui_coord_t row[BMAT_CALENDAR_ITEM_MAX]  = {0};

        for (scui_coord_t idx = 0; idx < layout.num; idx++) {
            unit[idx] = 1;
            row[idx]  = idx / layout.col_num;
            /* 年视图取月份, 其余取日号 */
            scui_coord_t value = scui_ui_res_bmat_calendar->type_idx == scui_calendar_type_year ?
                layout.item[idx].month : layout.item[idx].day;
            snprintf(scui_ui_res_bmat_calendar->text[idx], sizeof(scui_ui_res_bmat_calendar->text[idx]),
                "%d", value);
        }
        scui_obj_bmat_item_set(scui_ui_res_bmat_calendar->bmat,
            layout.num, layout.row_num, unit, row);

        /* 条目布局未解析(首帧)时跳过, 待bmat绘制解析出条目区域 */
        scui_area_t area_first = {0};
        scui_obj_bmat_item_area(scui_ui_res_bmat_calendar->bmat, 0, &area_first);
        if (area_first.w == 0)
            break;
        scui_ui_res_bmat_calendar->ready = true;

        for (scui_coord_t idx = 0; idx < layout.num; idx++) {
            scui_area_t area = {0};
            scui_obj_bmat_item_area(scui_ui_res_bmat_calendar->bmat, idx, &area);
            SCUI_LOG_INFO("calendar[%2d] x:%d y:%d w:%d h:%d",
                idx, area.x, area.y, area.w, area.h);

            /* 归属当前视图用亮色, 今日用红色, 前后补位用暗色 */
            uint32_t color = 0xFFFFFFFF;
            if (layout.item[idx].today)
                color = 0xFFFF0000;
            else if (!layout.item[idx].cur)
                color = 0xFF606060;

            scui_string_maker_define(string_maker);
            scui_handle_t string_handle = SCUI_HANDLE_INVALID;

            string_maker.widget.parent  = scui_ui_res_bmat_calendar->bmat;
            string_maker.widget.clip    = area;
            string_maker.font_idx       = SCUI_FONT_IDX_36;
            string_maker.args.lang      = scui_lang_type_en;
            string_maker.args.align_hor = 2;
            string_maker.args.align_ver = 2;
            string_maker.args.color     = SCUI_COLOR_MAKE32_SE(true, 0, color, color);
            scui_widget_create(&string_maker, &string_handle);
            scui_string_update_str(string_handle, (uint8_t *)scui_ui_res_bmat_calendar->text[idx]);
            scui_ui_res_bmat_calendar->string[idx] = string_handle;
        }
        break;
    }
    case scui_event_key_click: {
        if (event->key_id != scui_event_key_val_enter)
            break;
        scui_event_mask_over(event);

        /* 单元格控件随布局条目数变化: 先销毁旧的, 待下一帧按新布局重建 */
        scui_calendar_date_t date = {0};
        scui_calendar_date_set(&date, BMAT_CALENDAR_YEAR,
            BMAT_CALENDAR_MONTH, BMAT_CALENDAR_DAY);

        scui_calendar_layout_t layout = {0};
        layout.cap  = BMAT_CALENDAR_ITEM_MAX;
        layout.item = scui_ui_res_bmat_calendar->item;
        scui_calendar_layout(scui_ui_res_bmat_calendar->type_idx, true, &date, &date, &layout);
        for (scui_coord_t idx = 0; idx < layout.num; idx++)
            scui_widget_destroy(scui_ui_res_bmat_calendar->string[idx]);

        scui_coord_t type_idx = scui_ui_res_bmat_calendar->type_idx + 1;
        if (type_idx >= scui_calendar_type_num)
            type_idx = scui_calendar_type_week;
        scui_ui_res_bmat_calendar->type_idx = type_idx;
        scui_ui_res_bmat_calendar->ready    = false;
        break;
    }
    default:
        break;
    }
}

/*@brief obj_line 控件事件响应回调(Test Line)
 *@param event 事件
 */
void scui_test_ui_object_obj_line_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {

        /* 轨迹: 正弦波(连续1段) + 三段离散短线 */
        scui_point_t track_dot[LINE_TRACK_DOT_NUM] = {0};
        for (scui_coord_t idx = 0; idx < 12; idx++)
            track_dot[idx] = (scui_point_t){.x = 445 * idx / 11,
                .y = 95 + scui_sin4096(idx * 30) * 80 / 4096};
        track_dot[12] = (scui_point_t){.x =  20, .y = 20};
        track_dot[13] = (scui_point_t){.x =  80, .y = 60};
        track_dot[14] = (scui_point_t){.x = 140, .y = 20};
        track_dot[15] = (scui_point_t){.x = 200, .y = 20};
        track_dot[16] = (scui_point_t){.x = 260, .y = 60};
        track_dot[17] = (scui_point_t){.x = 320, .y = 60};
        track_dot[18] = (scui_point_t){.x = 380, .y = 20};
        track_dot[19] = (scui_point_t){.x = 440, .y = 60};
        track_dot[20] = (scui_point_t){.x = 440, .y = 20};

        scui_obj_line_maker_define(track_maker);
        scui_handle_t track_handle = SCUI_HANDLE_INVALID;

        track_maker.widget.parent = event->object;
        track_maker.widget.clip   = SCUI_AREA_MAKE_BM(10, 50, 446, 190);
        track_maker.seg_num       = LINE_TRACK_SEG_NUM;
        track_maker.dot_num       = LINE_TRACK_DOT_NUM;
        scui_widget_create(&track_maker, &track_handle);

        scui_obj_line_res_t track_res = {0};
        track_res.part  = scui_object_part_line_item;
        track_res.alpha = scui_alpha_cover;
        track_res.color.color.full = 0xFF2196F3;
        track_res.width = 4;
        track_res.round = true;
        scui_obj_line_style(track_handle, &track_res);
        scui_obj_line_data_set(track_handle, LINE_TRACK_SEG_NUM, LINE_TRACK_DOT_NUM,
            scui_test_ui_object_line_track_seg, track_dot);

        /* 径向: 数据多边形 + 内外圈(均闭合) */
        scui_point_t radial_dot[LINE_RADIAL_DOT_NUM] = {0};
        scui_coord_t dot_i = 0;
        for (scui_coord_t idx = 0; idx < 13; idx++)
            radial_dot[dot_i++] = (scui_point_t){.x = idx * 30, .y = 55 + (idx % 3) * 20};
        for (scui_coord_t idx = 0; idx < 25; idx++)
            radial_dot[dot_i++] = (scui_point_t){.x = idx * 15, .y = 95};
        for (scui_coord_t idx = 0; idx < 25; idx++)
            radial_dot[dot_i++] = (scui_point_t){.x = idx * 15, .y = 55};

        scui_obj_line_maker_define(radial_maker);
        scui_handle_t radial_handle = SCUI_HANDLE_INVALID;

        radial_maker.widget.parent = event->object;
        radial_maker.widget.clip   = SCUI_AREA_MAKE_BM(10, 250, 446, 196);
        radial_maker.mode          = 1;
        radial_maker.seg_num       = LINE_RADIAL_SEG_NUM;
        radial_maker.dot_num       = LINE_RADIAL_DOT_NUM;
        scui_widget_create(&radial_maker, &radial_handle);

        scui_obj_line_res_t radial_res = {0};
        radial_res.part  = scui_object_part_line_item;
        radial_res.alpha = scui_alpha_cover;
        radial_res.color.color.full = 0xFFFF0000;
        radial_res.width = 3;
        radial_res.round = true;
        scui_obj_line_style(radial_handle, &radial_res);
        scui_obj_line_data_set(radial_handle, LINE_RADIAL_SEG_NUM, LINE_RADIAL_DOT_NUM,
            scui_test_ui_object_line_radial_seg, radial_dot);
        break;
    }
    default:
        break;
    }
}
