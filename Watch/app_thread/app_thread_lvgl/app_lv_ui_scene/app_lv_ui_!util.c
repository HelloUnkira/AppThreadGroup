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

#include "lvgl.h"
#include "app_lv_event.h"
#include "app_lv_scene.h"
#include "app_lv_ui_!util.h"
#include "app_lv_ui_scene_set.h"

/*@brief 界面默认事件响应回调
 */
static void app_lv_ui_event_default(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
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
    case LV_EVENT_KEY: {
        uint32_t key = lv_indev_get_key(lv_indev_get_act());
        APP_SYS_LOG_INFO("LV_EVENT_KEY:%u\n", key);
        /* 回到主界面 */
        if (key == LV_KEY_ESC) {
            app_lv_scene_reset(&app_lv_scene_main);
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
                app_lv_scene_add(&app_lv_scene_test_sys);
            }
        }
        /* 添加其他事件 */
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
        APP_SYS_LOG_INFO("LV_EVENT_DEFOCUSED\n");
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
}

/*@brief    场景默认事件响应回调设置
 *param[in] scene 场景
 */
void app_lv_ui_event_default_set(lv_obj_t *scene)
{
    lv_obj_add_event_cb(scene, app_lv_ui_event_default, LV_EVENT_ALL, scene);
    lv_group_add_obj(lv_group_get_default(), scene);
}

/*@brief    场景默认事件响应回调清除
 *param[in] scene 场景
 */
void app_lv_ui_event_default_clr(lv_obj_t *scene)
{
    lv_group_remove_obj(scene);
    lv_obj_remove_event_cb(scene, app_lv_ui_event_default);
}

