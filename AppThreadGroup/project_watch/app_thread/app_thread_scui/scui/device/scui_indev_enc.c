/*实现目标:
 *    输入设备事件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 编码器事件吸收回调
 */
static bool scui_event_enc_absorb(void *evt_old, void *evt_new)
{
    scui_event_t *event_old = evt_old;
    scui_event_t *event_new = evt_new;
    
    /* 将enc值转移到它上面: */
    event_old->enc_diff += event_new->enc_diff;
    return true;
}

/*@brief 输入设备事件检查
 *@param event 输入设备事件
 */
static void scui_indev_enc_event_check(scui_event_t *event)
{
    #if SCUI_INDEV_EVENT_MERGE
    #else
    scui_event_notify(event);
    #endif
}

/*@brief 输入设备事件合并
 */
void scui_indev_enc_event_merge(void)
{
    #if SCUI_INDEV_EVENT_MERGE
    #endif
}

/*@brief 输入设备数据通报
 *@param data 数据
 */
void scui_indev_enc_notify(scui_indev_data_t *data)
{
    if (data->enc.enc_diff == 0)
        return;
    scui_event_sys_t type = scui_event_invalid;
    /* 编码器类型事件无需额外复杂处理 */
    if (data->enc.enc_diff > 0) {
        type = scui_event_enc_clockwise;
        SCUI_LOG_INFO("scui_event_enc_clockwise:%d", scui_abs(data->enc.enc_diff));
    }
    if (data->enc.enc_diff < 0) {
        type = scui_event_enc_clockwise_anti;
        SCUI_LOG_INFO("scui_event_enc_clockwise_anti:%d", scui_abs(data->enc.enc_diff));
    }
    /* 直接作为系统事件发送给管理器即可 */
    scui_event_t event = {
        .object = SCUI_HANDLE_SYSTEM,
        .type   = type,
        .absorb = scui_event_enc_absorb,
    };
    event.enc_diff = scui_abs(data->enc.enc_diff);
    scui_indev_enc_event_check(&event);
}

/*@brief 输入设备初始化
 */
void scui_indev_enc_ready(void)
{
}
