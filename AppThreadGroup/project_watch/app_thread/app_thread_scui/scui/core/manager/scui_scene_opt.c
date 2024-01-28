/*实现目标:
 *    场景管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 拆分部件,这部分直接引用 */
extern scui_scene_mgr_t scui_scene_mgr;

/*@brief 场景列表添加场景
 *@param handle 场景句柄
 */
void scui_scene_list_add(scui_handle_t handle)
{
    for (scui_handle_t idx = 0; idx < SCUI_SCENE_MGR_LIMIT; idx++)
        if (scui_scene_mgr.list[idx] == handle) {
            SCUI_LOG_INFO("redundant operation");
            return;
        }
    for (scui_handle_t idx = 0; idx < SCUI_SCENE_MGR_LIMIT; idx++)
        if (scui_scene_mgr.list[idx] == SCUI_HANDLE_INVALID) {
            scui_scene_mgr.list[idx]  = handle;
            scui_scene_mgr.list_num++;
            return;
        }
    SCUI_LOG_ERROR("scene %u add fail", handle);
}

/*@brief 场景列表移除场景
 *@param handle 场景句柄
 */
void scui_scene_list_del(scui_handle_t handle)
{
    /* 如果移除的是焦点,记得清空 */
    if (scui_scene_mgr.active_curr == handle) {
        scui_scene_mgr.active_curr  = SCUI_HANDLE_INVALID;
        scui_scene_mgr.active_last  = SCUI_HANDLE_INVALID;
    }
    
    for (scui_handle_t idx = 0; idx < SCUI_SCENE_MGR_LIMIT; idx++)
        if (scui_scene_mgr.list[idx] == handle) {
            scui_scene_mgr.list[idx]  = SCUI_HANDLE_INVALID;
            scui_scene_mgr.list_num--;
            return;
        }
    SCUI_LOG_ERROR("scene %u del fail", handle);
}

/*@brief 场景隐藏
 *@param handle 场景句柄
 */
void scui_scene_hide_without(scui_handle_t handle)
{
    for (scui_handle_t idx = 0; idx < SCUI_SCENE_MGR_LIMIT; idx++)
        if (scui_scene_mgr.list[idx] != handle)
            scui_widget_hide(scui_scene_mgr.list[idx]);
}

/*@brief 获得活跃场景句柄
 *@retval 场景句柄
 */
scui_handle_t scui_scene_active_last(void)
{
    SCUI_ASSERT(scui_handle_remap(scui_scene_mgr.active_last));
    return scui_scene_mgr.active_last;
}

/*@brief 获得活跃场景句柄
 *@retval 场景句柄
 */
scui_handle_t scui_scene_active_curr(void)
{
    SCUI_ASSERT(scui_handle_remap(scui_scene_mgr.active_curr));
    return scui_scene_mgr.active_curr;
}

/*@brief 场景激活
 *@param handle 场景句柄
 */
void scui_scene_active(scui_handle_t handle)
{
    SCUI_ASSERT(scui_handle_remap(handle));
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    
    if (scui_scene_mgr.active_curr == handle) {
        SCUI_LOG_WARN("redundant operation");
        return;
    }
    
    /* 焦点必须在场景列表中 */
    bool not_match_yet = true;
    for (scui_handle_t idx = 0; idx < SCUI_SCENE_MGR_LIMIT; idx++)
        if (scui_scene_mgr.list[idx] == handle) {
            not_match_yet = false;
            break;
        }
    if (not_match_yet) {
        SCUI_LOG_WARN("unknown scene %u", handle);
        return;
    }
    
    scui_event_t event = {.style = scui_event_style_sync,};
    scui_scene_mgr.active_last = scui_scene_mgr.active_curr;
    scui_scene_mgr.active_curr = handle;
    
    /* 先失活旧的焦点窗口 */
    if (scui_handle_remap(scui_scene_mgr.active_last)) {
        event.type   = scui_event_focus_lost;
        event.object = scui_scene_mgr.active_last;
        scui_event_notify(&event);
    }
    
    /* 后激活新的焦点窗口 */
    event.type   = scui_event_focus_get;
    event.object = scui_scene_mgr.active_curr;
    scui_event_notify(&event);
}

/*@brief 场景切换风格自动更新
 *@param switch_type 场景切换风格
 *@param switch_dir  场景切换方向
 */
