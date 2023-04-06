/*
 *实现目的:
 *    lvgl事件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_lvgl.h"
#include "app_module_clock.h"
#include "app_module_remind_group.h"
#include "app_module_remind_alarm.h"
#include "app_module_system.h"

#include "lvgl.h"
#include "app_lv_drv.h"
#include "app_lv_scene.h"
#include "app_lv_ui_scene.h"
#include "app_lv_ui_float.h"
#include "app_lv_ui_check_time.h"

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
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        APP_SYS_LOG_INFO("LV_EVENT_GESTURE:%x", dir);
        /* 左右滑动回到上一层 */
        if ((dir & LV_DIR_LEFT) || (dir & LV_DIR_RIGHT)) {
            /* 场景栈手势被浮窗锁定 */
            if (app_lv_ui_float_cannot_gestrue(dir))
                break;
            /* 忽略掉当次按下,剩下的所有事件 */
            lv_indev_wait_release(lv_event_get_indev(e));
            /* 浮动子窗口复位(如果未复位的话) */
            if (app_lv_ui_float_reset())
                break;
            /* 左右滑动回到上一层 */
            if (app_lv_scene_get_nest() > 1) {
                app_lv_scene_t scene = {0};
                app_lv_scene_del(&scene);
            }
            break;
        }
        break;
    }
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
                    app_lv_scene_reset(&app_lv_ui_main, false);
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
                    lv_group_t *kb_group = app_lv_driver_get_kb_group();
                    lv_group_t *mw_group = app_lv_driver_get_mw_group();
                    lv_obj_t *kb_focus_obj = lv_group_get_focused(kb_group);
                    lv_obj_t *mw_focus_obj = lv_group_get_focused(mw_group);
                    if (kb_focus_obj == NULL || mw_focus_obj == NULL)
                        break;
                    if (kb_focus_obj != mw_focus_obj) {
                        /* 指定类型控件才可以进入编辑模式 */
                        if (lv_obj_get_class(mw_focus_obj) == &lv_roller_class /* || 继续添加指定类型 */) {
                            if (lv_group_get_editing(mw_group))
                                lv_group_set_editing(mw_group, false);
                            else
                                lv_group_set_editing(mw_group, true);
                        } else {
                            /* 其他类型控件发送点击事件即可 */
                            lv_event_send(mw_focus_obj, LV_EVENT_CLICKED, lv_indev_get_act());
                        }
                    }
                }
                /* 主界面进入下一层 */
                if (app_lv_scene_get_nest() == 1) {
                    /* 浮窗复位,如果可以,此时不做别的操作 */
                    if (app_lv_ui_float_reset())
                        break;
                    app_lv_scene_add(&app_lv_ui_list, false);
                }
            }
        }
        /* 添加其他事件 */
        break;
    }
    case LV_EVENT_FOCUSED: {
        APP_SYS_LOG_INFO("LV_EVENT_FOCUSED");
        break;
    }
    case LV_EVENT_DEFOCUSED: {
        APP_SYS_LOG_INFO("LV_EVENT_DEFOCUSED");
        /* 更新焦点后及时退出编辑模式 */
        lv_group_t *mw_group = app_lv_driver_get_mw_group();
        lv_indev_t *defocus_indev = lv_event_get_param(e);
        lv_obj_t *mw_focus_obj = lv_group_get_focused(mw_group);
        if (defocus_indev == NULL)
            break;
        /* 编码器焦点切换事件到达 */
        if (lv_indev_get_type(defocus_indev) == LV_INDEV_TYPE_ENCODER) {
            /* 非指定类型控件则退出编辑模式 */
            if (mw_focus_obj != NULL &&
                lv_obj_get_class(mw_focus_obj) != &lv_roller_class /* || 继续添加指定类型 */)
                /* 只有指定类型控件才可以保持编辑模式 */
                if (lv_group_get_editing(mw_group))
                    lv_group_set_editing(mw_group, false);
        }
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
        lv_obj_t *obj = lv_indev_get_scroll_obj(lv_indev_get_act());
        if (obj == NULL)
            break;
        APP_SYS_LOG_DEBUG("scroll_x:%d",        lv_obj_get_scroll_x(obj));
        APP_SYS_LOG_DEBUG("scroll_y:%d",        lv_obj_get_scroll_y(obj));
        APP_SYS_LOG_DEBUG("scroll_top:%d",      lv_obj_get_scroll_top(obj));
        APP_SYS_LOG_DEBUG("scroll_bottom:%d",   lv_obj_get_scroll_bottom(obj));
        APP_SYS_LOG_DEBUG("scroll_left:%d",     lv_obj_get_scroll_left(obj));
        APP_SYS_LOG_DEBUG("scroll_right:%d",    lv_obj_get_scroll_right(obj));
        break;
    }
    case LV_EVENT_SCROLL_END: {
        APP_SYS_LOG_INFO("LV_EVENT_SCROLL_END");
        lv_obj_t *obj = lv_indev_get_scroll_obj(lv_indev_get_act());
        if (obj == NULL)
            break;
        APP_SYS_LOG_DEBUG("scroll_x:%d",        lv_obj_get_scroll_x(obj));
        APP_SYS_LOG_DEBUG("scroll_y:%d",        lv_obj_get_scroll_y(obj));
        APP_SYS_LOG_DEBUG("scroll_top:%d",      lv_obj_get_scroll_top(obj));
        APP_SYS_LOG_DEBUG("scroll_bottom:%d",   lv_obj_get_scroll_bottom(obj));
        APP_SYS_LOG_DEBUG("scroll_left:%d",     lv_obj_get_scroll_left(obj));
        APP_SYS_LOG_DEBUG("scroll_right:%d",    lv_obj_get_scroll_right(obj));
        break;
    }
    case LV_EVENT_SCROLL: {
        APP_SYS_LOG_INFO("LV_EVENT_SCROLL");
        lv_obj_t *obj = lv_indev_get_scroll_obj(lv_indev_get_act());
        if (obj == NULL)
            break;
        APP_SYS_LOG_DEBUG("scroll_x:%d",        lv_obj_get_scroll_x(obj));
        APP_SYS_LOG_DEBUG("scroll_y:%d",        lv_obj_get_scroll_y(obj));
        APP_SYS_LOG_DEBUG("scroll_top:%d",      lv_obj_get_scroll_top(obj));
        APP_SYS_LOG_DEBUG("scroll_bottom:%d",   lv_obj_get_scroll_bottom(obj));
        APP_SYS_LOG_DEBUG("scroll_left:%d",     lv_obj_get_scroll_left(obj));
        APP_SYS_LOG_DEBUG("scroll_right:%d",    lv_obj_get_scroll_right(obj));
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
 *param[in] scene  事件捕获场景,为NULL默认为scr
 *param[in] enable 启用或者禁用
 */
void app_lv_ui_event_default_config(lv_obj_t *scene, bool enable)
{
    scene = scene == NULL ? lv_scr_act() : scene;
    static bool event_config = false;
    if (enable) {
        if (event_config)
            return;
        event_config = true;
        lv_group_t *group = app_lv_driver_get_kb_group();
        lv_obj_add_event_cb(scene, app_lv_ui_event_default, LV_EVENT_ALL, NULL);
        lv_group_add_obj(group, scene);
        lv_group_focus_freeze(group, true);
    } else {
        if (!event_config)
            return;
        event_config = false;
        lv_group_t *group = app_lv_driver_get_kb_group();
        lv_group_focus_freeze(group, false);
        lv_group_remove_obj(scene);
        lv_obj_remove_event_cb(scene, app_lv_ui_event_default);
    }
}

/*@brief 点击返回上一层
 */
void app_lv_ui_event_click_turn_back_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        app_lv_scene_t scene = {0};
        app_lv_scene_del(&scene);
        break;
    }
    default:
        break;
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
            app_lv_scene_add(&app_lv_ui_countdown_remind, true);
            app_module_system_dlps_set(false);
        } else {
            /* 倒计时提醒事件 */
            app_lv_scene_add(&app_lv_ui_countdown_remind, false);
        }
        break;
    }
    case app_thread_lvgl_ui_remind_alarm: {
        app_module_remind_package_t *remind = data;
        remind->remind_group;
        remind->remind_item;
        remind->remind_type;
        break;
    }
    case app_thread_lvgl_ui_remind_drink: {
        /* 如果进入到低功耗模式先唤醒场景 */
        if (app_module_system_dlps_get()) {
            app_lv_scene_add(&app_lv_ui_remind_drink, true);
            app_module_system_dlps_set(false);
        } else {
            /* 喝水提醒事件 */
            app_lv_scene_add(&app_lv_ui_remind_drink, false);
        }
        break;
    }
    default:
        break;
    }
}
