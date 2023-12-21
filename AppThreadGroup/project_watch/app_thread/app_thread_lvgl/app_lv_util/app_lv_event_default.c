/*实现目标:
 *    lvgl事件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_lv_lib.h"

/*@brief 界面默认时间检查更新事件回调
 */
static void app_lv_event_default_check_time_cb(lv_event_t *e)
{
    /* 产生我们认为不能忽略的事件动作时,重置界面时间状态检查 */
    switch (lv_event_get_code(e))
    {
    case LV_EVENT_KEY:
    case LV_EVENT_PRESSING:
    case LV_EVENT_PRESSED:
    case LV_EVENT_RELEASED:
    case LV_EVENT_SCROLL_BEGIN:
    case LV_EVENT_SCROLL_END:
    case LV_EVENT_SCROLL:
        app_lv_check_time_reset(0, 0);
        break;
    default:
        break;
    }
}

/*@brief 界面默认手势响应事件回调
 */
static void app_lv_event_default_gesture_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        APP_SYS_LOG_WARN("LV_EVENT_GESTURE:%x", dir);
        /* 左右滑动回到上一层 */
        if ((dir & LV_DIR_LEFT) || (dir & LV_DIR_RIGHT)) {
            
            /* 场景栈手势被轮盘锁定 */
            if (app_lv_wheel_status(dir))
                break;
            /* 忽略掉当次按下,剩下的所有事件 */
            lv_indev_wait_release(lv_event_get_indev(e));
            
            /* 轮盘复位(如果未复位的话) */
            if (app_lv_wheel_rollback(0, LV_DIR_NONE) ||
                app_lv_wheel_rollback(1, dir))
                break;
            
            /* 左右滑动回到上一层 */
            if (app_lv_scene_get_nest() == 1)
                app_lv_scene_cover(&app_lv_ui_watch_face);
            if (app_lv_scene_get_nest() != 1) {
                app_lv_scene_t *scene = NULL;
                app_lv_scene_del(&scene);
            }
            break;
        }
        break;
    }
    default:
        break;
    }
}

/*@brief 界面默认按键长按压事件回调
 */
static bool app_lv_event_default_key_long_cb(lv_event_t *e)
{
    /* 内部记录按压与长按压特殊响应流程 */
    /* 用于响应内部长按压按键事件 */
    static bool catch_key_enter = false;
    static uint8_t count_key_enter = 0;
    
    switch (lv_event_get_code(e)) {
    case LV_EVENT_PRESSED:
        count_key_enter = 0;
        break;
    case LV_EVENT_RELEASED:
        count_key_enter = 0;
        catch_key_enter = false;
        break;
    case LV_EVENT_KEY: {
        uint32_t key = lv_indev_get_key(lv_indev_get_act());
        catch_key_enter = (key == LV_KEY_ENTER ? true : false);
        APP_SYS_LOG_INFO("LV_EVENT_KEY:%u", key);
        break;
    }
    case LV_EVENT_LONG_PRESSED:
        count_key_enter++;
        APP_SYS_LOG_INFO("LV_EVENT_LONG_PRESSED:%u %u", catch_key_enter, count_key_enter);
        break;
    case LV_EVENT_LONG_PRESSED_REPEAT:
        count_key_enter++;
        APP_SYS_LOG_INFO("LV_EVENT_LONG_PRESSED_REPEAT:%u %u", catch_key_enter, count_key_enter);
        break;
    default:
        break;
    }
    /* 同时抓获目标按键及其按压次数达标 */
    if (catch_key_enter && count_key_enter >= 3 * 10) {
        APP_SYS_LOG_WARN("catch key enter long click");
        /* 忽略掉当次按下,剩下的所有事件 */
        lv_indev_wait_release(lv_event_get_indev(e));
        /* 选择不同的流程 */
        if (app_module_system_mode_get() == app_module_data_center_system_mode_shutdown) {
            /* 电量不足为低电量模式,充足为正常模式 */
            app_module_system_mode_set(app_module_data_center_system_mode_normal);
            app_module_system_valid_set(false);
        } else {
            /* 电量不足为低电量模式,充足为正常模式 */
            app_module_system_mode_set(app_module_data_center_system_mode_shutdown);
            app_module_system_valid_set(false);
        }
        catch_key_enter = false;
        count_key_enter = 0;
        return true;
    }
    else if (catch_key_enter && count_key_enter != 0) {
        return true;
    }
    else {
        /* 关机模式:只允许响应按键及其相关事件 */
        if (app_module_system_mode_get() == app_module_data_center_system_mode_shutdown)
            return true;
        /* 正常模式:响应其他事件 */
    }
    
    return false;
}

/*@brief 界面默认事件组响应回调
 */
