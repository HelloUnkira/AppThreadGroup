/*实现目标:
 *    输入设备事件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_indev_bar_t scui_indev_bar = {0};

/*@brief 滑条事件吸收回调
 */
static bool scui_event_bar_move_absorb(void *evt_old, void *evt_new)
{
    scui_event_t *event_old = evt_old;
    scui_event_t *event_new = evt_new;
    
    scui_coord_t diff_old = event_old->bar_diff;
    scui_coord_t diff_new = event_new->bar_diff;
    if (event_old->bar_way == 1) diff_old = -diff_old;
    if (event_new->bar_way == 1) diff_new = -diff_new;
    scui_coord_t diff_sum = diff_old + diff_new;
    
    /* 将bar位移转移到它上面(同向累加/反向对消): */
    event_old->bar_way  = diff_sum >= 0 ? 0 : 1;
    event_old->bar_diff = scui_abs(diff_sum);
    return true;
}

/*@brief 输入设备事件检查
 *@param event 输入设备事件
 */
static void scui_indev_bar_event_check(scui_event_t *event)
{
    #if SCUI_INDEV_BAR_EVENT_MERGE
    #endif
    scui_event_notify(event);
}

/*@brief 输入设备事件合并
 */
void scui_indev_bar_event_merge(void)
{
    #if SCUI_INDEV_BAR_EVENT_MERGE
    #endif
}

/*@brief 输入设备数据通报
 *@param data 数据
 */
void scui_indev_bar_notify(scui_indev_data_t *data)
{
    scui_coord_t point = data->bar.bar_pos;
    
    scui_event_t event = {
        .object = SCUI_HANDLE_SYSTEM,
    };
    /* 当前状态为release */
    if (data->state == scui_indev_state_release) {
        /* 上一状态为release */
        if (scui_indev_bar.state == scui_indev_state_release)
            return;
        /* 上一状态为press */
        if (scui_indev_bar.state == scui_indev_state_press) {
            scui_indev_bar.state  = data->state;
            uint64_t elapse = scui_tick_cnt() - scui_indev_bar.cnt_tick;
            scui_coord_t last_r = scui_abs(scui_dist(scui_indev_bar.bar_last, point));
            scui_multi_t last_v = elapse == 0 ? 0 : (SCUI_SCALE_COF * last_r / elapse);
            /* 检查事件是否是fling */
            if (last_v >= SCUI_INDEV_BAR_FLING_RATE &&
                scui_indev_bar.move_cnt < SCUI_INDEV_BAR_MOVE_CNT) {
                event.type     = scui_event_bar_fling;
                event.bar_way  = point >= scui_indev_bar.bar_last ? 0 : 1;
                event.bar_diff = last_r;
                if (event.bar_diff == 0) event.bar_diff = 1;
                SCUI_LOG_INFO("scui_event_bar_fling:(dist:%d, rate:%d)", last_r, last_v);
                scui_indev_bar_event_check(&event);
            } else if (last_r >= SCUI_INDEV_BAR_MOVE_DELTA) {
            /* 事件是move(位移直出) */
                event.type     = scui_event_bar_move;
                event.absorb   = scui_event_bar_move_absorb,
                event.bar_way  = point >= scui_indev_bar.bar_last ? 0 : 1;
                event.bar_diff = last_r;
                SCUI_LOG_INFO("scui_event_bar_move:(dist:%d, rate:%d)", last_r, last_v);
                scui_indev_bar_event_check(&event);
            }
            /* 发送抬起事件 */
            event.type = scui_event_bar_up;
            SCUI_LOG_INFO("scui_event_bar_up");
            scui_indev_bar_event_check(&event);
            scui_indev_bar.cnt_tick = scui_tick_cnt();
            return;
        }
    }
    /* 当前状态为press */
    if (data->state == scui_indev_state_press) {
        /* 上一状态为release */
        if (scui_indev_bar.state == scui_indev_state_release) {
            scui_indev_bar.state    = data->state;
            scui_indev_bar.bar_last = point;
            scui_indev_bar.bar_near = point;
            scui_indev_bar.move_cnt = 0;
            scui_indev_bar.move_tag = false;
            scui_indev_bar.cnt_tick = scui_tick_cnt();
            /* 发送按下事件 */
            event.type = scui_event_bar_down;
            SCUI_LOG_INFO("scui_event_bar_down");
            scui_indev_bar_event_check(&event);
            return;
        }
        /* 上一状态为press */
        if (scui_indev_bar.state == scui_indev_state_press) {
            scui_indev_bar.state    = data->state;
            uint64_t elapse = scui_tick_cnt() - scui_indev_bar.cnt_tick;
            scui_coord_t last_r = scui_abs(scui_dist(scui_indev_bar.bar_last, point));
            scui_coord_t near_r = scui_abs(scui_dist(scui_indev_bar.bar_near, point));
            scui_multi_t last_v = elapse == 0 ? 0 : (SCUI_SCALE_COF * last_r / elapse);
            scui_multi_t near_v = elapse == 0 ? 0 : (SCUI_SCALE_COF * near_r / elapse);
            scui_indev_bar.bar_near = point;
            /* 移动速度不满足fling条件,转为move */
            if (last_v < SCUI_INDEV_BAR_FLING_RATE ||
                near_v < SCUI_INDEV_BAR_FLING_RATE) {
                
                if (scui_indev_bar.move_cnt <= SCUI_INDEV_BAR_MOVE_CNT)
                    scui_indev_bar.move_cnt++;
                
                if (scui_indev_bar.move_cnt >= SCUI_INDEV_BAR_MOVE_CNT &&
                    last_r >= SCUI_INDEV_BAR_MOVE_DELTA) {
                    event.type     = scui_event_bar_move;
                    event.absorb   = scui_event_bar_move_absorb,
                    event.bar_way  = point >= scui_indev_bar.bar_last ? 0 : 1;
                    event.bar_diff = last_r;
                    SCUI_LOG_INFO("scui_event_bar_move:(dist:%d, rate:%d)", last_r, last_v);
                    scui_indev_bar_event_check(&event);
                    /* 位移直出: 推进基准点(残差累计消抖) */
                    scui_indev_bar.bar_last = point;
                    scui_indev_bar.move_tag  = true;
                }
            } else {
                /* 清除移动计数器 */
                scui_indev_bar.move_cnt = 0;
            }
            return;
        }
        return;
    }
}

/*@brief 输入设备初始化
 */
void scui_indev_bar_ready(void)
{
}
