/*实现目标:
 *    lvgl场景栈
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_event.h"
#include "app_lv_scene.h"

static uint8_t app_lv_scene_num = 0;
static app_lv_scene_t app_lv_ui_scene[APP_LV_SCENE_NEST] = {0};

/*@brief     场景调度
 *           内部使用: 被lvgl线程使用,或者扩展使用
 *@param[in] scene 场景
 */
void app_lv_scene_sched(app_lv_scene_t *scene)
{
    APP_SYS_ASSERT(scene != NULL);
    
    switch (scene->event) {
    case app_lv_scene_need_show:
        scene->show(scene);
        break;
    case app_lv_scene_need_hide:
        scene->hide(scene);
        break;
    default:
        APP_SYS_LOG_WARN("catch error event%u:", scene->event);
        break;
    }
}

/*@brief      获取次上层显示场景
 *@param[out] scene 场景(不可为空)
 */
void app_lv_scene_get_last(app_lv_scene_t **scene)
{
    APP_SYS_ASSERT(scene != NULL);
    APP_SYS_ASSERT(app_lv_scene_num > 1);
    *scene = &app_lv_ui_scene[app_lv_scene_num - 2];
}

/*@brief      获取最上层显示场景
 *@param[out] scene 场景(不可为空)
 */
void app_lv_scene_get_top(app_lv_scene_t **scene)
{
    APP_SYS_ASSERT(scene != NULL);
    APP_SYS_ASSERT(app_lv_scene_num != 0);
    *scene = &app_lv_ui_scene[app_lv_scene_num - 1];
}

/*@brief     场景复位
 *@param[in] scene 场景
 *@param[in] reserve 保留当前场景
 */
void app_lv_scene_reset(app_lv_scene_t *scene, bool reserve)
{
    app_lv_scene_t *current = NULL;
    if (reserve) {
        app_lv_scene_get_top(&current);
        app_lv_scene_num = 0;
        app_lv_ui_scene[app_lv_scene_num++] = *scene;
        app_lv_ui_scene[app_lv_scene_num++] = *current;
    } else {
        if (app_lv_scene_num >= 1) {
            app_lv_scene_get_top(&current);
            current->event = app_lv_scene_need_hide;
            app_lv_scene_update(current);
            app_lv_scene_num = 0;
        }
        if (app_lv_scene_num == 0) {
            app_lv_ui_scene[app_lv_scene_num++] = *scene;
            app_lv_scene_get_top(&current);
            current->event = app_lv_scene_need_show;
            app_lv_scene_update(current);
        }
    }
}

/*@brief     场景覆盖显示场景
 *@param[in] scene 场景
 */
void app_lv_scene_cover(app_lv_scene_t *scene)
{
    app_lv_scene_t *current = NULL;
    APP_SYS_ASSERT(scene != NULL);
    APP_SYS_ASSERT(app_lv_scene_num != 0);
    app_lv_scene_get_top(&current);
    current->event = app_lv_scene_need_hide;
    app_lv_scene_update(current);
    app_lv_ui_scene[app_lv_scene_num] = *scene;
    app_lv_scene_get_top(&current);
    current->event = app_lv_scene_need_show;
    app_lv_scene_update(current);
}

/*@brief     场景添加新场景
 *@param[in] scene 场景
 *@param[in] reserve 保留当前场景
 */
void app_lv_scene_add(app_lv_scene_t *scene, bool reserve)
{
    app_lv_scene_t *current = NULL;
    APP_SYS_ASSERT(scene != NULL);
    if (reserve) {
        app_lv_scene_get_top(&current);
        app_lv_scene_num++;
        app_lv_ui_scene[app_lv_scene_num - 2] = *scene;
        app_lv_ui_scene[app_lv_scene_num - 1] = *current;
    } else {
        app_lv_scene_get_top(&current);
        current->event = app_lv_scene_need_hide;
        app_lv_scene_update(current);
        app_lv_ui_scene[app_lv_scene_num++] = *scene;
        app_lv_scene_get_top(&current);
        current->event = app_lv_scene_need_show;
        app_lv_scene_update(current);
    }
}

/*@brief      场景移除当前显示场景
 *@param[out] scene 场景
 */
void app_lv_scene_del(app_lv_scene_t *scene)
{
    app_lv_scene_t *current = NULL;
    APP_SYS_ASSERT(scene != NULL);
    app_lv_scene_get_top(&current);
    current->event = app_lv_scene_need_hide;
    app_lv_scene_update(current);
    *scene = app_lv_ui_scene[--app_lv_scene_num];
    app_lv_scene_get_top(&current);
    current->event = app_lv_scene_need_show;
    app_lv_scene_update(current);
}

/*@brief  当前场景嵌套层级
 *@retval 场景数量
 */
uint8_t app_lv_scene_get_nest(void)
{
    APP_SYS_ASSERT(app_lv_scene_num != 0);
    return app_lv_scene_num;
}

#if 0

/*@brief    激活一个游离窗口,该窗口不受场景栈管理,被其他窗口重新继承
 *          注意:它调用了敏感资源,对它的递归调用,不能到lv_timer_handler
 *param[in] scene 游离窗口
 *param[in] right_now 是否就地激活
 */
void app_lv_scene_active(app_lv_scene_t *scene, bool right_now)
{
    APP_SYS_ASSERT(scene != NULL);
    scene->event = app_lv_scene_need_show;
    if (right_now)
        app_lv_scene_sched(scene);
    else
        app_lv_scene_update(scene);
}

/*@brief    失活一个游离窗口
 *          注意:它调用了敏感资源,对它的递归调用,不能到lv_timer_handler
 *param[in] scene 游离窗口
 *param[in] right_now 是否就地失活
 */
void app_lv_scene_hidden(app_lv_scene_t *scene, bool right_now)
{
    APP_SYS_ASSERT(scene != NULL);
    lv_obj_set_parent(scene->self, lv_scr_act());
    scene->event = app_lv_scene_need_hide;
    if (right_now)
        app_lv_scene_sched(scene);
    else
        app_lv_scene_update(scene);
}

#endif
