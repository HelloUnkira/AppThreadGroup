/*实现目标:
 *    窗口:切表盘过渡
 *    与 scui_ui_home 结构一致, 仅承担一次窗口切换动画:
 *    scui_ui_home --(cover, 淡入淡出)--> scui_ui_home_sw
 *    scui_ui_home_sw 获得焦点(动画完成) --(cover, 无动画)--> scui_ui_home
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    void   *cwf_json_inst;
} * scui_ui_res_local = NULL;

/* 过渡窗口本次显示的表盘索引(权威值)
 * home 在过渡期间仍可能收到编码器, 索引会被额外加减;
 * 切回 home 前用它重置, 保证与本次过渡显示的表盘一致。
 * 用文件级静态: 回跳是异步调度, 执行时窗口可能已被销毁 */
static uint32_t scui_ui_home_sw_cwf_idx = 0;

/*@brief 切页动画完成: 无动画切回 home
 *       此时 home 会重建整个控件树, 并按 presenter 中的表盘索引重建 cwf
 *@param handle 窗口句柄
 */
static void scui_ui_scene_home_sw_jump_back(scui_handle_t handle)
{
    /* 已不是活跃窗口(被其他跳转取代): 丢弃本次回跳 */
    if (scui_window_active_curr() != SCUI_UI_SCENE_HOME_SW)
        return;
    
    /* 重置表盘索引: 过渡期间编码器可能已让它额外加减 */
    scui_presenter.cwf_idx_set(scui_ui_home_sw_cwf_idx);
    SCUI_LOG_INFO("sw-jump-back: reset idx:%u", scui_ui_home_sw_cwf_idx);
    scui_window_stack_cover_by(handle, scui_window_switch_none, scui_opt_dir_none);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_home_sw_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        
        /* 表盘索引与清单由 presenter 提供(home 切表盘时写入) */
        uint32_t cwf_idx = scui_presenter.cwf_idx_get();
        if (cwf_idx >= scui_presenter.cwf_dial_num())
            cwf_idx  = 0;
        /* 记录本次过渡的权威索引(回跳前用它重置) */
        scui_ui_home_sw_cwf_idx = cwf_idx;
        scui_cwf_json_make(&scui_ui_res_local->cwf_json_inst, scui_presenter.cwf_dial_name(cwf_idx), event->object);
        SCUI_LOG_INFO("sw-create: idx:%u/%u", cwf_idx, scui_presenter.cwf_dial_num());
        break;
    }
    case scui_event_destroy:
        SCUI_LOG_INFO("sw-destroy");
        scui_cwf_json_burn(&scui_ui_res_local->cwf_json_inst);
        break;
    case scui_event_focus_get: {
        /* 切页动画已完成, 但窗口切换锁在动画完成回调的末尾才释放
         * 此处延后调度, 避免跳转被 scui_window_switch.lock_jump 拦下 */
        scui_event_define(event_jump, SCUI_HANDLE_SYSTEM, false, scui_event_sched_async, NULL);
        event_jump.style.prior = scui_event_prior_real;
        event_jump.sched       = scui_ui_scene_home_sw_jump_back;
        event_jump.handle      = SCUI_UI_SCENE_HOME;
        scui_event_notify(&event_jump);
        break;
    }
    default:
        break;
    }
}
