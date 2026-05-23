#ifndef APP_LV_VIEW_PLUG_H
#define APP_LV_VIEW_PLUG_H

typedef enum {
    ui_plug_global_event_none = 0,
    // 全局弹窗事件枚举量-------------->起始点
    
    
    
    ui_plug_global_event_popup_num,
    // 全局弹窗事件枚举量-------------->结束点
    
    
    
    /*!!!不需要触发弹窗的全局事件, 移到下面来!!!
     *节约上面的量数量信息, 比如说关闭一个可能存在的弹窗事件
     */
    
    
    
    
    
    // 进出低功耗模式
    ui_plug_global_event_sleep_enter,
    ui_plug_global_event_sleep_exit,
    ui_plug_global_event_deep_sleep_enter,      // 待定研究中
    ui_plug_global_event_deep_sleep_exit,       // 待定研究中 
    
} ui_plug_global_event_t;

typedef struct {
    uint32_t page_id;       // 界面id号
    uint32_t prio;          // 优先级, 数字越大优先级越高
} ui_plug_global_popup_t;



#define APP_LV_VIEW_CHECK_TIME_OVER_MAX  0xFF       /* 一直界面常亮 */
#define APP_LV_VIEW_CHECK_TIME_IDLE_MAX  0xFF       /* 一直空闲等待(息屏后不自动返回主界面) */
#define APP_LV_VIEW_CHECK_TIME_OVER      5          /* 界面空闲亮屏等待时间 */
#define APP_LV_VIEW_CHECK_TIME_IDLE      5          /* 息屏后返回主界面空闲等待时间 */
#define APP_LV_VIEW_CHECK_TIME_USE       0          /* 模组禁用 */



typedef struct {
    bool    dlps;    /* 需要进入dlps */
    bool    back;    /* 需要回退界面 */
    bool    once;     /* 单次时间设置 */
    uint8_t over_time;
    uint8_t idle_time;
    uint8_t over_time_once;
    uint8_t idle_time_once;
    uint8_t over_time_bak;
    uint8_t idle_time_bak;
    uint8_t over_time_def;    // 默认息屏时间
    uint8_t idle_time_def;    // 默认息屏后回到主界面时间
    bool sleep_tag:1;        // 低功耗标记
    bool dlps_tag:1;        // 低功耗标记
    // 暂定用lv_timer_t，后续应该去掉
    // 或者换成rtos的timer
    lv_timer_t *timer;
} ui_plug_check_time_t;

extern lv_coord_t s_global_swipe_touch_x0;
extern lv_coord_t s_global_swipe_touch_y0;

/* 更变状况的进出低功耗模式 */
void ui_plug_check_time_enter_deep_sleep(void);
void ui_plug_check_time_exit_deep_sleep(void);
void ui_plug_check_time_enter_sleep(void);
void ui_plug_check_time_exit_sleep(void);

void ui_plug_check_time_ready(void);
void ui_plug_check_time_pause(void);
void ui_plug_check_time_resume(void);
void ui_plug_check_time_reset(uint8_t over_time, uint8_t idle_time);
void ui_plug_check_time_def_reset(uint8_t over_time, uint8_t idle_time);
void ui_plug_check_time_once_reset(uint8_t over_time, uint8_t idle_time);

// 界面常亮:
// ui_plug_check_time_reset(APP_LV_VIEW_CHECK_TIME_OVER_MAX, 0);
// 界面息屏后不自动返回主界面:
// ui_plug_check_time_reset(0, APP_LV_VIEW_CHECK_TIME_IDLE_MAX);
// 恢复默认设置:
// ui_plug_check_time_reset(0, 0);


// 检查当次SHORT_CLICK的真实性
bool ui_plug_watch_global_move_check(void);

// 短按抖动监测, 如果从按下到抬起中所有的点都落在以下数字
// 为一个圆心的区域内, 那么我们认为这符合点击的特点(不可为0)
#define UI_PLUG_WATCH_GLOBAL_SHORT_CLICK_DITHER        (8)


void ui_plug_watch_global_view_event_stop(bool stop);
void ui_plug_watch_global_view_event_cb(lv_event_t * e);

void ui_plug_page_jump_click_event_cb(lv_event_t * e);
void yc_swipe_pressed_cb(lv_event_t * e);
bool yc_is_valid_click(lv_event_t *e);

/* way(0:正转;1:反转),val(步进值1~n) */
void app_lv_view_enc_notify(uintptr_t way, uintptr_t val);

/* 这个模组填充所有外源事件 */
/* 包括且不限于各种界面弹窗等等 */
void ui_plug_global_event_proc_cb(ui_plug_global_event_t global_event, bool sleep_tag);

/* 外部注册回调使用该接口通知事件 */
void ui_plug_global_event_notify(ui_plug_global_event_t global_event);

/* 下按键单击跳转界面设置 */
uint32_t ui_plug_watch_global_key1_click_jump_page_id(uint32_t page_id);

// key0:上按键(和编码器在一起的那个); key1:下按键
// 界面注册按键回调(key_event: key_driver.h文件中: enum key_action), 不使用的时候注册为NULL
void ui_plug_watch_global_key0_cb_register(void (*key_cb)(uint8_t key_event));
void ui_plug_watch_global_key1_cb_register(void (*key_cb)(uint8_t key_event));

// 界面注册编码器回调(way:+1;-1; step:步进值), 不使用的时候注册为NULL
void ui_plug_watch_global_enc_cb_register(void (*enc_cb)(uint8_t way, uint8_t step));

typedef enum {
    app_lv_view_event_custom_swipe_r,        // 右滑返回
    app_lv_view_event_custom_key0,        // 按键0:上按键
    app_lv_view_event_custom_key1,        // 按键1:下按键
    app_lv_view_event_custom_enc,            // 编码器
} app_lv_view_event_custom_type_t;

// 插件:自定义事件:
typedef struct {
    app_lv_view_event_custom_type_t type;
    union {
    
    // 右滑返回事件携带数据
    struct {
        void *occupy;
    } swipe_r;
    
    // 按键事件携带数据
    struct {
        uint8_t event;    // key_driver.h文件中: enum key_action
    } key;
    
    // 编码器事件携带数据
    struct {
        int8_t  way;    // 正转反转:+-1;
        int32_t step;    // 步进值:转了几个格子;
    } enc;
    
    };
    
    // 事件吸收标记
    uint8_t stop:1;
    
} app_lv_view_event_custom_t;

extern uint32_t app_lv_view_event_custom;
extern app_lv_view_event_custom_t app_lv_view_event_custom_param;

// 事件项
#define APP_LV_VIEW_EVENT_CUSTOM                  app_lv_view_event_custom
#define APP_LV_VIEW_EVENT_CUSTOM_PARAM        &app_lv_view_event_custom_param
#define APP_LV_VIEW_EVENT_CUSTOM_STOP(param)    ((param)->stop = true)

void ui_plug_event_ready(void);

// 计算数组长度
#ifndef ARRAY_LEN
#define ARRAY_LEN(array)    (sizeof(array)/sizeof(array[0]))
#endif

#endif
