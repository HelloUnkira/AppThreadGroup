/*实现目标:
 *    窗口管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

scui_window_mgr_t scui_window_mgr = {0};

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
    
    if ((scui_window_mgr.switch_args.dir_cur | scui_event_dir_ver) != 0)
         scui_window_mgr.switch_args.ofs = scui_disp_get_ver_res();
    if ((scui_window_mgr.switch_args.dir_cur | scui_event_dir_hor) != 0)
         scui_window_mgr.switch_args.ofs = scui_disp_get_hor_res();
    
    scui_window_mgr.switch_args.pct  = anima->value_c;
    scui_window_mgr.switch_args.ofs *= anima->value_c;
    scui_window_mgr.switch_args.ofs /= 100;
    
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
    }
    default:
        SCUI_LOG_ERROR("error switch tyoe 0x%08x", scui_window_mgr.switch_args.type_cur);
        break;
    }
}

/*@brief 窗口管理器混合根控件列表
 *       将所有根控件画布混合到绘制画布上
 *       窗口管理器会有特殊的处理
 *       用于处理画布级别特效流程
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_mix_list(scui_widget_t **list, scui_handle_t num)
{
    switch (scui_window_mgr.switch_args.type_cfg) {
        case scui_window_switch_none:
        case scui_window_switch_center_in:
        case scui_window_switch_center_out:
        case scui_window_switch_normal: {
                scui_surface_t *dst_surface = scui_surface_fb_draw();
                scui_area_t dst_clip = {
                    .w = scui_disp_get_hor_res(),
                    .h = scui_disp_get_ver_res(),
                };
            for (scui_handle_t idx = 0; idx < num; idx++) {
                scui_surface_t *src_surface = &list[idx]->surface;
                scui_area_t src_clip = src_surface->clip;
                scui_draw_area_blend(dst_surface, &dst_clip, src_surface, &src_clip);
            }
            break;
        }
        case scui_window_switch_zoom1:
        case scui_window_switch_zoom2:
            break;
    }
}

/*@brief 窗口管理器排序根控件列表
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_window_sort_list(scui_widget_t **list, scui_handle_t num)
{
    /* 要用稳定排序,这里使用冒泡排序 */
    for (scui_handle_t idx_i = 0; idx_i < num; idx_i++)
    for (scui_handle_t idx_j = 0; idx_j + 1 < num - idx_i; idx_j++) {
        scui_widget_t *widget_1 = list[idx_j];
        scui_widget_t *widget_2 = list[idx_j + 1];
        SCUI_ASSERT(widget_1->parent == SCUI_HANDLE_INVALID);
        SCUI_ASSERT(widget_2->parent == SCUI_HANDLE_INVALID);
        SCUI_ASSERT(scui_handle_remap(widget_1->myself));
        SCUI_ASSERT(scui_handle_remap(widget_2->myself));
        
        SCUI_ASSERT(widget_1->type == scui_widget_type_window);
        SCUI_ASSERT(widget_2->type == scui_widget_type_window);
        scui_window_t *window_1 = (scui_window_t *)widget_1;
        scui_window_t *window_2 = (scui_window_t *)widget_2;
        
        if (window_1->level > window_2->level) {
            scui_widget_t *widget_t = list[idx_j];
            list[idx_j] = list[idx_j + 1];
            list[idx_j + 1] = widget_t;
        }
    }
}

/*@brief 窗口管理器混合画布
 *       将所有独立画布混合到绘制画布上
 *       将所有无独立画布就地渲染
 */
void scui_window_mix_surface(void)
{
    scui_handle_t list_num = 0;
    scui_widget_t *list[SCUI_SCENE_MGR_LIMIT] = {0};
    
    /* 第一轮混合:处理所有独立画布 */
    for (scui_handle_t idx = 0; idx < SCUI_SCENE_MGR_LIMIT; idx++) {
        if (scui_window_mgr.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        scui_handle_t  handle = scui_window_mgr.list[idx];
        scui_widget_t *widget = scui_handle_get(handle);
        SCUI_ASSERT(scui_handle_remap(handle));
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        
        if (scui_widget_surface_only(widget))
            list[list_num++] = widget;
    }
    scui_window_sort_list(list, list_num);
    scui_window_mix_list(list, list_num);
    list_num = 0;
    
    /* 第二轮混合:处理所有独立画布 */
    for (scui_handle_t idx = 0; idx < SCUI_SCENE_MGR_LIMIT; idx++) {
        if (scui_window_mgr.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        scui_handle_t  handle = scui_window_mgr.list[idx];
        scui_widget_t *widget = scui_handle_get(handle);
        SCUI_ASSERT(scui_handle_remap(handle));
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        
        if (!scui_widget_surface_only(widget))
             list[list_num++] = widget;
    }
    scui_window_sort_list(list, list_num);
    
    for (scui_handle_t idx = 0; idx < list_num; idx++) {
        scui_widget_t *widget = list[idx];
        scui_handle_t  handle = widget->myself;
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        SCUI_ASSERT(scui_handle_remap(handle));
        
        scui_widget_draw(handle, true);
    }
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_window_event_dispatch(scui_event_t *event)
{
    return scui_event_retval_quit;
}
