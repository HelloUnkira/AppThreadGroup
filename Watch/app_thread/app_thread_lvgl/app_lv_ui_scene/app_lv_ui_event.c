/*
 *实现目的:
 *    lvgl事件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_lvgl.h"
#include "app_module_system.h"

#include "lvgl.h"
#include "app_lv_driver.h"
#include "app_lv_scene.h"
#include "app_lv_ui_event.h"
#include "app_lv_ui_scene.h"
#include "app_lv_ui_check_time.h"

/* 手势迟延触发(以降低点击事件产生的影响) */
static bool app_lv_ui_gesture_busy = false;
static lv_dir_t app_lv_ui_gesture_dir = LV_DIR_NONE;
static lv_anim_t app_lv_ui_gesture_anim = {0};
static lv_indev_t *app_lv_ui_gesture_indev = NULL;

/*@brief 手势迟延动画响应回调
 */
static void app_lv_ui_gesture_anim_handler(void *para, int32_t value)
{
    if (value == 1) {
        /* 左右滑动回到上一层 */
        if ((app_lv_ui_gesture_dir & LV_DIR_LEFT) ||
            (app_lv_ui_gesture_dir & LV_DIR_RIGHT)) {
            if (app_lv_scene_get_nest() > 1) {
                app_lv_scene_t scene = {0};
                app_lv_scene_del(&scene);
            }
        }
        app_lv_ui_gesture_busy = false;
        app_lv_ui_gesture_dir = LV_DIR_NONE;
        /* 恢复手势设备的禁用 */
        if (app_lv_ui_gesture_indev != NULL) {
            lv_indev_enable(app_lv_ui_gesture_indev, true);
            app_lv_ui_gesture_indev = NULL;
        }
    }
}

/*@brief 界面默认事件响应回调
 */
