/*实现目标:
 *    场景管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 拆分部件,这部分直接引用 */
extern scui_scene_mgr_t scui_scene_mgr;

/*@brief 场景切换风格(配置)
 *@param switch_type 场景切换风格
 */
void scui_scene_switch_type_cfg(scui_scene_switch_type_t switch_type)
{
    SCUI_ASSERT(switch_type != scui_scene_switch_auto);
    scui_scene_mgr.switch_type_cfg = switch_type;
}

/*@brief 场景切换方向(配置)
 *@param switch_dir 场景切换方向
 */
void scui_scene_switch_dir_cfg(scui_event_dir_t switch_dir)
{
    scui_scene_mgr.switch_dir_cfg = switch_dir;
}

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

/*@brief 场景跳转
 *@param handle 场景句柄
 *@param type   场景切换风格
 *@param dir    场景切换方向
 */
void scui_scene_jump(scui_handle_t handle, scui_scene_switch_type_t type, scui_event_dir_t dir)
{
    /* 外联引用 */
    void scui_scene_jump_auto_update(scui_scene_switch_type_t type, scui_event_dir_t dir);
    void scui_scene_jump_anima_start(void *anima);
    void scui_scene_jump_anima_ready(void *anima);
    void scui_scene_jump_anima_expired(void *anima);
    
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
    scui_scene_jump_auto_update(type, dir);
    
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
    } else {
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
}
