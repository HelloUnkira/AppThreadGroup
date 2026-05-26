#ifndef LVGL_VIEW_EVENT_H
#define LVGL_VIEW_EVENT_H

// 后四位作为enc_diff值
#define LVGL_VIEW_EVENT_ENC_BDIR        (0xEB0000)
#define LVGL_VIEW_EVENT_ENC_FDIR        (0xEF0000)
#define LVGL_VIEW_EVENT_ENC_MASK        (0x00FFFF)
// 编码器合并等待与切换窗口的值
#define LVGL_VIEW_EVENT_ENC_CNT         (1)
#define LVGL_VIEW_EVENT_ENC_SPAN        (500)

// LV_EVENT_SHORT_CLICK抖动监测
// 如果从按下到抬起中所有的点都落在以下数字
// 为一个圆心的区域内, 那么我们认为这符合点击的特点(不可为0)
#define LVGL_VIEW_EVENT_SHORT_CLICK_DITHER          (8)

// 右滑动手势的判定门限
#define LVGL_VIEW_EVENT_SWIPE_R_DX      (60)
#define LVGL_VIEW_EVENT_SWIPE_R_DY      (30)

typedef enum
{
    // 系统事件
    lvgl_view_event_type_enc,           // 编码器
    lvgl_view_event_type_key,           // 按键
    // 系统事件(其他)
    lvgl_view_event_type_sleep_deep_enter,
    lvgl_view_event_type_sleep_deep_exit,
    lvgl_view_event_type_sleep_enter,
    lvgl_view_event_type_sleep_exit,
    
    // 项目补充事件(右滑返回)
    lvgl_view_event_type_swipe_r,
    // 项目补充事件(弹窗提醒...)
    lvgl_view_event_type_popup_s,
    lvgl_view_event_type_remind_xxx,
    // keep add...
    lvgl_view_event_type_popup_e,
    lvgl_view_event_type_popup_num = lvgl_view_event_type_popup_e - lvgl_view_event_type_popup_s - 1,
    
} lvgl_view_event_type_t;

// 插件:自定义事件:
typedef struct
{
    lvgl_view_event_type_t type;
    union
    {
    
        // 右滑返回事件携带数据
        struct
        {
            void *occupy;
        } swipe_r;
        
        // 按键事件携带数据
        struct
        {
            lv_key_t   key_id;
            lv_event_code_t key_event;
        } key;
        
        // 编码器事件携带数据
        struct
        {
            int8_t  way;        // 正转反转:+-1;
            int32_t step;       // 步进值:转了几个格子;
        } enc;
        
    };
    
    // 事件吸收标记
    uint8_t stop: 1;
    
} lvgl_view_event_param_t;

// 注册方式:
// lv_obj_add_event_cb(page, event_cb, lvgl_view_event_id(), NULL);
// lvgl_view_event_param_t *param = lv_event_get_param(e);

uint32_t lvgl_view_event_id(void);
lv_obj_t *lvgl_view_event_obj(void);
void lvgl_view_event_ready(void);

// 外源发送一个全局事件, 到达指定的目标
void lvgl_view_event_send(lvgl_view_event_param_t *param);
void lvgl_view_event_stop(lvgl_view_event_param_t *param);

#endif
