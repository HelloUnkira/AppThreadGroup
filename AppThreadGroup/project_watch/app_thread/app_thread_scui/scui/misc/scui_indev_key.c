/*实现目标:
 *    输入设备事件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

static scui_indev_key_t scui_indev_key = {.item = {0}};

/*@brief 输入设备数据通报
 *@param data 数据
 */
void scui_indev_key_notify(scui_indev_data_t *data)
{
    if (data->key.key_id == -1)
        return;
    
    APP_SYS_LOG_DEBUG("tick:%u", scui_tick_cnt());
    APP_SYS_LOG_DEBUG("type:%d",     data->type);
    APP_SYS_LOG_DEBUG("state:%d",    data->state);
    APP_SYS_LOG_DEBUG("key_id:%d",   data->key.key_id);
    APP_SYS_LOG_DEBUG("key_val:%d",  data->key.key_val);
    
    bool key_id_not_find = true;
    for (uint32_t idx = 0; idx < SCUI_INDEV_KEY_LIMIT; idx++)
        if (scui_indev_key.item[idx].key_id == data->key.key_id) {
            key_id_not_find = false;
            break;
        }
    if (key_id_not_find)
    for (uint32_t idx = 0; idx < SCUI_INDEV_KEY_LIMIT; idx++)
        if (scui_indev_key.item[idx].key_id == -1) {
            scui_indev_key.item[idx].key_id  = data->key.key_id;
            key_id_not_find = false;
            break;
        }
    if (key_id_not_find) {
        APP_SYS_LOG_ERROR("key id is too much:");
        APP_SYS_LOG_ERROR("key_id:%d", data->key.key_id);
        for (uint32_t idx = 0; idx < SCUI_INDEV_KEY_LIMIT; idx++)
            APP_SYS_LOG_ERROR("key_id:%d", scui_indev_key.item[idx].key_id);
        APP_SYS_ASSERT(false);
    }
    
    /* 如果命中按键号,直接处理 */
    for (uint32_t idx = 0; idx < SCUI_INDEV_KEY_LIMIT; idx++)
        if (scui_indev_key.item[idx].key_id == data->key.key_id) {
            scui_indev_key.item[idx].key_id  = data->key.key_id;
            scui_indev_key.item[idx].key_val = data->key.key_val;
            scui_event_t event = {
                .object  = SCUI_HANDLE_SYSTEM,
                .key_id  = scui_indev_key.item[idx].key_id,
                .key_val = scui_indev_key.item[idx].key_val,
            };
            /* 当前状态为release */
            if (data->state == scui_indev_state_release) {
                /* 上一状态为release */
                if (scui_indev_key.item[idx].state == scui_indev_state_release)
                    return;
                /* 上一状态为press */
                if (scui_indev_key.item[idx].state == scui_indev_state_press) {
                    scui_indev_key.item[idx].state  = data->state;
                    scui_indev_key.item[idx].key_cnt++;
                    scui_coord_t elapse = scui_tick_cnt() - scui_indev_key.item[idx].cnt_tick;
                    /* 检查事件是否是点击 */
                    if (elapse < SCUI_INDEV_KEY_CLICK) {
                        event.type    = scui_event_key_click;
                        event.key_cnt = scui_indev_key.item[idx].key_cnt;
                        APP_SYS_LOG_INFO("scui_event_key_click:%d", event.key_cnt);
                        scui_event_notify(&event);
                    }
                    scui_indev_key.item[idx].cnt_tick = scui_tick_cnt();
                    /* 发送抬起事件 */
                    event.type    = scui_event_key_up;
                    event.key_cnt = scui_indev_key.item[idx].key_cnt;
                    APP_SYS_LOG_INFO("scui_event_key_up:%d", event.key_cnt);
                    scui_event_notify(&event);
                    return;
                }
            }
            /* 当前状态为press */
            if (data->state == scui_indev_state_press) {
                /* 上一状态为release */
                if (scui_indev_key.item[idx].state == scui_indev_state_release) {
                    scui_indev_key.item[idx].state  = data->state;
                    scui_coord_t elapse = scui_tick_cnt() - scui_indev_key.item[idx].cnt_tick;
                    scui_indev_key.item[idx].cnt_tick = scui_tick_cnt();
                    /* 检查点击是否连续 */
                    if (elapse >= SCUI_INDEV_KEY_CLICK_SPAN)
                        scui_indev_key.item[idx].key_cnt  = 0;
                    /* 发送按下事件 */
                    event.type     = scui_event_key_down;
                    event.key_tick = elapse;
                    APP_SYS_LOG_INFO("scui_event_key_down:%d", event.key_tick);
                    scui_event_notify(&event);
                    return;
                }
                /* 上一状态为press */
                if (scui_indev_key.item[idx].state == scui_indev_state_press) {
                    scui_indev_key.item[idx].state  = data->state;
                    scui_coord_t elapse = scui_tick_cnt() - scui_indev_key.item[idx].cnt_tick;
                    /* 发送按下事件 */
                    event.type     = scui_event_key_hold,
                    event.key_tick = elapse,
                    APP_SYS_LOG_INFO("scui_event_key_hold:%d", event.key_tick);
                    scui_event_notify(&event);
                    return;
                }
            }
            return;
        }
}

/*@brief 输入设备初始化
 */
void scui_indev_key_ready(void)
{
    for (uint32_t idx = 0; idx < SCUI_INDEV_KEY_LIMIT; idx++)
        scui_indev_key.item[idx].key_id = -1;
}
