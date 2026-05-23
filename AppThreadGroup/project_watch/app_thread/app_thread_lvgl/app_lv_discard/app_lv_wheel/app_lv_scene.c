/*实现目标:
 *    lvgl单场景调度
 */


#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

/*@brief 场景调度
 *       内部使用: 被中心轮盘调度使用
 *@param sched 场景调度实例
 */
void app_lv_scene_sched(app_lv_scene_sched_t *sched)
{
    APP_SYS_ASSERT(sched != NULL);
    APP_SYS_ASSERT(sched->scene != NULL);
    
    switch (sched->event) {
    case app_lv_scene_event_show:
        sched->scene->show(sched->scene);
        break;
    case app_lv_scene_event_hide:
        sched->scene->hide(sched->scene);
        break;
    default:
        LV_LOG_WARN("catch error event%u:", sched->event);
        break;
    }
}
