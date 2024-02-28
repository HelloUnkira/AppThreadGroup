/*实现目标:
 *    窗口管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 拆分部件,这部分直接引用 */
extern scui_window_mgr_t scui_window_mgr;

/*@brief 窗口切换风格(配置)
 *@param switch_type 窗口切换风格
 */
void scui_window_switch_type_cfg(scui_window_switch_type_t switch_type)
{
    SCUI_ASSERT(switch_type != scui_window_switch_auto);
    scui_window_mgr.switch_args.cfg_type = switch_type;
}

/*@brief 窗口切换方向(配置)
 *@param switch_dir 窗口切换方向
 */
void scui_window_switch_dir_cfg(scui_event_dir_t switch_dir)
{
    scui_window_mgr.switch_args.cfg_dir = switch_dir;
}

/*@brief 窗口列表添加窗口
 *@param handle 窗口句柄
 */
void scui_window_list_add(scui_handle_t handle)
{
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_MGR_LIMIT; idx++)
        if (scui_window_mgr.list[idx] == handle) {
            SCUI_LOG_INFO("redundant operation");
            return;
        }
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_MGR_LIMIT; idx++)
        if (scui_window_mgr.list[idx] == SCUI_HANDLE_INVALID) {
            scui_window_mgr.list[idx]  = handle;
            scui_window_mgr.list_num++;
            return;
        }
    SCUI_LOG_ERROR("scene %u add fail", handle);
}

/*@brief 窗口列表移除窗口
 *@param handle 窗口句柄
 */
void scui_window_list_del(scui_handle_t handle)
{
    /* 如果移除的是焦点,记得清空 */
    if (scui_window_mgr.active_curr == handle) {
        scui_window_mgr.active_curr  = SCUI_HANDLE_INVALID;
        scui_window_mgr.active_last  = SCUI_HANDLE_INVALID;
    }
    
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_MGR_LIMIT; idx++)
        if (scui_window_mgr.list[idx] == handle) {
            scui_window_mgr.list[idx]  = SCUI_HANDLE_INVALID;
            scui_window_mgr.list_num--;
            return;
        }
    SCUI_LOG_ERROR("scene %u del fail", handle);
}

/*@brief 窗口隐藏
 *@param handle 窗口句柄
 *@param any    所有或者仅含有资源的窗口
 */
void scui_window_hide_without(scui_handle_t handle, bool any)
{
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_MGR_LIMIT; idx++)
        if (scui_window_mgr.list[idx] != SCUI_HANDLE_INVALID &&
            scui_window_mgr.list[idx] != handle) {
            scui_widget_t *widget = scui_handle_get(scui_window_mgr.list[idx]);
            SCUI_ASSERT(widget != NULL);
            SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
            if (any || scui_widget_surface_only(widget))
                scui_widget_hide(scui_window_mgr.list[idx]);
        }
}

/*@brief 获得活跃窗口句柄
 *@retval 窗口句柄
 */
scui_handle_t scui_window_active_last(void)
{
    SCUI_ASSERT(scui_handle_remap(scui_window_mgr.active_last));
    return scui_window_mgr.active_last;
}

/*@brief 获得活跃窗口句柄
 *@retval 窗口句柄
 */
scui_handle_t scui_window_active_curr(void)
{
    SCUI_ASSERT(scui_handle_remap(scui_window_mgr.active_curr));
    return scui_window_mgr.active_curr;
}

/*@brief 窗口激活
 *@param handle 窗口句柄
 */
void scui_window_active(scui_handle_t handle)
{
    SCUI_ASSERT(scui_handle_remap(handle));
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    
    if (scui_window_mgr.active_curr == handle) {
        SCUI_LOG_WARN("redundant operation");
        return;
    }
    
    /* 焦点必须在窗口列表中 */
    bool not_match_yet = true;
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_MGR_LIMIT; idx++)
        if (scui_window_mgr.list[idx] == handle) {
            not_match_yet = false;
            break;
        }
    if (not_match_yet) {
        SCUI_LOG_WARN("unknown scene %u", handle);
        return;
    }
    
    scui_event_t event = {.style = scui_event_style_sync,};
    scui_window_mgr.active_last = scui_window_mgr.active_curr;
    scui_window_mgr.active_curr = handle;
    
    /* 先失活旧的焦点窗口 */
    if (scui_handle_remap(scui_window_mgr.active_last)) {
        event.type   = scui_event_focus_lost;
        event.object = scui_window_mgr.active_last;
        scui_event_notify(&event);
    }
    
    /* 后激活新的焦点窗口 */
    event.type   = scui_event_focus_get;
    event.object = scui_window_mgr.active_curr;
    scui_event_notify(&event);
}

