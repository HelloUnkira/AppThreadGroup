/*实现目标:
 *    窗口管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 拆分部件,这部分直接引用 */
extern scui_window_mgr_t scui_window_mgr;

/*@brief 窗口切换风格自动更新
 *@param switch_type 窗口切换风格
 *@param switch_dir  窗口切换方向
 */
void scui_window_jump_auto_update(scui_window_switch_type_t type, scui_event_dir_t dir)
{
    /* 自适应需要更新窗口切换状态 */
    if (type == scui_window_switch_auto) {
        switch (scui_window_mgr.switch_args.type_cfg) {
        case scui_window_switch_none:
        case scui_window_switch_center_in:
        case scui_window_switch_center_out:
            scui_window_mgr.switch_args.type_cur = scui_window_mgr.switch_args.type_cfg;
            break;
        case scui_window_switch_normal:
        case scui_window_switch_zoom1:
        case scui_window_switch_zoom2:
            /* 带手势方向的窗口切换需要判断方向 */
            /* 如果没有方向的话就默认无窗口切换 */
            if (scui_window_mgr.switch_args.dir_cur == scui_event_dir_to_l ||
                scui_window_mgr.switch_args.dir_cur == scui_event_dir_to_r ||
                scui_window_mgr.switch_args.dir_cur == scui_event_dir_to_u ||
                scui_window_mgr.switch_args.dir_cur == scui_event_dir_to_d) {
                scui_window_mgr.switch_args.type_cur = scui_window_mgr.switch_args.type_cfg;
                break;
            }
            if (scui_window_mgr.switch_args.dir_cfg == scui_event_dir_to_l ||
                scui_window_mgr.switch_args.dir_cfg == scui_event_dir_to_r ||
                scui_window_mgr.switch_args.dir_cfg == scui_event_dir_to_u ||
                scui_window_mgr.switch_args.dir_cfg == scui_event_dir_to_d) {
                scui_window_mgr.switch_args.dir_cur  = scui_window_mgr.switch_args.dir_cfg;
                scui_window_mgr.switch_args.type_cur = scui_window_mgr.switch_args.type_cfg;
                break;
            }
            scui_window_mgr.switch_args.dir_cur  = scui_event_dir_none;
            scui_window_mgr.switch_args.type_cur = scui_window_switch_none;
            break;
        default:
            scui_window_mgr.switch_args.dir_cur  = scui_event_dir_none;
            scui_window_mgr.switch_args.type_cur = scui_window_switch_none;
            SCUI_LOG_WARN("unknown switch type cfg, use switch none");
            break;
        }
    }
}

/*@brief 窗口跳转动画回调
 */
void scui_window_jump_anima_start(void *instance)
{
    SCUI_LOG_INFO("");
    scui_window_mgr.switch_args.pct = 0;
    scui_window_mgr.switch_args.ofs = 0;
    
    scui_point_t point = (scui_point_t){0};
    switch (scui_window_mgr.switch_args.type_cur) {
    case scui_window_switch_center_in:
    case scui_window_switch_center_out: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[0]);
        widget->surface.alpha = scui_alpha_pct100;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[1]);
        widget->surface.alpha = scui_alpha_pct0;
        scui_widget_move(scui_window_mgr.switch_args.list[0], &point);
        scui_widget_move(scui_window_mgr.switch_args.list[1], &point);
        break;
    }
    case scui_window_switch_zoom1:
    case scui_window_switch_zoom2: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[0]);
        widget->surface.alpha = scui_alpha_pct100;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[1]);
        widget->surface.alpha = scui_alpha_pct0;
    }
    case scui_window_switch_normal: {
        scui_widget_move(scui_window_mgr.switch_args.list[0], &point);
        switch (scui_window_mgr.switch_args.dir_cur) {
        case scui_event_dir_to_u:
            point.y = +scui_widget_attr_clip(scui_window_mgr.switch_args.list[0]).h;
            break;
        case scui_event_dir_to_d:
            point.y = -scui_widget_attr_clip(scui_window_mgr.switch_args.list[1]).h;
            break;
        case scui_event_dir_to_l:
            point.x = +scui_widget_attr_clip(scui_window_mgr.switch_args.list[0]).w;
            break;
        case scui_event_dir_to_r:
            point.x = -scui_widget_attr_clip(scui_window_mgr.switch_args.list[1]).w;
            break;
        default:
            SCUI_LOG_ERROR("error switch dir 0x%08x", scui_window_mgr.switch_args.dir_cur);
            break;
        }
        scui_widget_move(scui_window_mgr.switch_args.list[1], &point);
        break;
    }
    default:
        SCUI_LOG_ERROR("error switch tyoe 0x%08x", scui_window_mgr.switch_args.type_cur);
        break;
    }
    
    /* 窗口发生改变,刷新 */
    scui_widget_refr(scui_window_mgr.active_curr, true);
}

/*@brief 窗口跳转动画回调
 */
