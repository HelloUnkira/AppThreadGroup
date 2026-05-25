#ifndef LVGL_VIEW_EVENT_H
#define LVGL_VIEW_EVENT_H

// 后四位作为enc_diff值
#define LVGL_VIEW_EVENT_ENC_BDIR        (0xEB0000)
#define LVGL_VIEW_EVENT_ENC_FDIR        (0xEF0000)
#define LVGL_VIEW_EVENT_ENC_MASK        (0x00FFFF)
// 编码器合并等待与切换窗口的值
#define LVGL_VIEW_EVENT_ENC_CNT         (1)
#define LVGL_VIEW_EVENT_ENC_SPAN        (500)


typedef enum
{
    lvgl_view_event_type_enc,           // 编码器
    lvgl_view_event_type_key,           // 按键
    lvgl_view_event_type_swipe_r,       // 右滑返回
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

void lvgl_view_event_ready(void);
void lvgl_view_event_stop(void);
uint32_t lvgl_view_event_id(void);

#endif
