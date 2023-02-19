/*
 *实现目的:
 *    lvgl面向功能模组的场景事件转接集合
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"

#include "app_sys_pipe.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_lvgl.h"

#include "lvgl.h"
#include "app_lv_event.h"
#include "app_lv_scene.h"
#include "app_lv_ui_scene_set.h"

/*@brief 场景内事件处理集合
 */
void app_lv_ui_scene_set_event(uint32_t event, uint8_t *data, uint32_t size)
{
    switch (event) {
    case app_thread_lvgl_ui_countdown_remind: {
        /* 倒计时提醒事件 */
        app_lv_scene_add(&app_lv_scene_countdown_remind);
        break;
    }
    default:
        break;
    }
}