/*@brief 窗口跳转
 *@param handle 窗口句柄
 *@param type   窗口切换风格
 *@param dir    窗口切换方向
 */
void scui_window_jump(scui_handle_t handle, scui_window_switch_type_t type, scui_event_dir_t dir)
{
    if (scui_handle_remap(handle)) {
        scui_widget_t *widget = scui_handle_get(handle);
        SCUI_ASSERT(widget != NULL);
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    } else {
        scui_widget_maker_t *maker = scui_handle_get(handle);
        SCUI_ASSERT(maker != NULL);
        SCUI_ASSERT(maker->parent == SCUI_HANDLE_INVALID);
    }
    
    if (scui_window_mgr.switch_args.lock) {
        SCUI_LOG_WARN("scene is switching");
        return;
    }
    if (scui_window_mgr.switch_args.anima != SCUI_HANDLE_INVALID) {
        SCUI_LOG_WARN("scene is switching");
        return;
    }
    
    /* 新窗口已经是焦点窗口 */
    if (scui_window_mgr.active_curr == handle) {
        scui_point_t point = (scui_point_t){0};
        scui_widget_show(scui_window_mgr.active_curr);
        scui_widget_move(scui_window_mgr.active_curr, &point);
        scui_window_active(scui_window_mgr.active_curr);
        return;
    }
    
    /* 先上锁 */
    scui_window_mgr.switch_args.lock = true;
    scui_window_mgr.switch_args.type = type;
    scui_window_mgr.switch_args.dir  = dir;
    /* 自适应需要更新窗口切换状态 */
    scui_window_switch_type_update(type, dir);
    
    /* 清除切换窗口列表,回收除去焦点以外所有其他旧窗口 */
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_MGR_LIMIT; idx++) {
        scui_window_mgr.switch_args.list[idx] = SCUI_HANDLE_INVALID;
        if (scui_window_mgr.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        if (scui_window_mgr.list[idx] != scui_window_mgr.active_curr &&
            scui_window_mgr.list[idx] != handle) {
            scui_widget_hide(scui_window_mgr.list[idx]);
            SCUI_ASSERT(scui_window_mgr.list[idx] == SCUI_HANDLE_INVALID);
            continue;
        }
    }
    
    /* 无切换效果 */
    if (scui_window_mgr.switch_args.type == scui_window_switch_none) {
        scui_window_hide_without(scui_window_mgr.active_curr, false);
        scui_widget_hide(scui_window_mgr.active_last);
        scui_widget_show(scui_window_mgr.active_curr);
        scui_window_active(scui_window_mgr.active_curr);
        scui_window_mgr.switch_args.lock = false;
        return;
    } else {
        scui_window_mgr.switch_args.list[0] = scui_window_mgr.active_curr;
        scui_window_mgr.switch_args.list[1] = handle;
        scui_window_hide_without(scui_window_mgr.active_curr, false);
        scui_widget_show(scui_window_mgr.active_curr);
        scui_widget_show(handle);
        /* 其他切换效果都需要动画完成 */
        uint32_t peroid = (dir & scui_event_dir_hor) != 0 ? scui_disp_get_hor_res() :
                          (dir & scui_event_dir_ver) != 0 ? scui_disp_get_ver_res() : 0;
        SCUI_ASSERT(peroid != 0);
        scui_anima_t switch_anima = {0};
        switch_anima.path    = scui_anima_path_bounce;
        switch_anima.start   = scui_window_jump_anima_start;
        switch_anima.ready   = scui_window_jump_anima_ready;
        switch_anima.expired = scui_window_jump_anima_expired;
        switch_anima.peroid  = peroid;
        scui_anima_create(&switch_anima, &scui_window_mgr.switch_args.anima);
        scui_anima_start(scui_window_mgr.switch_args.anima);
    }
}
