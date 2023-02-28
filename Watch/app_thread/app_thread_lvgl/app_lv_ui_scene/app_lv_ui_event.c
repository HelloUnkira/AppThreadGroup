/*
 *实现目的:
 *    lvgl事件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_pipe.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_lvgl.h"
#include "app_module_system.h"

#include "lvgl.h"
#include "app_lv_event.h"
#include "app_lv_scene.h"
#include "app_lv_ui_event.h"
#include "app_lv_ui_scene.h"
#include "app_lv_ui_check_time.h"

/*@brief 界面默认事件响应回调
 */
static void app_lv_ui_event_default(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_KEY: {
        uint32_t key = lv_indev_get_key(lv_indev_get_act());
        APP_SYS_LOG_INFO("LV_EVENT_KEY:%u\n", key);
        /* DLPS界面退出 */
        if (app_module_system_dlps_get()) {
            if (key == LV_KEY_ENTER) {
                app_module_system_dlps_set(false);
            }
        } else {
            /* 回到主界面 */
            if (key == LV_KEY_ESC) {
                app_lv_scene_reset(&app_lv_scene_main, false);
            }
            /* 返回上一层 */
            if (key == LV_KEY_BACKSPACE) {
                if (app_lv_scene_get_nest() > 1) {
                    app_lv_scene_t scene = {0};
                    app_lv_scene_del(&scene);
                }
            }
            /* 主界面进入下一层 */
            if (key == LV_KEY_ENTER) {
                if (app_lv_scene_get_nest() == 1) {
                    app_lv_scene_add(&app_lv_scene_list, false);
                }
            }
        }
        /* 添加其他事件 */
        break;
    }
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        APP_SYS_LOG_INFO("LV_EVENT_GESTURE:%x\n", dir);
        /* 左右滑动回到上一层 */
        if ((dir & LV_DIR_LEFT) || (dir & LV_DIR_RIGHT)) {
            if (app_lv_scene_get_nest() > 1) {
                app_lv_scene_t scene = {0};
                app_lv_scene_del(&scene);
            }
        }
        break;
    }
    case LV_EVENT_PRESSED: {
        APP_SYS_LOG_INFO("LV_EVENT_PRESSED\n");
        break;
    }
    case LV_EVENT_PRESSING: {
        APP_SYS_LOG_INFO("LV_EVENT_PRESSING\n");
        break;
    }
    case LV_EVENT_PRESS_LOST: {
        APP_SYS_LOG_INFO("LV_EVENT_PRESS_LOST\n");
        break;
    }
    case LV_EVENT_SHORT_CLICKED: {
        APP_SYS_LOG_INFO("LV_EVENT_SHORT_CLICKED\n");
        break;
    }
    case LV_EVENT_LONG_PRESSED: {
        APP_SYS_LOG_INFO("LV_EVENT_LONG_PRESSED\n");
        break;
    }
    case LV_EVENT_LONG_PRESSED_REPEAT: {
        APP_SYS_LOG_INFO("LV_EVENT_LONG_PRESSED_REPEAT\n");
        break;
    }
    case LV_EVENT_CLICKED: {
        APP_SYS_LOG_INFO("LV_EVENT_CLICKED\n");
        break;
    }
    case LV_EVENT_RELEASED: {
        APP_SYS_LOG_INFO("LV_EVENT_RELEASED\n");
        break;
    }
    case LV_EVENT_SCROLL_BEGIN: {
        APP_SYS_LOG_INFO("LV_EVENT_SCROLL_BEGIN\n");
        break;
    }
    case LV_EVENT_SCROLL_END: {
        APP_SYS_LOG_INFO("LV_EVENT_SCROLL_END\n");
        break;
    }
    case LV_EVENT_SCROLL: {
        APP_SYS_LOG_INFO("LV_EVENT_SCROLL\n");
        break;
    }
    case LV_EVENT_FOCUSED: {
        APP_SYS_LOG_INFO("LV_EVENT_FOCUSED\n");
        break;
    }
    case LV_EVENT_DEFOCUSED: {
        APP_SYS_LOG_INFO("LV_EVENT_DEFOCUSED\n");
        break;
    }
    case LV_EVENT_LEAVE: {
        APP_SYS_LOG_INFO("LV_EVENT_LEAVE\n");
        break;
    }
    case LV_EVENT_HIT_TEST: {
        APP_SYS_LOG_INFO("LV_EVENT_HIT_TEST\n");
        break;
    }
    default:
        break;
    }
    /* 产生我们认为不能忽略的事件动作时,重置界面时间状态检查 */
    switch (code)
    {
    case LV_EVENT_KEY:
    case LV_EVENT_GESTURE:
    case LV_EVENT_PRESSED:
    case LV_EVENT_CLICKED:
    case LV_EVENT_RELEASED:
    case LV_EVENT_SCROLL_BEGIN:
    case LV_EVENT_SCROLL_END:
    case LV_EVENT_SCROLL:
        app_lv_ui_check_time_reset(0, 0);
        break;
    default:
        break;
    }
}

