/*实现目标:
 *    输入设备事件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_indev_enc_t scui_indev_enc = {0};

/*@brief 旋钮事件吸收回调
 */
static bool scui_event_enc_absorb(void *evt_old, void *evt_new)
{
    scui_event_t *event_old = evt_old;
    scui_event_t *event_new = evt_new;
    
    scui_coord_t diff_old = event_old->enc_diff;
    scui_coord_t diff_new = event_new->enc_diff;
    if (event_old->enc_way == 1) diff_old = -diff_old;
    if (event_new->enc_way == 1) diff_new = -diff_new;
    scui_coord_t diff_sum = diff_old + diff_new;
    
    /* 将enc差分转移到它上面(同向累加/反向对消): */
    event_old->enc_way  = diff_sum >= 0 ? 0 : 1;
    event_old->enc_diff = scui_abs(diff_sum);
    return true;
}

/*@brief 输入设备事件检查
 *@param event 输入设备事件
 */
static void scui_indev_enc_event_check(scui_event_t *event)
{
    #if SCUI_INDEV_ENC_EVENT_MERGE
    scui_event_t event_diff = *event;
    event_diff.enc_way  = scui_indev_enc.event_diff.enc_way;
    event_diff.enc_diff = scui_indev_enc.event_diff.enc_diff;
    
    scui_event_t *event_old = &event_diff;
    scui_event_t *event_new = event;
    
    scui_coord_t diff_old = event_old->enc_diff;
    scui_coord_t diff_new = event_new->enc_diff;
    if (event_old->enc_way == 1) diff_old = -diff_old;
    if (event_new->enc_way == 1) diff_new = -diff_new;
    scui_coord_t diff_sum = diff_old + diff_new;
    
    /* 将enc差分转移到它上面(同向累加/反向对消): */
    event_old->enc_way  = diff_sum >= 0 ? 0 : 1;
    event_old->enc_diff = scui_abs(diff_sum);
    
    scui_indev_enc.event_diff = event_diff;
    scui_indev_enc.event_diff_tick = scui_tick_cnt();
    #else
    scui_event_notify(event);
    #endif
}

/*@brief 输入设备事件合并
 */
void scui_indev_enc_event_merge(void)
{
    #if SCUI_INDEV_ENC_EVENT_MERGE
    uint64_t diff_tick = scui_indev_enc.event_diff_tick;
    if (diff_tick != 0 && scui_tick_cnt() - diff_tick > SCUI_INDEV_ENC_MERGE_SPAN) {
        scui_event_notify(&scui_indev_enc.event_diff);
        scui_indev_enc.event_diff.enc_diff = 0;
        scui_indev_enc.event_diff_tick = 0;
    }
    #endif
}

/*@brief 输入设备数据通报
 *@param data 数据
 */
void scui_indev_enc_notify(scui_indev_data_t *data)
{
    if (data->enc.enc_diff == 0)
        return;
    
    scui_event_t event = {
        .object   = SCUI_HANDLE_SYSTEM,
        .type     = scui_event_enc_tick,
        .absorb   = scui_event_enc_absorb,
        .enc_way  = data->enc.enc_diff > 0 ? 0 : 1,
        .enc_diff = scui_abs(data->enc.enc_diff),
    };
    SCUI_LOG_INFO("scui_event_enc_tick:%d", event.enc_diff);
    scui_indev_enc_event_check(&event);
}

/*@brief 输入设备初始化
 */
void scui_indev_enc_ready(void)
{
}