static void scui_scene_switch_auto_update(scui_scene_switch_type_t type, scui_event_dir_t dir)
{
    /* 自适应需要更新场景切换状态 */
    if (type == scui_scene_switch_auto) {
        switch (scui_scene_mgr.switch_type_cfg) {
        case scui_scene_switch_none:
        case scui_scene_switch_center_in:
        case scui_scene_switch_center_out:
            scui_scene_mgr.switch_type_cur = scui_scene_mgr.switch_type_cfg;
            break;
        case scui_scene_switch_normal:
        case scui_scene_switch_zoom1:
        case scui_scene_switch_zoom2:
            /* 带手势方向的场景切换需要判断方向 */
            /* 如果没有方向的话就默认无场景切换 */
            if (scui_scene_mgr.switch_dir_cur == scui_event_dir_to_l ||
                scui_scene_mgr.switch_dir_cur == scui_event_dir_to_r ||
                scui_scene_mgr.switch_dir_cur == scui_event_dir_to_u ||
                scui_scene_mgr.switch_dir_cur == scui_event_dir_to_d) {
                scui_scene_mgr.switch_type_cur = scui_scene_mgr.switch_type_cfg;
                break;
            }
            if (scui_scene_mgr.switch_dir_cfg == scui_event_dir_to_l ||
                scui_scene_mgr.switch_dir_cfg == scui_event_dir_to_r ||
                scui_scene_mgr.switch_dir_cfg == scui_event_dir_to_u ||
                scui_scene_mgr.switch_dir_cfg == scui_event_dir_to_d) {
                scui_scene_mgr.switch_dir_cur  = scui_scene_mgr.switch_dir_cfg;
                scui_scene_mgr.switch_type_cur = scui_scene_mgr.switch_type_cfg;
                break;
            }
            scui_scene_mgr.switch_dir_cur  = scui_event_dir_none;
            scui_scene_mgr.switch_type_cur = scui_scene_switch_none;
            break;
        default:
            scui_scene_mgr.switch_dir_cur  = scui_event_dir_none;
            scui_scene_mgr.switch_type_cur = scui_scene_switch_none;
            SCUI_LOG_WARN("unknown switch type cfg, use switch none");
            break;
        }
    }
}

/*@brief 场景跳转动画回调
 *@param handle 场景句柄
 */
static void scui_scene_jump_anima_start(void *instance)
{
    SCUI_LOG_INFO("");
    scui_scene_mgr.switch_pct = 0;
    scui_scene_mgr.switch_ofs = 0;
    
    scui_point_t point = (scui_point_t){0};
    switch (scui_scene_mgr.switch_type_cur) {
    case scui_scene_switch_center_in:
    case scui_scene_switch_center_out: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_scene_mgr.switch_list[0]);
        widget->surface.alpha = scui_alpha_pct100;
        widget = scui_handle_get(scui_scene_mgr.switch_list[1]);
        widget->surface.alpha = scui_alpha_pct0;
        scui_widget_move(scui_scene_mgr.switch_list[0], &point);
        scui_widget_move(scui_scene_mgr.switch_list[1], &point);
        break;
    }
    case scui_scene_switch_zoom1:
    case scui_scene_switch_zoom2: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_scene_mgr.switch_list[0]);
        widget->surface.alpha = scui_alpha_pct100;
        widget = scui_handle_get(scui_scene_mgr.switch_list[1]);
        widget->surface.alpha = scui_alpha_pct0;
    }
    case scui_scene_switch_normal: {
        scui_widget_move(scui_scene_mgr.switch_list[0], &point);
        switch (scui_scene_mgr.switch_dir_cur) {
        case scui_event_dir_to_u:
            point.y = +scui_widget_attr_clip(scui_scene_mgr.switch_list[0]).h;
            break;
        case scui_event_dir_to_d:
            point.y = -scui_widget_attr_clip(scui_scene_mgr.switch_list[1]).h;
            break;
        case scui_event_dir_to_l:
            point.x = +scui_widget_attr_clip(scui_scene_mgr.switch_list[0]).w;
            break;
        case scui_event_dir_to_r:
            point.x = -scui_widget_attr_clip(scui_scene_mgr.switch_list[1]).w;
            break;
        default:
            SCUI_LOG_ERROR("error switch dir 0x%08x", scui_scene_mgr.switch_dir_cur);
            break;
        }
        scui_widget_move(scui_scene_mgr.switch_list[1], &point);
        break;
    }
    default:
        SCUI_LOG_ERROR("error switch tyoe 0x%08x", scui_scene_mgr.switch_type_cur);
        break;
    }
}