static void app_lv_event_default_group_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
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
                /* 轮盘复位(如果未复位的话) */
                if (app_lv_wheel_rollback(0, LV_DIR_NONE)  ||
                    app_lv_wheel_rollback(1, LV_DIR_RIGHT) ||
                    app_lv_wheel_rollback(1, LV_DIR_LEFT))
                    break;
                /* 主界面休眠 */
                if (app_lv_scene_get_nest() == 1) {
                    app_lv_scene_t *current = NULL;
                    app_lv_scene_get_top(&current);
                    /* 补充:如果调度主界面不是主界面,重定位它 */
                    if (current != &app_lv_ui_watch_face)
                        app_lv_scene_cover(&app_lv_ui_watch_face);
                    else
                        app_module_system_dlps_set(true);
                }
                /* 回到主界面 */
                if (app_lv_scene_get_nest() != 1)
                    app_lv_scene_reset(&app_lv_ui_watch_face, false);
            }
            /* 返回上一层 */
            if (key == LV_KEY_BACKSPACE) {
                /* 轮盘复位(如果未复位的话) */
                if (app_lv_wheel_rollback(0, LV_DIR_NONE)  ||
                    app_lv_wheel_rollback(1, LV_DIR_RIGHT) ||
                    app_lv_wheel_rollback(1, LV_DIR_LEFT))
                    break;
                /* 主界面休眠 */
                if (app_lv_scene_get_nest() == 1) {
                    app_lv_scene_t *current = NULL;
                    app_lv_scene_get_top(&current);
                    if (current == &app_lv_ui_watch_face)
                        app_module_system_dlps_set(true);
                }
                /* 返回上一层 */
                if (app_lv_scene_get_nest() != 1) {
                    app_lv_scene_t *scene = NULL;
                    app_lv_scene_del(&scene);
                }
                if (app_lv_scene_get_nest() == 1) {
                    app_lv_scene_t *current = NULL;
                    app_lv_scene_get_top(&current);
                    /* 补充:如果调度主界面不是主界面,重定位它 */
                    if (current != &app_lv_ui_watch_face)
                        app_lv_scene_cover(&app_lv_ui_watch_face);
                }
            }
            /* 主界面进入下一层 */
            if (key == LV_KEY_ENTER) {
                #if 1
                /* 非主界面响应焦点 */
                if (app_lv_scene_get_nest() != 1) {
                    #if APP_EXT_DEV_GUI_IS_LVGL
                    lv_group_t *group = app_dev_gui_drv_group_inst(&app_dev_gui_drv);
                    #else
                    lv_group_t *group = NULL;
                    #endif
                    lv_obj_t *focus_obj = lv_group_get_focused(group);
                    if (focus_obj == NULL)
                        break;
                    if (focus_obj != NULL) {
                        /* 指定类型控件才可以进入编辑模式 */
                        if (lv_obj_get_class(focus_obj) == &lv_roller_class /* || 继续添加指定类型 */) {
                            if (lv_group_get_editing(focus_obj))
                                lv_group_set_editing(focus_obj, false);
                            else
                                lv_group_set_editing(focus_obj, true);
                        } else {
                            /* 其他类型控件发送点击事件即可 */
                            lv_event_send(focus_obj, LV_EVENT_PRESSED,  lv_indev_get_act());
                            lv_event_send(focus_obj, LV_EVENT_RELEASED, lv_indev_get_act());
                            lv_event_send(focus_obj, LV_EVENT_CLICKED,  lv_indev_get_act());
                        }
                    }
                }
                #endif
                /* 主界面进入下一层 */
                if (app_lv_scene_get_nest() == 1) {
                    /* 轮盘复位(如果未复位的话) */
                    if (app_lv_wheel_rollback(0, LV_DIR_NONE))
                        break;
                    app_lv_scene_add(&app_lv_ui_theme, false);
                }
            }
            /* 模拟器上鼠标不太好通过滑动触发手势 */
            /* 这里加四个扩充按键作为手势触发,这里直接发送到顶层即可 */
            if (key == LV_KEY_UP || key == LV_KEY_DOWN || key == LV_KEY_LEFT || key == LV_KEY_RIGHT) {
                lv_dir_t dir = key == LV_KEY_UP    ? LV_DIR_TOP :
                               key == LV_KEY_DOWN  ? LV_DIR_BOTTOM :
                               key == LV_KEY_LEFT  ? LV_DIR_LEFT :
                               key == LV_KEY_RIGHT ? LV_DIR_RIGHT : LV_DIR_NONE;
                APP_SYS_ASSERT(dir != LV_DIR_NONE);
                app_lv_scene_t *scene = NULL;
                app_lv_scene_get_top(&scene);
                APP_SYS_ASSERT(scene != NULL);
                lv_indev_t *indev = lv_indev_get_act();
                /* 这样子并不好,破坏了OOP的语义,属性被暴露出来 */
                /* 但是因为源码未提供相关接口,不能擅自添加接口,这里妥协为之 */
                if (indev != NULL) {
                    indev->proc.types.pointer.gesture_dir = dir;
                    lv_event_send(app_lv_wheel_obj_inst(), LV_EVENT_GESTURE, indev);
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
        #if APP_EXT_DEV_GUI_IS_LVGL
        lv_group_t *group = app_dev_gui_drv_group_inst(&app_dev_gui_drv);
        #else
        lv_group_t *group = NULL;
        #endif
        lv_indev_t *defocus_indev = lv_event_get_param(e);
        lv_obj_t *focus_obj = lv_group_get_focused(group);
        if (defocus_indev == NULL)
            break;
        /* 编码器焦点切换事件到达 */
        if (lv_indev_get_type(defocus_indev) == LV_INDEV_TYPE_ENCODER) {
            /* 非指定类型控件则退出编辑模式 */
            if (focus_obj != NULL &&
                lv_obj_get_class(focus_obj) != &lv_roller_class /* || 继续添加指定类型 */)
                /* 只有指定类型控件才可以保持编辑模式 */
                if (lv_group_get_editing(group))
                    lv_group_set_editing(group, false);
        }
        break;
    }
    case LV_EVENT_CLICKED: {
        APP_SYS_LOG_INFO("LV_EVENT_CLICKED");
        /* DLPS界面退出 */
        if (lv_indev_get_type(lv_indev_get_act()) == LV_INDEV_TYPE_POINTER)
            app_module_system_dlps_set(false);
        break;
    }
    default:
        break;
    }
}

