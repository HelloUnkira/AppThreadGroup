/*实现目标:
 *    输入设备事件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_indev_ptr_t scui_indev_ptr = {0};

/*@brief 指针事件吸收回调
 */
static bool scui_event_ptr_hold_absorb(void *evt_old, void *evt_new)
{
    scui_event_t *event_old = evt_old;
    scui_event_t *event_new = evt_new;
    
    /* 将ptr值转移到它上面: */
    event_old->ptr_tick = event_new->ptr_tick;
    return true;
}

/*@brief 编码器事件吸收回调
 */
static bool scui_event_ptr_move_absorb(void *evt_old, void *evt_new)
{
    scui_event_t *event_old = evt_old;
    scui_event_t *event_new = evt_new;
    
    /* 将key值转移到它上面: */
    event_old->ptr_e = event_new->ptr_e;
    return true;
}

/*@brief 输入设备数据通报
 *@param data 数据
 */
void scui_indev_ptr_notify(scui_indev_data_t *data)
{
    scui_point_t point = data->ptr.ptr_pos;
    scui_coord_t hor_res = SCUI_HOR_RES;
    scui_coord_t ver_res = SCUI_VER_RES;
    
    /* 检查范围是否跃出 */
    if (point.x < 0 || point.x >= hor_res)
        SCUI_LOG_WARN("pos x invalid<%d, %d>:%d", 0, hor_res - 1, point.x);
    if (point.y < 0 || point.y >= ver_res)
        SCUI_LOG_WARN("pos y invalid<%d, %d>:%d", 0, ver_res - 1, point.y);
    
    /* 限制坐标范围 */
    point.x = scui_clamp(point.x, 0, hor_res - 1);
    point.y = scui_clamp(point.y, 0, ver_res - 1);
    
    scui_event_t event = {
        .object = SCUI_HANDLE_SYSTEM,
    };
    /* 当前状态为release */
    if (data->state == scui_indev_state_release) {
        /* 上一状态为release */
        if (scui_indev_ptr.state == scui_indev_state_release)
            return;
        /* 上一状态为press */
        if (scui_indev_ptr.state == scui_indev_state_press) {
            scui_indev_ptr.state  = data->state;
            scui_indev_ptr.ptr_cnt++;
            scui_coord_t elapse = scui_tick_cnt() - scui_indev_ptr.cnt_tick;
            scui_coord_t last_x = scui_dist(scui_indev_ptr.ptr_last.x, point.x);
            scui_coord_t last_y = scui_dist(scui_indev_ptr.ptr_last.y, point.y);
            scui_coord_t last_r = scui_max(scui_abs(last_x), scui_abs(last_y));
            scui_multi_t last_v = elapse == 0 ? 0 : (SCUI_SCALE_COF * last_r / elapse);
            /* 扫描范围超出限制, 不看作是click */
            if (last_r > SCUI_INDEV_PTR_CLICK_RANGE)
                scui_indev_ptr.move_tag = true;
            /* 检查事件是否是点击 */
            if (elapse < SCUI_INDEV_PTR_CLICK && !scui_indev_ptr.move_tag) {
                event.type    = scui_event_ptr_click;
                event.ptr_cnt = scui_indev_ptr.ptr_cnt;
                event.ptr_c   = point;
                SCUI_LOG_INFO("scui_event_ptr_click:%d", event.ptr_cnt);
                scui_event_notify(&event);
            } else
            /* 检查事件是否是fling */
            if (last_v >= SCUI_INDEV_PTR_FLING_RATE &&
                scui_indev_ptr.move_cnt < SCUI_INDEV_PTR_MOVE_CNT) {
                event.type  = scui_event_ptr_fling;
                event.ptr_s = scui_indev_ptr.ptr_last;
                event.ptr_e = point;
                SCUI_LOG_INFO("scui_event_ptr_fling:(dist:%d, rate:%d)", last_r, last_v);
                scui_event_notify(&event);
            } else
            /* 事件是move */
            if (scui_indev_ptr.ptr_last.x != point.x ||
                scui_indev_ptr.ptr_last.y != point.y) {
                event.type   = scui_event_ptr_move;
                event.absorb = scui_event_ptr_move_absorb,
                event.ptr_s  = scui_indev_ptr.ptr_last;
                event.ptr_e  = point;
                SCUI_LOG_INFO("scui_event_ptr_move:(dist:%d, rate:%d)", last_r, last_v);
                scui_event_notify(&event);
            }
            scui_indev_ptr.cnt_tick = scui_tick_cnt();
            /* 发送抬起事件 */
            event.type    = scui_event_ptr_up;
            event.ptr_cnt = scui_indev_ptr.ptr_cnt;
            event.ptr_c   = point;
            SCUI_LOG_INFO("scui_event_ptr_up:%d", event.ptr_cnt);
            scui_event_notify(&event);
            return;
        }
    }
    /* 当前状态为press */
    if (data->state == scui_indev_state_press) {
        /* 上一状态为release */
        if (scui_indev_ptr.state == scui_indev_state_release) {
            scui_indev_ptr.state  = data->state;
            scui_indev_ptr.ptr_last = point;
            scui_indev_ptr.ptr_near = point;
            scui_indev_ptr.move_cnt = 0;
            scui_indev_ptr.move_tag = false;
            scui_coord_t elapse = scui_tick_cnt() - scui_indev_ptr.cnt_tick;
            scui_indev_ptr.cnt_tick = scui_tick_cnt();
            /* 检查点击是否连续 */
            if (elapse > SCUI_INDEV_PTR_CLICK_SPAN)
                scui_indev_ptr.ptr_cnt = 0;
            /* 发送按下事件 */
            event.type     = scui_event_ptr_down;
            event.ptr_c    = point;
            event.ptr_tick = elapse;
            SCUI_LOG_INFO("scui_event_ptr_down:%d", event.ptr_tick);
            scui_event_notify(&event);
            return;
        }
        /* 上一状态为press */
        if (scui_indev_ptr.state == scui_indev_state_press) {
            scui_indev_ptr.state  = data->state;
            scui_coord_t elapse = scui_tick_cnt() - scui_indev_ptr.cnt_tick;
            scui_coord_t last_x = scui_dist(scui_indev_ptr.ptr_last.x, point.x);
            scui_coord_t last_y = scui_dist(scui_indev_ptr.ptr_last.y, point.y);
            scui_coord_t near_x = scui_dist(scui_indev_ptr.ptr_near.x, point.x);
            scui_coord_t near_y = scui_dist(scui_indev_ptr.ptr_near.y, point.y);
            scui_coord_t last_r = scui_max(scui_abs(last_x), scui_abs(last_y));
            scui_coord_t near_r = scui_max(scui_abs(near_x), scui_abs(near_y));
            scui_multi_t last_v = elapse == 0 ? 0 : (SCUI_SCALE_COF * last_r / elapse);
            scui_multi_t near_v = elapse == 0 ? 0 : (SCUI_SCALE_COF * near_r / elapse);
            scui_indev_ptr.ptr_near = point;
            /* 移动速度不满足fling条件,转为move */
            if (last_v < SCUI_INDEV_PTR_FLING_RATE ||
                near_v < SCUI_INDEV_PTR_FLING_RATE) {
                
                if (scui_indev_ptr.move_cnt <= SCUI_INDEV_PTR_MOVE_CNT)
                    scui_indev_ptr.move_cnt++;
                
                if (scui_indev_ptr.move_cnt >= SCUI_INDEV_PTR_MOVE_CNT)
                if (last_r >= SCUI_INDEV_PTR_MOVE_DELTA) {
                    event.type   = scui_event_ptr_move;
                    event.absorb = scui_event_ptr_move_absorb,
                    event.ptr_s  = scui_indev_ptr.ptr_last;
                    event.ptr_e  = point;
                    SCUI_LOG_INFO("scui_event_ptr_move:(dist:%d, rate:%d)", last_r, last_v);
                    SCUI_LOG_INFO("scui_event_ptr_move:(dist:%d, rate:%d)", near_v, near_v);
                    scui_event_notify(&event);
                    scui_indev_ptr.ptr_last = point;
                    scui_indev_ptr.move_tag = true;
                }
            } else {
                // 清除移动计数器
                scui_indev_ptr.move_cnt = 0;
            }
            /* 发送按下事件 */
            event.type     = scui_event_ptr_hold;
            event.absorb   = scui_event_ptr_hold_absorb;
            event.ptr_c    = point;
            event.ptr_tick = elapse;
            SCUI_LOG_INFO("scui_event_ptr_hold:%d", event.ptr_tick);
            scui_event_notify(&event);
            return;
        }
        return;
    }
}

/*@brief 输入设备初始化
 */
void scui_indev_ptr_ready(void)
{
}

/*@brief 输入设备手势方向计算
 *@param event 输入设备事件
 *@retval 手势方向
 */
scui_opt_dir_t scui_indev_ptr_dir(scui_event_t *event)
{
    if (event->type > scui_event_ptr_s && event->type < scui_event_ptr_e) {
        scui_coord_t dist_x = scui_dist(event->ptr_s.x, event->ptr_e.x);
        scui_coord_t dist_y = scui_dist(event->ptr_s.y, event->ptr_e.y);
        
        if (dist_x >= dist_y) {
            if (event->ptr_s.x >= event->ptr_e.x)
                return scui_opt_dir_to_l;
            if (event->ptr_s.x <= event->ptr_e.x)
                return scui_opt_dir_to_r;
        }
        if (dist_x <= dist_y) {
            if (event->ptr_s.y >= event->ptr_e.y)
                return scui_opt_dir_to_u;
            if (event->ptr_s.y <= event->ptr_e.y)
                return scui_opt_dir_to_d;
        }
    }
    return scui_opt_dir_none;
}