void app_lv_ui_event_default(lv_event_t *e)
{
    #if 0  /* 输入设备的事件表 */
    LV_EVENT_PRESSED,
    LV_EVENT_PRESSING,
    LV_EVENT_PRESS_LOST,
    LV_EVENT_SHORT_CLICKED,
    LV_EVENT_LONG_PRESSED,
    LV_EVENT_LONG_PRESSED_REPEAT,
    LV_EVENT_CLICKED,
    LV_EVENT_RELEASED,
    LV_EVENT_SCROLL_BEGIN,
    LV_EVENT_SCROLL_END,
    LV_EVENT_SCROLL,
    LV_EVENT_GESTURE,
    LV_EVENT_KEY,
    LV_EVENT_FOCUSED,
    LV_EVENT_DEFOCUSED,
    LV_EVENT_LEAVE
    #endif

    lv_event_code_t code = lv_event_get_code(e);
    /* 响应输入设备的事件 */
    switch (code) {
    case LV_EVENT_KEY: {
        uint32_t key = lv_indev_get_key(lv_indev_get_act());
        APP_SYS_LOG_INFO("LV_EVENT_KEY:%u", key);
        /* DLPS界面退出 */
        if (app_module_system_dlps_get()) {
            if (key == LV_KEY_ENTER)
                app_module_system_dlps_set(false);
        } else {
            /* 回到主界面 */
            if (key == LV_KEY_ESC) {
                /* 主界面休眠 */
                if (app_lv_scene_get_nest() == 1)
                if (!app_module_system_dlps_get())
                     app_module_system_dlps_set(true);
                /* 回到主界面 */
                if (app_lv_scene_get_nest() != 1)
                    app_lv_scene_reset(&app_lv_scene_main, false);
            }
            /* 返回上一层 */
            if (key == LV_KEY_BACKSPACE) {
                /* 主界面休眠 */
                if (app_lv_scene_get_nest() == 1)
                if (!app_module_system_dlps_get())
                     app_module_system_dlps_set(true);
                /* 返回上一层 */
                if (app_lv_scene_get_nest() != 1) {
                    app_lv_scene_t scene = {0};
                    app_lv_scene_del(&scene);
                }
            }
            /* 主界面进入下一层 */
            if (key == LV_KEY_ENTER) {
                /* 非主界面响应焦点 */
                if (app_lv_scene_get_nest() != 1) {
                    lv_obj_t *kb_focus_obj = lv_group_get_focused(app_lv_driver_get_kb_group());
                    lv_obj_t *mw_focus_obj = lv_group_get_focused(app_lv_driver_get_mw_group());
                    if (kb_focus_obj != mw_focus_obj)
                        lv_event_send(mw_focus_obj, LV_EVENT_CLICKED, lv_indev_get_act());
                }
                /* 主界面进入下一层 */
                if (app_lv_scene_get_nest() == 1)
                    app_lv_scene_add(&app_lv_scene_list, false);
            }
        }
        /* 添加其他事件 */
        break;
    }
    case LV_EVENT_CLICKED: {
        APP_SYS_LOG_INFO("LV_EVENT_CLICKED");
        /* DLPS界面退出 */
        if (lv_indev_get_type(lv_indev_get_act()) == LV_INDEV_TYPE_POINTER)
        if (app_module_system_dlps_get())
            app_module_system_dlps_set(false);
        break;
    }
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        APP_SYS_LOG_INFO("LV_EVENT_GESTURE:%x", dir);
        /* 左右滑动回到上一层 */
        if ((dir & LV_DIR_LEFT) || (dir & LV_DIR_RIGHT)) {
            /* 事件过于敏感,手势迟延动画降低干扰 */
            if (app_lv_ui_gesture_busy)
                return;
            app_lv_ui_gesture_busy = true;
            app_lv_ui_gesture_dir = dir;
            /* 临时禁用手势设备 */
            app_lv_ui_gesture_indev = lv_event_get_indev(e);
            lv_indev_enable(app_lv_ui_gesture_indev, false);
            /* 手势迟延动画 */
            lv_anim_init(&app_lv_ui_gesture_anim);
            lv_anim_set_var(&app_lv_ui_gesture_anim, NULL);
            lv_anim_set_exec_cb(&app_lv_ui_gesture_anim, app_lv_ui_gesture_anim_handler);
            lv_anim_set_repeat_count(&app_lv_ui_gesture_anim, 0);
            lv_anim_set_values(&app_lv_ui_gesture_anim, 0, 1);
            lv_anim_set_time(&app_lv_ui_gesture_anim, 300);
            lv_anim_start(&app_lv_ui_gesture_anim);
            break;
        }
        break;
    }
    case LV_EVENT_PRESSED: {
        APP_SYS_LOG_INFO("LV_EVENT_PRESSED");
        break;
    }
    case LV_EVENT_PRESSING: {
        APP_SYS_LOG_INFO("LV_EVENT_PRESSING");
        break;
    }
    case LV_EVENT_PRESS_LOST: {
        APP_SYS_LOG_INFO("LV_EVENT_PRESS_LOST");
        break;
    }
    case LV_EVENT_SHORT_CLICKED: {
        APP_SYS_LOG_INFO("LV_EVENT_SHORT_CLICKED");
        break;
    }
    case LV_EVENT_LONG_PRESSED: {
        APP_SYS_LOG_INFO("LV_EVENT_LONG_PRESSED");
        break;
    }
    case LV_EVENT_LONG_PRESSED_REPEAT: {
        APP_SYS_LOG_INFO("LV_EVENT_LONG_PRESSED_REPEAT");
        break;
    }
    case LV_EVENT_RELEASED: {
        APP_SYS_LOG_INFO("LV_EVENT_RELEASED");
        break;
    }
    case LV_EVENT_SCROLL_BEGIN: {
        APP_SYS_LOG_INFO("LV_EVENT_SCROLL_BEGIN");
        break;
    }
    case LV_EVENT_SCROLL_END: {
        APP_SYS_LOG_INFO("LV_EVENT_SCROLL_END");
        break;
    }
    case LV_EVENT_SCROLL: {
        APP_SYS_LOG_INFO("LV_EVENT_SCROLL");
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
    case LV_EVENT_PRESSING:
    case LV_EVENT_PRESSED:
    case LV_EVENT_SHORT_CLICKED:
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
 *param[in] enable 启用或者禁用
 */
void app_lv_ui_event_default_config(bool enable)
{
    static bool event_config = false;
    if (enable) {
        if (event_config)
            return;
        event_config = true;
        lv_group_t *group = app_lv_driver_get_kb_group();
        lv_obj_add_event_cb(lv_scr_act(), app_lv_ui_event_default, LV_EVENT_ALL, NULL);
        lv_group_add_obj(group, lv_scr_act());
        lv_group_focus_freeze(group, true);
    } else {
        if (!event_config)
            return;
        event_config = false;
        lv_group_t *group = app_lv_driver_get_kb_group();
        lv_group_focus_freeze(group, false);
        lv_group_remove_obj(lv_scr_act());
        lv_obj_remove_event_cb(lv_scr_act(), app_lv_ui_event_default);
    }
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
