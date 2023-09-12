/*实现目标:
 *    lvgl轮盘场景移动动画
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

/*@brief 浮动子窗口透明度递归改变回调
 */
static void app_lv_wheel_opa_update(lv_obj_t *obj, uint8_t opa)
{
    lv_obj_set_style_opa(obj, opa, 0);
    /* 需要递归??? */
    return;
    
    for (uint32_t idx = 0; idx < lv_obj_get_child_cnt(obj); idx++)
         app_lv_wheel_opa_update(lv_obj_get_child(obj, idx), opa);
}

/*@brief 跟手动画迭代回调
 */
void app_lv_wheel_anim_exec_cb(void *var, int32_t val)
{
    app_lv_wheel_src_t *wheel_src = var;
    app_lv_wheel_t *wheel = wheel_src->wheel;
    lv_obj_t  *obj_self = wheel->self->root;
    lv_obj_t  *obj_sib  = wheel->sibling[wheel_src->obj_idx]->root;
    lv_coord_t wheel_w  = lv_obj_get_width(app_lv_wheel_obj_inst());
    lv_coord_t wheel_h  = lv_obj_get_height(app_lv_wheel_obj_inst());
    
    switch (wheel->style[wheel_src->obj_idx]) {
    case app_lv_wheel_style_float:
        if (wheel_src->scroll_way == LV_DIR_HOR) {
            uint8_t opa = wheel_src->scroll_per * 255 / wheel_w;
            app_lv_wheel_opa_update(obj_sib, opa);
            lv_obj_set_x(obj_sib, val);
        }
        if (wheel_src->scroll_way == LV_DIR_VER) {
            uint8_t opa = wheel_src->scroll_per * 255 / wheel_h;
            app_lv_wheel_opa_update(obj_sib, opa);
            lv_obj_set_y(obj_sib, val);
        }
        break;
    case app_lv_wheel_style_rotate:
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
    APP_SYS_LOG_WARN("wheel prepare:<%u,%u>", wheel_src->obj_idx, wheel_src->scroll_way);
}

/*@brief 跟手动画结束回调
 */
void app_lv_wheel_anim_ready_cb(lv_anim_t *a)
{
    APP_SYS_LOG_INFO("");
    app_lv_wheel_src_t *wheel_src = a->var;
    app_lv_wheel_t *wheel = wheel_src->wheel;
    lv_coord_t wheel_w = lv_obj_get_width(app_lv_wheel_obj_inst());
    lv_coord_t wheel_h = lv_obj_get_height(app_lv_wheel_obj_inst());
    APP_SYS_ASSERT(wheel_src->obj_idx <= 4);
    
    switch (wheel->style[wheel_src->obj_idx]) {
    case app_lv_wheel_style_float:
        /* 触摸结束的最后一个抬起动画结束后清除捕获标记 */
        if (wheel_src->touch_over) {
            wheel_src->touch_over = false;
            wheel_src->scroll_way = LV_DIR_NONE;
            APP_SYS_LOG_WARN("wheel finish");
        }
        break;
    case app_lv_wheel_style_rotate:
        /* 覆盖标记位有效时 */
         if (wheel_src->cover) {
             wheel_src->cover = false;
            APP_SYS_LOG_WARN("wheel update");
            /* 滚动结束后需要刷新轮盘(中心窗口已经发生改变) */
            app_lv_scene_t *parent  = NULL;
            app_lv_scene_t *sibling = wheel->sibling[wheel_src->obj_idx];
            app_lv_scene_get_last(&parent);
            if (sibling != parent) {
                app_lv_scene_t *scene = sibling;
                app_lv_scene_cover(scene);
            } else {
                app_lv_scene_t *scene = NULL;
                app_lv_scene_del(&scene);
            }
            /* 触摸结束的最后一个抬起动画结束后清除捕获标记 */
            if (wheel_src->touch_over) {
                wheel_src->touch_over = false;
                wheel_src->scroll_way = LV_DIR_NONE;
                APP_SYS_LOG_WARN("wheel finish");
            }
        }
        break;
    default:
        APP_SYS_ASSERT(true == false);
        break;
    }
}
