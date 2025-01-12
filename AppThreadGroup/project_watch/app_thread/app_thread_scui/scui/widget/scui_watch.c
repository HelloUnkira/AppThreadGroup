/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 表盘指针控件创建
 *@param maker  表盘指针控件创建参数
 *@param handle 表盘指针控件句柄
 *@param layout 通过布局创建
 */
void scui_watch_make(scui_watch_maker_t *maker, scui_handle_t *handle, bool layout)
{
    /* 创建表盘指针控件实例 */
    scui_watch_t *watch = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_watch_t));
    memset(watch, 0, sizeof(scui_watch_t));
    
    /* 创建基础控件实例 */
    scui_widget_maker_t widget_maker = maker->widget;
    
    /* 必须标记anima事件 */
    widget_maker.style.sched_anima = true;
    
    /* 创建基础控件实例 */
    scui_widget_make(&watch->widget, &widget_maker, handle, layout);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_watch));
    SCUI_ASSERT(widget_maker.parent != SCUI_HANDLE_INVALID);
    
    watch->image_h   = maker->image_h;
    watch->image_m   = maker->image_m;
    watch->image_s   = maker->image_s;
    watch->anchor_h  = maker->anchor_h;
    watch->center_h  = maker->center_h;
    watch->anchor_m  = maker->anchor_m;
    watch->center_m  = maker->center_m;
    watch->anchor_s  = maker->anchor_s;
    watch->center_s  = maker->center_s;
    watch->tick_mode = maker->tick_mode;
    
    /* 创建时刷新一次 */
    scui_event_t event = {.object = *handle, .type = scui_event_anima_elapse,};
    scui_widget_event_mask_execute(&event);
    scui_watch_event(&event);
}

/*@brief 表盘指针控件销毁
 *@param handle 表盘指针控件句柄
 *@param parent_way 来自父控件的销毁
 */
void scui_watch_burn(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_watch_t  *watch  = (void *)widget;
    
    /* 销毁基础控件实例 */
    scui_widget_burn(&watch->widget);
    
    /* 销毁表盘指针控件实例 */
    SCUI_MEM_FREE(watch);
}

/*@brief 表盘指针跳动模式
 *@param handle 滚动控件句柄
 *@param tick_mode 一度一跳:1;一秒一跳:0;
 */
void scui_watch_tick_mode(scui_handle_t handle, bool tick_mode)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_watch));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_watch_t  *watch  = (void *)widget;
    
    watch->tick_mode = tick_mode;
    scui_widget_draw(handle, NULL, false);
}

/*@brief 表盘指针控件事件处理回调
 *@param event 事件
 */
void scui_watch_event(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_watch_t  *watch  = (void *)widget;
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             return;
        
        scui_indev_data_set_t *data_set = NULL;
        scui_indev_data_set(&data_set);
        
        if (watch->tick_h != data_set->sys_time_h ||
            watch->tick_m != data_set->sys_time_m ||
            watch->tick_s != data_set->sys_time_s) {
            watch->tick_h  = data_set->sys_time_h;
            watch->tick_m  = data_set->sys_time_m;
            watch->tick_s  = data_set->sys_time_s;
            watch->tick_ms     = 0;
            watch->tick_ms_rcd = 0;
            scui_widget_draw(widget->myself, NULL, false);
            break;
        }
        
        if (watch->tick_mode) {
            
            watch->tick_ms += SCUI_ANIMA_TICK;
            if (watch->tick_ms >= 1000)
                watch->tick_ms  = 1000;
            
            /* 一度一跳时 */
            if (scui_dist(watch->tick_ms, watch->tick_ms_rcd) >= (1000 / 6)) {
                watch->tick_ms_rcd = watch->tick_ms;
                scui_widget_draw(widget->myself, NULL, false);
                break;
            }
        } else {
            watch->tick_ms     = 0;
            watch->tick_ms_rcd = 0;
        }
        break;
    }
    case scui_event_draw: {
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             return;
        
        /* hour: */
        scui_coord_t angle_h = watch->tick_h * (360 / 24) + watch->tick_m * (360 / 60) * 6 / 360;
        scui_widget_draw_image_rotate(widget->myself, NULL, watch->image_h, NULL, &watch->anchor_h, &watch->center_h, angle_h);
        /* minute: */
        scui_coord_t angle_m = watch->tick_m * (360 / 60) + watch->tick_s * (360 / 60) * 6 / 360;
        scui_widget_draw_image_rotate(widget->myself, NULL, watch->image_m, NULL, &watch->anchor_m, &watch->center_m, angle_m);
        /* second: */
        scui_coord_t angle_s = watch->tick_s * (360 / 60) + watch->tick_ms * 6 / 1000;
        scui_widget_draw_image_rotate(widget->myself, NULL, watch->image_s, NULL, &watch->anchor_s, &watch->center_s, angle_s);
        
        break;
    }
    default:
        break;
    }
}