/*@brief 场景跳转动画回调
 *@param handle 场景句柄
 */
static void scui_scene_jump_anima_ready(void *instance)
{
    SCUI_LOG_INFO("");
    
    switch (scui_scene_mgr.switch_type_cur) {
    case scui_scene_switch_center_in:
    case scui_scene_switch_center_out:
    case scui_scene_switch_zoom1:
    case scui_scene_switch_zoom2: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_scene_mgr.switch_list[0]);
        widget->surface.alpha = scui_alpha_pct0;
        widget = scui_handle_get(scui_scene_mgr.switch_list[1]);
        widget->surface.alpha = scui_alpha_pct100;
    }
    case scui_scene_switch_normal: {
        scui_scene_mgr.switch_pct = 100;
        scui_point_t point = (scui_point_t){0};
        scui_scene_active(scui_scene_mgr.switch_list[1]);
        scui_widget_move(scui_scene_mgr.switch_list[1], &point);
        scui_widget_hide(scui_scene_mgr.switch_list[0]);
        scui_widget_refr(scui_scene_mgr.switch_list[1], true);
        break;
    }
    default:
        SCUI_LOG_ERROR("error switch tyoe 0x%08x", scui_scene_mgr.switch_type_cur);
        break;
    }
    
    SCUI_ASSERT(scui_scene_mgr.switch_anima != SCUI_HANDLE_INVALID);
    scui_anima_destroy(scui_scene_mgr.switch_anima);
    scui_scene_mgr.switch_lock = false;
}

/*@brief 场景跳转动画回调
 *@param handle 场景句柄
 */
static void scui_scene_jump_anima_expired(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t *anima = instance;
    
    if ((scui_scene_mgr.switch_dir_cur | scui_event_dir_ver) != 0)
         scui_scene_mgr.switch_ofs = scui_disp_get_ver_res();
    if ((scui_scene_mgr.switch_dir_cur | scui_event_dir_hor) != 0)
         scui_scene_mgr.switch_ofs = scui_disp_get_hor_res();
    
    scui_scene_mgr.switch_pct  = anima->value_c;
    scui_scene_mgr.switch_ofs *= anima->value_c;
    scui_scene_mgr.switch_ofs /= 100;
    
    scui_point_t point = (scui_point_t){0};
    switch (scui_scene_mgr.switch_type_cur) {
    case scui_scene_switch_center_in:
    case scui_scene_switch_center_out: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_scene_mgr.switch_list[0]);
        widget->surface.alpha = scui_alpha_by_percent(100 - scui_scene_mgr.switch_pct);
        widget = scui_handle_get(scui_scene_mgr.switch_list[1]);
        widget->surface.alpha = scui_alpha_by_percent(scui_scene_mgr.switch_pct);
        scui_widget_move(scui_scene_mgr.switch_list[0], &point);
        scui_widget_move(scui_scene_mgr.switch_list[1], &point);
        break;
    }
    case scui_scene_switch_zoom1:
    case scui_scene_switch_zoom2: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_scene_mgr.switch_list[0]);
        widget->surface.alpha = scui_alpha_by_percent(100 - scui_scene_mgr.switch_pct);
        widget = scui_handle_get(scui_scene_mgr.switch_list[1]);
        widget->surface.alpha = scui_alpha_by_percent(scui_scene_mgr.switch_pct);
    }
    case scui_scene_switch_normal: {
        scui_point_t point = (scui_point_t){0};
        switch (scui_scene_mgr.switch_dir_cur) {
        case scui_event_dir_to_u:
            point.y = -scui_scene_mgr.switch_ofs;
            scui_widget_move(scui_scene_mgr.switch_list[0], &point);
            point.y += +scui_widget_attr_clip(scui_scene_mgr.switch_list[0]).h;
            scui_widget_move(scui_scene_mgr.switch_list[1], &point);
            break;
        case scui_event_dir_to_d:
            point.y = +scui_scene_mgr.switch_ofs;
            scui_widget_move(scui_scene_mgr.switch_list[0], &point);
            point.y += -scui_widget_attr_clip(scui_scene_mgr.switch_list[1]).h;
            scui_widget_move(scui_scene_mgr.switch_list[1], &point);
            break;
        case scui_event_dir_to_l:
            point.x = -scui_scene_mgr.switch_ofs;
            scui_widget_move(scui_scene_mgr.switch_list[0], &point);
            point.x += +scui_widget_attr_clip(scui_scene_mgr.switch_list[0]).w;
            scui_widget_move(scui_scene_mgr.switch_list[1], &point);
            break;
        case scui_event_dir_to_r:
            point.x = +scui_scene_mgr.switch_ofs;
            scui_widget_move(scui_scene_mgr.switch_list[0], &point);
            point.x += -scui_widget_attr_clip(scui_scene_mgr.switch_list[1]).w;
            scui_widget_move(scui_scene_mgr.switch_list[1], &point);
            break;
        default:
            SCUI_LOG_ERROR("error switch dir 0x%08x", scui_scene_mgr.switch_dir_cur);
            break;
        }
    }
    default:
        SCUI_LOG_ERROR("error switch tyoe 0x%08x", scui_scene_mgr.switch_type_cur);
        break;
    }
}