/*@brief 界面默认事件编码器转接响应回调
 */
static void app_lv_event_default_encode_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_KEY: {
        uint32_t key = lv_indev_get_key(lv_indev_get_act());
        /* LVGL原生的编码器事件不适合这里使用,则本地定义的编码器事件使用按键做转接 */
        /* 使用键盘的使用-描述ANTICLOCKWISE;使用+描述CLOCKWISE; */
        /* 则正式平台只需要将编码器事件转化为按键事件,在驱动层,即可 */
        
        // 编码器逆时针旋转一个单位
        // 编码器顺时针旋转一个单位
        if (key == APP_LV_EVENT_CLOCKWISE ||
            key == APP_LV_EVENT_CLOCKWISE_ANTI)
            APP_SYS_LOG_INFO("LV_EVENT_KEY: redirect to encode:%d", key);
        
        break;
    }
    default:
        break;
    }
}

/*@brief 界面默认事件响应回调
 */
void app_lv_event_default_cb(lv_event_t *e)
{
    /* 界面默认时间检查更新事件回调 */
    app_lv_event_default_check_time_cb(e);
    /* 界面默认手势响应事件回调 */
    app_lv_event_default_gesture_cb(e);
    /* 界面默认按键长按压事件回调 */
    if (app_lv_event_default_key_long_cb(e))
        return;
    /* 界面默认事件组响应回调 */
    app_lv_event_default_group_cb(e);
    /* 界面默认事件编码器转接响应回调 */
    app_lv_event_default_encode_cb(e);
    
    
    
    switch (lv_event_get_code(e)) {
    case LV_EVENT_SCROLL_BEGIN:
        APP_SYS_LOG_INFO("LV_EVENT_SCROLL_BEGIN");
        break;
    case LV_EVENT_SCROLL:
        APP_SYS_LOG_INFO("LV_EVENT_SCROLL");
        break;
    case LV_EVENT_SCROLL_END:
        APP_SYS_LOG_INFO("LV_EVENT_SCROLL_END");
        break;
    default:
        break;
    }
}

/*@brief 场景默认事件响应回调设置
 *@param scene    事件捕获场景,为NULL默认为wheel
 *@param enable   启用或者禁用
 *@param redirect 场景默认事件重定向
 */
void app_lv_event_default_config(lv_obj_t *scene, bool enable, lv_event_cb_t redirect)
{
    static bool event_config = false;
    
    scene = scene == NULL ? lv_scr_act() : scene;
    redirect = redirect == NULL ? app_lv_event_default_cb : redirect;
    
    if (enable) {
        if (event_config)
            return;
        event_config = true;
        
        #if APP_EXT_DEV_GUI_IS_LVGL
        lv_group_t *group = app_dev_gui_drv_group_inst(&app_dev_gui_drv);
        #else
        lv_group_t *group = NULL;
        #endif
        
        //lv_group_focus_freeze(group, true);
        
        lv_obj_add_event_cb(scene, redirect, LV_EVENT_ALL, NULL);
        lv_group_add_obj(group, scene);
    } else {
        if (!event_config)
             return;
        event_config = false;
        
        #if APP_EXT_DEV_GUI_IS_LVGL
        lv_group_t *group = app_dev_gui_drv_group_inst(&app_dev_gui_drv);
        #else
        lv_group_t *group = NULL;
        #endif
        
        //lv_group_focus_freeze(group, false);
        
        lv_group_remove_obj(scene);
        lv_obj_remove_event_cb(scene, redirect);
    }
}
