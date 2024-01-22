/*实现目标:
 *    lvgl场景提醒管理
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_lv_lib.h"

/* 场景优先级表 */
/* 在表中按从上往下的顺序优先级递减 */
/* 不在表中默认可以被任何类型的提醒场景打断 */
static const app_lv_scene_t *app_lv_ui_scene_priority_table[] = {
    &app_lv_ui_countdown_remind,
    &app_lv_ui_remind_sedentary,
    &app_lv_ui_remind_drink,
};

/*@brief 特殊提醒场景优先级排序
 *       静止不高于当前提醒场景的新提醒场景打断
 */
bool app_lv_ui_scene_remind(app_lv_scene_t *remind_scene)
{
    /* 查找当前有效显示场景 */
    app_lv_scene_t *current_scene = NULL;
    app_lv_scene_get_top(&current_scene);
    if (current_scene == &app_lv_ui_dlps)
        app_lv_scene_get_last(&current_scene);
    APP_SYS_ASSERT(current_scene != NULL);
    /* 查找当前场景的优先级 */
    uint32_t priority1 = 0;
    for (priority1 = 0; priority1 < app_sys_arr_len(app_lv_ui_scene_priority_table); priority1++)
        if (app_lv_ui_scene_priority_table[priority1] == current_scene)
            break;
    /* 查找提醒场景的优先级 */
    uint32_t priority2 = 0;
    for (priority2 = 0; priority2 < app_sys_arr_len(app_lv_ui_scene_priority_table); priority2++)
        if (app_lv_ui_scene_priority_table[priority2] == remind_scene)
            break;
    /* 比较场景优先级 */
    if (priority1 <= priority2)
        return false;
    /* 勿扰模式是否产生提醒??? */
    if (app_module_do_not_disturb_status())
        return false;
    /* 是否配置无声震动? */
    if (true/* 开启震动 */) {
        /* 根据不同的场景配置不同的震动旋律 */
        app_module_vibrate_t vibrate = {
            .melody  = app_module_vibrate_melody_default_1,
            .pwm_max = 100,
            .pwm_min = 0,
            .period  = 500,
            .repeat  = 2,
        };
        /* 可以先获取震动状态考虑是否需要打断,这里默认打断 */
        app_module_vibrate_stop();
        app_module_vibrate_set(&vibrate);
        app_module_vibrate_start();
    }
    /* 如果进入到低功耗模式先唤醒场景 */
    if (app_module_system_dlps_get()) {
        app_lv_scene_add(remind_scene, true);
        app_module_system_dlps_set(false);
    } else {
        /* 提醒事件 */
        app_lv_scene_add(remind_scene, false);
    }
    return true;
}