/*@brief    场景默认事件响应回调设置
 *param[in] scene 场景
 */
void app_lv_ui_event_default_set(lv_obj_t *scene)
{
    lv_group_t *group = lv_group_get_default();
    lv_obj_add_event_cb(scene, app_lv_ui_event_default, LV_EVENT_ALL, scene);
    lv_group_add_obj(group, scene);
    lv_group_focus_freeze(group, true);
}

/*@brief    场景默认事件响应回调清除
 *param[in] scene 场景
 */
void app_lv_ui_event_default_clr(lv_obj_t *scene)
{
    lv_group_t *group = lv_group_get_default();
    lv_group_focus_freeze(group, false);
    lv_group_remove_obj(scene);
    lv_obj_remove_event_cb(scene, app_lv_ui_event_default);
}

/*@brief 滚轮事件自定义回调
 */
void app_lv_ui_roller_mask_event_cb(lv_event_t * e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    /* 静态蒙层索引号 */
    static lv_coord_t roller_mask_id_t = -1;   /* 顶部蒙层索引号 */
    static lv_coord_t roller_mask_id_b = -1;   /* 底部蒙层索引号 */
    /* 处理交互事件 */
    switch (lv_event_get_code(e)) {
    /* 覆盖检查事件 */
    case LV_EVENT_COVER_CHECK: {
        lv_event_set_cover_res(e, LV_COVER_RES_MASKED);
        break;
    }
    /* 控件绘制开始事件 */
    case LV_EVENT_DRAW_MAIN_BEGIN: {
        /* 提取绘制域信息 */
        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        lv_coord_t line_space  = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
        lv_coord_t line_height = lv_font_get_line_height(font);
        /* 定位滚轮坐标 */
        lv_area_t roller_coords;
        lv_obj_get_coords(obj, &roller_coords);
        /* 重定位上部蒙层绘制域 */
        lv_area_t rect_area;
        rect_area.x1 = roller_coords.x1;
        rect_area.x2 = roller_coords.x2;
        rect_area.y1 = roller_coords.y1;
        rect_area.y2 = roller_coords.y1 + (lv_obj_get_height(obj) - line_height - line_space) / 2;
        /* 创建上部渐变蒙层 */
        lv_draw_mask_fade_param_t * fade_mask_t = app_mem_alloc(sizeof(lv_draw_mask_fade_param_t));
        lv_draw_mask_fade_init(fade_mask_t, &rect_area, LV_OPA_TRANSP, rect_area.y1, LV_OPA_COVER, rect_area.y2);
        roller_mask_id_t = lv_draw_mask_add(fade_mask_t, NULL);
        /* 重定位下部蒙层绘制域 */
        rect_area.x1 = roller_coords.x1;
        rect_area.x2 = roller_coords.x2;
        rect_area.y1 = rect_area.y2 + line_height + line_space - 1;
        rect_area.y2 = roller_coords.y2;
        /* 创建下部渐变蒙层 */
        lv_draw_mask_fade_param_t * fade_mask_b = app_mem_alloc(sizeof(lv_draw_mask_fade_param_t));
        lv_draw_mask_fade_init(fade_mask_b, &rect_area, LV_OPA_COVER, rect_area.y1, LV_OPA_TRANSP, rect_area.y2);
        roller_mask_id_b = lv_draw_mask_add(fade_mask_b, NULL);
        break;
    }
    /* 控件绘制结束推送事件 */
    case LV_EVENT_DRAW_POST_END: {
        lv_draw_mask_fade_param_t * fade_mask_t = lv_draw_mask_remove_id(roller_mask_id_t);
        lv_draw_mask_fade_param_t * fade_mask_b = lv_draw_mask_remove_id(roller_mask_id_b);
//        lv_draw_mask_free_param(fade_mask_t);
//        lv_draw_mask_free_param(fade_mask_b);
        app_mem_free(fade_mask_t);
        app_mem_free(fade_mask_b);
        roller_mask_id_t = -1;
        roller_mask_id_b = -1;
        break;
    }
    default:
        break;
    }
}

/*@brief 场景内事件处理集合
 */
void app_lv_ui_scene_event(uint32_t event, uint8_t *data, uint32_t size)
{
    switch (event) {
    case app_thread_lvgl_ui_countdown_remind: {
        /* 如果进入到低功耗模式先唤醒场景 */
        if (app_module_system_dlps_get()) {
            app_lv_scene_add(&app_lv_scene_countdown_remind, true);
            app_module_system_dlps_set(false);
        } else {
            /* 倒计时提醒事件 */
            app_lv_scene_add(&app_lv_scene_countdown_remind, false);
        }
        break;
    }
    default:
        break;
    }
}
