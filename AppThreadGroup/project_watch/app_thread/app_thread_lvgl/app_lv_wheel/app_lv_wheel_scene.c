/*实现目标:
 *    lvgl轮盘场景调度
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

/*@brief     轮盘显示(激活)
 *@param[in] wheel 轮盘实例
 */
void app_lv_wheel_show(app_lv_wheel_t *wheel)
{
    /* 子场景要逆序显示,显示的子场景挂载到轮盘场景之下 */
    lv_obj_t *parent = app_lv_wheel_obj_inst();
    APP_SYS_ASSERT(parent != NULL);
    /* 显示当前场景,防止场景重复被显示 */
    if (wheel->self != NULL && wheel->self->root == NULL) {
        /* 调度子场景,目标对象挂载到轮盘对象之下 */
        app_lv_scene_sched_t sched = {
            .scene = wheel->self,
            .event = app_lv_scene_event_show,
        };
        app_lv_scene_sched(&sched);
        lv_obj_t *child = sched.scene->root;
        lv_obj_set_parent(child, parent);
    }
    /* 显示兄弟场景 */
    for (uint8_t idx = 0; idx < app_sys_arr_len(wheel->sibling); idx++) {
        /* 防止场景重复被显示,不应该存在俩个相同的兄弟场景 */
        if (wheel->sibling[idx] == NULL ||
            wheel->sibling[idx] == wheel->self ||
            wheel->sibling[idx]->root != NULL)
            continue;
        /* 调度子场景,目标对象挂载到轮盘对象之下 */
        app_lv_scene_sched_t sched = {
            .scene = wheel->sibling[idx],
            .event = app_lv_scene_event_show,
        };
        app_lv_scene_sched(&sched);
        lv_obj_t *child = sched.scene->root;
        lv_obj_set_parent(child, parent);
    }
}

/*@brief     轮盘隐藏(失活)
 *@param[in] wheel 轮盘实例
 */
void app_lv_wheel_hide(app_lv_wheel_t *wheel)
{
    /* 子场景要逆序隐藏,隐藏的子场景移除出轮盘场景 */
    lv_obj_t *parent = app_lv_wheel_obj_inst();
    APP_SYS_ASSERT(parent != NULL);
    /* 隐藏兄弟场景 */
    for (uint8_t idx = 0; idx < app_sys_arr_len(wheel->sibling); idx++) {
        /* 防止场景重复被隐藏,不应该存在俩个相同的兄弟场景 */
        if (wheel->sibling[idx] == NULL ||
            wheel->sibling[idx] == wheel->self ||
            wheel->sibling[idx]->root == NULL)
            continue;
        /* 调度子场景,目标对象挂载到根对象之下 */
        app_lv_scene_sched_t sched = {
            .scene = wheel->sibling[idx],
            .event = app_lv_scene_event_hide,
        };
        lv_obj_t *child = sched.scene->root;
        lv_obj_set_parent(child, lv_scr_act());
        app_lv_scene_sched(&sched);
    }
    /* 隐藏当前场景,防止场景重复被隐藏 */
    if (wheel->self != NULL && wheel->self->root != NULL) {
        /* 调度子场景,目标对象挂载到根对象之下 */
        app_lv_scene_sched_t sched = {
            .scene = wheel->self,
            .event = app_lv_scene_event_hide,
        };
        lv_obj_t *child = sched.scene->root;
        lv_obj_set_parent(child, lv_scr_act());
        app_lv_scene_sched(&sched);
    }
}

/*@brief     轮盘子场景状态复位
 *@param[in] wheel 轮盘实例
 */
void app_lv_wheel_reset(app_lv_wheel_t *wheel)
{
    app_lv_wheel_src_t *wheel_src = app_lv_wheel_src_inst();

    for (uint8_t idx = 0; idx < app_sys_arr_len(wheel->sibling); idx++) {
        if (wheel->sibling[idx] == NULL ||
            wheel->sibling[idx] == wheel->self ||
            wheel->sibling[idx]->root == NULL)
            continue;
        lv_obj_t *obj = wheel->sibling[idx]->root;
        /* 目标移动到屏幕之外(当前场景附近(-w,-h)) */
        /* 动画会自动更新路径,这里将它移到屏幕显示区域外面即可 */
        lv_obj_set_x(obj, -lv_obj_get_width(obj));
        lv_obj_set_y(obj, -lv_obj_get_height(obj));
    }
    {
        lv_obj_t *obj = wheel->self->root;
        lv_obj_set_x(obj, 0);
        lv_obj_set_y(obj, 0);
    }
    /* 配置项更新 */
    wheel_src->scroll_way  = LV_DIR_NONE;
    wheel_src->threshold   = 30;
    wheel_src->rollback_x  = lv_obj_get_width(app_lv_wheel_obj_inst()) / 3;
    wheel_src->rollback_y  = lv_obj_get_height(app_lv_wheel_obj_inst()) / 3;
    wheel_src->anim_period = 500;
    /* 状态项更新 */
    wheel_src->obj_idx    = 4;
    wheel_src->event_lock = false;
    wheel_src->touch_over = true;
    wheel_src->scroll     = false;
    /* 资源绑定 */
    wheel_src->wheel = wheel;
    /* 初始化并配置跟手特效动画 */
    lv_anim_init(        &wheel_src->anim_follow);
    lv_anim_set_var(     &wheel_src->anim_follow, wheel_src);
    lv_anim_set_time(    &wheel_src->anim_follow, wheel_src->anim_period);
    lv_anim_set_exec_cb( &wheel_src->anim_follow, app_lv_wheel_anim_exec_cb);
    lv_anim_set_start_cb(&wheel_src->anim_follow, app_lv_wheel_anim_start_cb);
    lv_anim_set_ready_cb(&wheel_src->anim_follow, app_lv_wheel_anim_ready_cb);
    /* 为轮盘挂载事件回调 */
    lv_obj_add_event_cb(app_lv_wheel_obj_inst(), app_lv_wheel_event_cb, LV_EVENT_ALL, wheel_src);
}