/*@brief 场景跳转
 *@param handle 场景句柄
 *@param type   场景切换风格
 *@param dir    场景切换方向
 */
void scui_scene_jump(scui_handle_t handle, scui_scene_switch_type_t type, scui_event_dir_t dir)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    
    if (scui_scene_mgr.switch_lock) {
        SCUI_LOG_WARN("scene is switching");
        return;
    }
    if (scui_scene_mgr.switch_anima != SCUI_HANDLE_INVALID) {
        SCUI_LOG_WARN("scene is switching");
        return;
    }
    
    /* 新场景已经是焦点场景 */
    if (scui_scene_mgr.active_curr == handle) {
        scui_point_t point = (scui_point_t){0};
        scui_widget_show(scui_scene_mgr.active_curr);
        scui_widget_move(scui_scene_mgr.active_curr, &point);
        scui_scene_active(scui_scene_mgr.active_curr);
        return;
    }
    
    /* 先上锁 */
    scui_scene_mgr.switch_lock = true;
    scui_scene_mgr.switch_type_cur = type;
    scui_scene_mgr.switch_dir_cur  = dir;
    /* 自适应需要更新场景切换状态 */
    scui_scene_switch_auto_update(type, dir);
    
    /* 清除切换场景列表,回收除去焦点以外所有其他旧场景 */
    for (scui_handle_t idx = 0; idx < SCUI_SCENE_MGR_LIMIT; idx++) {
        scui_scene_mgr.switch_list[idx] = SCUI_HANDLE_INVALID;
        if (scui_scene_mgr.list[idx] != scui_scene_mgr.active_curr &&
            scui_scene_mgr.list[idx] != handle) {
            scui_widget_hide(scui_scene_mgr.list[idx]);
            SCUI_ASSERT(scui_scene_mgr.list[idx] == SCUI_HANDLE_INVALID);
            continue;
        }
    }
    
    /* 无切换效果 */
    if (scui_scene_mgr.switch_type_cur == scui_scene_switch_none) {
        scui_widget_hide(scui_scene_mgr.active_last);
        scui_widget_show(scui_scene_mgr.active_curr);
        scui_scene_active(scui_scene_mgr.active_curr);
        scui_scene_mgr.switch_lock = false;
        return;
    }
    
    scui_scene_mgr.switch_list[0] = scui_scene_mgr.active_last;
    scui_scene_mgr.switch_list[1] = scui_scene_mgr.active_curr;
    /* 其他切换效果都需要动画完成 */
    scui_anima_t switch_anima = {0};
    switch_anima.path    = scui_anima_path_bounce;
    switch_anima.start   = scui_scene_jump_anima_start;
    switch_anima.ready   = scui_scene_jump_anima_ready;
    switch_anima.expired = scui_scene_jump_anima_expired;
    switch_anima.peroid  = SCUI_SCENE_SWITCH_MS;
    scui_anima_create(&switch_anima, &scui_scene_mgr.switch_anima);
    scui_anima_start(scui_scene_mgr.switch_anima);
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_scene_event_dispatch(scui_event_t *event)
{
    return scui_event_retval_quit;
}
