/*实现目标:
 *    lvgl轮盘场景移动动画
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

/*@brief 跟手动画迭代回调
 */
void app_lv_wheel_anim_exec_cb(void *var, int32_t val)
{
    app_lv_wheel_src_t *wheel_src = var;
    /* 这里应该中断吗??? */
    if (wheel_src->obj_idx >= 4)
        return;
    APP_SYS_ASSERT(wheel_src->obj_idx < 4);
    app_lv_wheel_t *wheel = wheel_src->wheel;
    lv_obj_t  *obj_self = wheel->self->root;
    lv_obj_t  *obj_sib  = wheel->sibling[wheel_src->obj_idx]->root;
    lv_coord_t wheel_w  = lv_obj_get_width(app_lv_wheel_obj_inst());
    lv_coord_t wheel_h  = lv_obj_get_height(app_lv_wheel_obj_inst());
    
    switch (wheel->style[wheel_src->obj_idx]) {
    case app_lv_wheel_style_float: {
        if (wheel_src->scroll_way == LV_DIR_HOR) {
            lv_obj_set_x(obj_sib, val);
            /*  */
            lv_coord_t scroll_dis = 0;
            if (wheel_src->obj_idx == 0) /* Scroll From Left */
                scroll_dis = val + wheel_w;
            if (wheel_src->obj_idx == 1) /* Scroll From Right */
                scroll_dis = wheel_w - val;
            lv_coord_t opa = scroll_dis * 255 / wheel_w;
            APP_SYS_LOG_DEBUG("scroll_dis:%u opa:%u", scroll_dis, opa);
            app_lv_style_opa_update(obj_sib, opa);
        }
        if (wheel_src->scroll_way == LV_DIR_VER) {
            lv_obj_set_y(obj_sib, val);
            /*  */
            lv_coord_t scroll_dis = 0;
            if (wheel_src->obj_idx == 2) /* Scroll From Top */
                scroll_dis = val + wheel_h;
            if (wheel_src->obj_idx == 3) /* Scroll From Right */
                scroll_dis = wheel_h - val;
            lv_coord_t opa = scroll_dis * 255 / wheel_h;
            APP_SYS_LOG_DEBUG("scroll_dis:%u opa:%u", scroll_dis, opa);
            app_lv_style_opa_update(obj_sib, opa);
        }
        break;
    }
    case app_lv_wheel_style_gesture:
    case app_lv_wheel_style_rotate: {
        if (wheel_src->scroll_way == LV_DIR_HOR) {
            lv_obj_set_x(obj_sib, val);
            if (wheel_src->obj_idx == 0) /* Scroll From Left */
                lv_obj_set_x(obj_self, val + wheel_w);
            if (wheel_src->obj_idx == 1) /* Scroll From Right */
                lv_obj_set_x(obj_self, val - wheel_w);
        }
        if (wheel_src->scroll_way == LV_DIR_VER) {
            lv_obj_set_y(obj_sib, val);
            if (wheel_src->obj_idx == 2) /* Scroll From Top */
                lv_obj_set_y(obj_self, val + wheel_h);
            if (wheel_src->obj_idx == 3) /* Scroll From Right */
                lv_obj_set_y(obj_self, val - wheel_h);
        }
        break;
    }
    default:
        APP_SYS_ASSERT(true == false);
        break;
    }
}

/*@brief 跟手动画开始回调
 */
void app_lv_wheel_anim_start_cb(lv_anim_t *a)
{
    APP_SYS_LOG_INFO("");
    app_lv_wheel_src_t *wheel_src = a->var;
    /* 这里应该中断吗??? */
    if (wheel_src->obj_idx >= 4)
        return;
    APP_SYS_ASSERT(wheel_src->obj_idx < 4);
    /* 动画特效:撞墙动画 */
    lv_anim_set_path_cb(a, lv_anim_path_bounce);
    APP_SYS_LOG_WARN("wheel prepare:<%u,%u>", wheel_src->obj_idx, wheel_src->scroll_way);
}

/*@brief 跟手动画结束回调
 */
void app_lv_wheel_anim_ready_cb(lv_anim_t *a)
{
    APP_SYS_LOG_INFO("");
    app_lv_wheel_src_t *wheel_src = a->var;
    /* 这里应该中断吗??? */
    if (wheel_src->obj_idx >= 4)
        return;
    APP_SYS_ASSERT(wheel_src->obj_idx < 4);
    app_lv_wheel_t *wheel = wheel_src->wheel;
    lv_obj_t  *obj_self = wheel->self->root;
    lv_obj_t  *obj_sib  = wheel->sibling[wheel_src->obj_idx]->root;
    lv_coord_t wheel_w = lv_obj_get_width(app_lv_wheel_obj_inst());
    lv_coord_t wheel_h = lv_obj_get_height(app_lv_wheel_obj_inst());
    
    switch (wheel->style[wheel_src->obj_idx]) {
    case app_lv_wheel_style_float: {
        /* 浮动结束后刷新轮盘 */
        app_lv_style_opa_update(obj_sib, 255);
        /* 触摸结束的最后一个抬起动画结束后清除捕获标记 */
        if (wheel_src->touch_over) {
            wheel_src->touch_over = false;
            wheel_src->scroll_way = LV_DIR_NONE;
            if (!wheel_src->cover)
                 wheel_src->obj_idx = 4;
            APP_SYS_LOG_WARN("wheel finish");
        }
        break;
    }
    case app_lv_wheel_style_gesture:
    case app_lv_wheel_style_rotate: {
        /* 滚动结束后刷新轮盘 */
        app_lv_scene_t *parent  = NULL;
        app_lv_scene_t *sibling = wheel->sibling[wheel_src->obj_idx];
        if (app_lv_scene_get_nest() > 1)
            app_lv_scene_get_last(&parent);
        /* 触摸结束的最后一个抬起动画结束后清除捕获标记 */
        if (wheel_src->touch_over) {
            wheel_src->touch_over = false;
            wheel_src->scroll_way = LV_DIR_NONE;
            wheel_src->obj_idx = 4;
            APP_SYS_LOG_WARN("wheel finish");
        }
        /* 覆盖标记位有效时(中心窗口已经发生改变) */
        if (wheel_src->cover) {
            wheel_src->cover = false;
            if (sibling != parent || parent == NULL) {
                app_lv_scene_t *scene = sibling;
                app_lv_scene_wheel_update(&scene, 1);
            } else {
                app_lv_scene_t *scene = NULL;
                app_lv_scene_wheel_update(&scene, 2);
            }
        }
        break;
    }
    default:
        APP_SYS_ASSERT(true == false);
        break;
    }
}
