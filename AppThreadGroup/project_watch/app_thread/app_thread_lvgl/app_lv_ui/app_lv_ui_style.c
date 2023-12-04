/*实现目标:
 *    lvgl控件默认风格或者常规风格
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

/*@brief 默认控件风格
 *          黑色背景,无边框,无阴影,无间隙,无外部线
 *@param obj 控件
 */
void app_lv_style_object(lv_obj_t *obj)
{
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
    /*  */
    lv_obj_set_style_pad_all(obj, 0, 0);
    /*  */
    lv_obj_set_style_opa(obj, LV_OPA_COVER, 0);
    /*  */
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    /*  */
    lv_obj_set_style_radius(obj, 0, 0);
    /*  */
    lv_obj_set_style_border_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_FULL, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_border_color(obj, lv_color_black(), 0);
    /*  */
    lv_obj_set_style_shadow_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_shadow_width(obj, 0, 0);
    lv_obj_set_style_shadow_color(obj, lv_color_black(), 0);
    /*  */
    lv_obj_set_style_outline_pad(obj, 0, 0);
    lv_obj_set_style_outline_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_outline_width(obj, 0, 0);
    lv_obj_set_style_outline_color(obj, lv_color_black(), 0);
    /*  */
    lv_obj_set_style_text_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(obj, app_lv_multi_font_def(), 0);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(obj, lv_color_white(), 0);
}

/*@brief 默认界面风格
 *       无主题风格,全屏幕,黑色背景全覆盖,无边界,无内部间隙
 *retval 返回场景(窗口,界面)
 */
lv_obj_t * app_lv_style_scene(void)
{
    lv_obj_t *scene = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(scene);
    app_lv_style_object(scene);
    lv_obj_clear_flag(scene, LV_OBJ_FLAG_SCROLLABLE);
    /* 场景较为特殊:界面的承载体,这里还没有完成所有的事件 */
    lv_obj_set_pos(scene, 0, 0);
    lv_obj_set_size(scene, LV_HOR_RES, LV_VER_RES);
    /* 就地更新尺寸 */
    lv_obj_refr_pos(scene);
    lv_obj_refr_size(scene);
    return scene;
}