void scui_window_jump_anima_ready(void *instance)
{
    SCUI_LOG_INFO("");
    
    switch (scui_window_mgr.switch_args.type_cur) {
    case scui_window_switch_center_in:
    case scui_window_switch_center_out:
    case scui_window_switch_zoom1:
    case scui_window_switch_zoom2: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[0]);
        widget->surface.alpha = scui_alpha_pct0;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[1]);
        widget->surface.alpha = scui_alpha_pct100;
    }
    case scui_window_switch_normal: {
        scui_window_mgr.switch_args.pct = 100;
        scui_point_t point = (scui_point_t){0};
        scui_window_active(scui_window_mgr.switch_args.list[1]);
        scui_widget_move(scui_window_mgr.switch_args.list[1], &point);
        scui_widget_hide(scui_window_mgr.switch_args.list[0]);
        scui_widget_refr(scui_window_mgr.switch_args.list[1], true);
        break;
    }
    default:
        SCUI_LOG_ERROR("error switch tyoe 0x%08x", scui_window_mgr.switch_args.type_cur);
        break;
    }
    
    SCUI_ASSERT(scui_window_mgr.switch_args.anima != SCUI_HANDLE_INVALID);
    scui_anima_destroy(scui_window_mgr.switch_args.anima);
    scui_window_mgr.switch_args.lock = false;
}

/*@brief 窗口跳转动画回调
 */
void scui_window_jump_anima_expired(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t *anima = instance;
    
    scui_multi_t ofs = 0;
    if ((scui_window_mgr.switch_args.dir_cur & scui_event_dir_ver) != 0)
         ofs = scui_disp_get_ver_res();
    if ((scui_window_mgr.switch_args.dir_cur & scui_event_dir_hor) != 0)
         ofs = scui_disp_get_hor_res();
    
    ofs *= anima->value_c;
    ofs /= 100;
    
    scui_window_mgr.switch_args.pct = anima->value_c;
    scui_window_mgr.switch_args.ofs = ofs;
    
    scui_point_t point = (scui_point_t){0};
    switch (scui_window_mgr.switch_args.type_cur) {
    case scui_window_switch_center_in:
    case scui_window_switch_center_out: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[0]);
        widget->surface.alpha = scui_alpha_by_percent(100 - scui_window_mgr.switch_args.pct);
        widget = scui_handle_get(scui_window_mgr.switch_args.list[1]);
        widget->surface.alpha = scui_alpha_by_percent(scui_window_mgr.switch_args.pct);
        scui_widget_move(scui_window_mgr.switch_args.list[0], &point);
        scui_widget_move(scui_window_mgr.switch_args.list[1], &point);
        break;
    }
    case scui_window_switch_zoom1:
    case scui_window_switch_zoom2: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[0]);
        widget->surface.alpha = scui_alpha_by_percent(100 - scui_window_mgr.switch_args.pct);
        widget = scui_handle_get(scui_window_mgr.switch_args.list[1]);
        widget->surface.alpha = scui_alpha_by_percent(scui_window_mgr.switch_args.pct);
    }
    case scui_window_switch_normal: {
        scui_point_t point = (scui_point_t){0};
        switch (scui_window_mgr.switch_args.dir_cur) {
        case scui_event_dir_to_u:
            point.y = -scui_window_mgr.switch_args.ofs;
            scui_widget_move(scui_window_mgr.switch_args.list[0], &point);
            point.y += +scui_widget_attr_clip(scui_window_mgr.switch_args.list[0]).h;
            scui_widget_move(scui_window_mgr.switch_args.list[1], &point);
            break;
        case scui_event_dir_to_d:
            point.y = +scui_window_mgr.switch_args.ofs;
            scui_widget_move(scui_window_mgr.switch_args.list[0], &point);
            point.y += -scui_widget_attr_clip(scui_window_mgr.switch_args.list[1]).h;
            scui_widget_move(scui_window_mgr.switch_args.list[1], &point);
            break;
        case scui_event_dir_to_l:
            point.x = -scui_window_mgr.switch_args.ofs;
            scui_widget_move(scui_window_mgr.switch_args.list[0], &point);
            point.x += +scui_widget_attr_clip(scui_window_mgr.switch_args.list[0]).w;
            scui_widget_move(scui_window_mgr.switch_args.list[1], &point);
            break;
        case scui_event_dir_to_r:
            point.x = +scui_window_mgr.switch_args.ofs;
            scui_widget_move(scui_window_mgr.switch_args.list[0], &point);
            point.x += -scui_widget_attr_clip(scui_window_mgr.switch_args.list[1]).w;
            scui_widget_move(scui_window_mgr.switch_args.list[1], &point);
            break;
        default:
            SCUI_LOG_ERROR("error switch dir 0x%08x", scui_window_mgr.switch_args.dir_cur);
            break;
        }
        break;
    }
    default:
        SCUI_LOG_ERROR("error switch type 0x%08x", scui_window_mgr.switch_args.type_cur);
        break;
    }
    
    /* 窗口发生改变,刷新 */
    scui_widget_refr(scui_window_mgr.active_curr, true);
}
