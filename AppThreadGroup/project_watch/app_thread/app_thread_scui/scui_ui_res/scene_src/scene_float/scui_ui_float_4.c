/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static void scui_scene_float_4_show(scui_event_t *event)
{
    if (!scui_widget_event_check_prepare(event))
         return;
    
    if (event->type == scui_event_show) {
        
        scui_coord_t vlist_min[20] = {0};
        scui_coord_t vlist_max[20] = {0};
        for (uint32_t idx = 0; idx < 20; idx++) {
            vlist_min[idx] =  60 + (uint32_t)scui_rand(0, 0xFF) % 40;   //统一底点就是柱状图,否则为股价图
            vlist_max[idx] = 220 - (uint32_t)scui_rand(0, 0xFF) % 40;
        }
        
        scui_chart_maker_t chart_maker = {0};
        scui_handle_t chart_handle = SCUI_HANDLE_INVALID;
        chart_maker.widget.type = scui_widget_type_chart;
        chart_maker.widget.style.indev_ptr = true;
        chart_maker.widget.style.indev_enc = true;
        chart_maker.widget.style.indev_key = true;
        chart_maker.widget.clip.w = SCUI_DRV_HOR_RES * 11 / 24;
        chart_maker.widget.clip.h = SCUI_DRV_VER_RES * 11 / 24;
        chart_maker.widget.parent = SCUI_UI_SCENE_FLOAT_4;
        chart_maker.widget.color.color.full = 0xFF4F4F4F;
        chart_maker.type = scui_chart_type_histogram;
        chart_maker.histogram.edge = scui_image_prj_image_src_repeat_05_dotbmp;
        chart_maker.histogram.value_min = 60;
        chart_maker.histogram.value_max = 220;
        chart_maker.histogram.number    = 20;
        chart_maker.histogram.height    = chart_maker.widget.clip.h - 10;
        chart_maker.histogram.space     = 4;
        chart_maker.histogram.color.color.full = 0xFFFF0000;
        scui_chart_create(&chart_maker, &chart_handle, false);
        scui_chart_histogram_data(chart_handle, vlist_min, vlist_max);
    }
}

static void scui_scene_float_4_hide(scui_event_t *event)
{
    if (!scui_widget_event_check_finish(event))
         return;
    
    if (event->type == scui_event_hide) {
        // 控件会自动销毁,无需额外销毁
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_4_event_proc(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        scui_scene_float_4_show(event);
        scui_window_float_event_grasp_show(event);
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        scui_scene_float_4_hide(event);
        scui_window_float_event_grasp_hide(event);
        break;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        scui_widget_event_mask_keep(event);
        break;
    default:
        if (event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e)
            scui_window_float_event_grasp_ptr(event);
        if (event->type >= scui_event_key_s && event->type <= scui_event_key_e)
            scui_window_float_event_grasp_key(event);
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
