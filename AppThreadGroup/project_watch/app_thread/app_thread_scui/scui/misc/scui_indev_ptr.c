/*实现目标:
 *    输入设备事件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

static scui_indev_ptr_t scui_indev_ptr = {0};

/*@brief 输入设备数据通报
 *@param data 数据
 */
void scui_indev_ptr_notify(scui_indev_data_t *data)
{
    scui_point_t point = data->ptr.ptr_pos;
    
    /* 检查范围是否跃出 */
    if (point.x < 0 || point.x >= scui_disp_get_hor_res())
        APP_SYS_LOG_WARN("pos x invalid<%d, %d>:%d", 0, scui_disp_get_hor_res() - 1, point.x);
    if (point.y < 0 || point.y >= scui_disp_get_ver_res())
        APP_SYS_LOG_WARN("pos y invalid<%d, %d>:%d", 0, scui_disp_get_ver_res() - 1, point.y);
    
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
            scui_coord_t dist_x = app_sys_dist(scui_indev_ptr.ptr_last.x, point.x);
            scui_coord_t dist_y = app_sys_dist(scui_indev_ptr.ptr_last.y, point.y);
            scui_coord_t dist_r = app_sys_max(app_sys_abs(dist_x), app_sys_abs(dist_y));
            /* 检查事件是否是点击 */
            if (elapse < SCUI_INDEV_PTR_CLICK) {
                event.type    = scui_event_ptr_click;
                event.ptr_cnt = scui_indev_ptr.ptr_cnt;
                APP_SYS_LOG_INFO("scui_event_ptr_click:%d", event.ptr_cnt);
                scui_event_notify(&event);
            } else
            /* 检查事件是否是fling */
            if (scui_indev_ptr.move_cnt < SCUI_INDEV_PTR_FLING_CNT &&
               (dist_r != 0 && elapse / dist_r < SCUI_INDEV_PTR_FLING_SPAN)) {
                event.type  = scui_event_ptr_fling;
                event.ptr_s = scui_indev_ptr.ptr_last;
                event.ptr_e = point;
                APP_SYS_LOG_INFO("scui_event_ptr_fling:%d", dist_r);
                scui_event_notify(&event);
            } else
            /* 事件是move */
            if (dist_r != 0) {
                event.type  = scui_event_ptr_move;
                event.ptr_s = scui_indev_ptr.ptr_last;
                event.ptr_e = point;
                APP_SYS_LOG_INFO("scui_event_ptr_move:%d", dist_r);
                scui_event_notify(&event);
            }
            scui_indev_ptr.cnt_tick = scui_tick_cnt();
            /* 发送抬起事件 */
            event.type    = scui_event_ptr_up;
            event.ptr_cnt = scui_indev_ptr.ptr_cnt;
            event.ptr_c   = point;
            APP_SYS_LOG_INFO("scui_event_ptr_up:%d", event.ptr_cnt);
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
            scui_indev_ptr.move_cnt = 0;
            scui_coord_t elapse = scui_tick_cnt() - scui_indev_ptr.cnt_tick;
            scui_indev_ptr.cnt_tick  = scui_tick_cnt();
            /* 检查点击是否连续 */
            if (elapse > SCUI_INDEV_PTR_CLICK_SPAN)
                scui_indev_ptr.ptr_cnt = 0;
            /* 发送按下事件 */
            event.type     = scui_event_ptr_down;
            event.ptr_tick = elapse;
            APP_SYS_LOG_INFO("scui_event_ptr_down:%d", event.ptr_tick);
            return;
        }
        /* 上一状态为press */
        if (scui_indev_ptr.state == scui_indev_state_press) {
            scui_indev_ptr.state  = data->state;
            scui_coord_t elapse = scui_tick_cnt() - scui_indev_ptr.cnt_tick;
            scui_coord_t dist_x = app_sys_dist(scui_indev_ptr.ptr_last.x, point.x);
            scui_coord_t dist_y = app_sys_dist(scui_indev_ptr.ptr_last.y, point.y);
            scui_coord_t dist_r = app_sys_max(app_sys_abs(dist_x), app_sys_abs(dist_y));
            /* 移动总速度不满足fling条件,转为move */
            if (dist_r != 0 && elapse / dist_r >= SCUI_INDEV_PTR_FLING_SPAN)
            if (scui_indev_ptr.move_cnt <= SCUI_INDEV_PTR_FLING_CNT)
                scui_indev_ptr.move_cnt++;
            if (scui_indev_ptr.move_cnt >= SCUI_INDEV_PTR_FLING_CNT) {
                event.type  = scui_event_ptr_move;
                event.ptr_s = scui_indev_ptr.ptr_last;
                event.ptr_e = point;
                APP_SYS_LOG_INFO("scui_event_ptr_move:%d", dist_r);
                scui_event_notify(&event);
                scui_indev_ptr.ptr_last = point;
            }
            /* 发送按下事件 */
            event.type     = scui_event_ptr_hold,
            event.ptr_tick = elapse,
            APP_SYS_LOG_INFO("scui_event_ptr_hold:%d", event.ptr_tick);
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
scui_indev_ptr_way_t scui_indev_ptr_way_calc(scui_event_t *event)
{
    if (event->type > scui_event_ptr_s && event->type < scui_event_ptr_e) {
        scui_coord_t dist_x = app_sys_dist(event->ptr_s.x, event->ptr_e.x);
        scui_coord_t dist_y = app_sys_dist(event->ptr_s.y, event->ptr_e.y);
        
        if (dist_x >= dist_y) {
            if (event->ptr_s.x >= event->ptr_e.x)
                return scui_indev_ptr_way_left;
            if (event->ptr_s.x <= event->ptr_e.x)
                return scui_indev_ptr_way_right;
        }
        if (dist_x <= dist_y) {
            if (event->ptr_s.y >= event->ptr_e.y)
                return scui_indev_ptr_way_up;
            if (event->ptr_s.y <= event->ptr_e.y)
                return scui_indev_ptr_way_down;
        }
    }
    return scui_indev_ptr_way_none;
}