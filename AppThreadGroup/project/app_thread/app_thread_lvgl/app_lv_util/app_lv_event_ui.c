/*实现目标:
 *    lvgl事件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_lv_lib.h"

/*@brief 界面默认事件响应回调
 */
void app_lv_event_ui_default(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    
    /* 内部记录按压与长按压特殊响应流程 */
    /* 用于响应内部长按压按键事件 */
    static bool catch_key_enter = false;
    static uint8_t count_key_enter = 0;
    switch (code) {
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
            break;
        }
        case LV_EVENT_LONG_PRESSED:
            count_key_enter += 4;
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
        return;
    }
    else if (catch_key_enter && count_key_enter != 0) {
        return;
    }
    else {
        /* 关机模式:只允许响应按键及其相关事件 */
        if (app_module_system_mode_get() == app_module_data_center_system_mode_shutdown)
            return;
        /* 正常模式:响应其他事件 */
    }
    
    /* 响应输入设备的事件 */
    switch (code) {
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        APP_SYS_LOG_INFO("LV_EVENT_GESTURE:%x", dir);
        /* 左右滑动回到上一层 */
        if ((dir & LV_DIR_LEFT) || (dir & LV_DIR_RIGHT)) {
            
            /* 场景栈手势被浮窗锁定 */
            // if (app_lv_ui_float_cannot_gestrue(dir))
            //     break;
            
            /* 忽略掉当次按下,剩下的所有事件 */
            lv_indev_wait_release(lv_event_get_indev(e));
            
            /* 浮动子窗口复位(如果未复位的话) */
            // if (app_lv_ui_float_reset())
            //     break;
            
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
                    app_lv_scene_reset(&app_lv_ui_wheel, false);
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
                            lv_event_send(mw_focus_obj, LV_EVENT_PRESSED,  lv_indev_get_act());
                            lv_event_send(mw_focus_obj, LV_EVENT_RELEASED, lv_indev_get_act());
                            lv_event_send(mw_focus_obj, LV_EVENT_CLICKED,  lv_indev_get_act());
                        }
                    }
                }
                /* 主界面进入下一层 */
                if (app_lv_scene_get_nest() == 1) {
                    /* 浮窗复位,如果可以,此时不做别的操作 */
                    // if (app_lv_ui_float_reset())
                    //     break;
                    app_lv_scene_add(&app_lv_ui_list, false);
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
                    lv_event_send(lv_scr_act(), LV_EVENT_GESTURE, indev);
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
        app_lv_check_time_reset(0, 0);
        break;
    default:
        break;
    }
}

/*@brief    场景默认事件响应回调设置
 *param[in] scene    事件捕获场景,为NULL默认为scr
 *param[in] enable   启用或者禁用
 *param[in] redirect 场景默认事件重定向
 */
void app_lv_event_ui_default_config(lv_obj_t *scene, bool enable, lv_event_cb_t redirect)
{
    scene = scene == NULL ? lv_scr_act() : scene;
    redirect = redirect == NULL ? app_lv_event_ui_default : redirect;
    
    static bool event_config = false;
    if (enable) {
        if (event_config)
            return;
        event_config = true;
        lv_group_t *group = app_lv_driver_get_kb_group();
        lv_obj_add_event_cb(scene, redirect, LV_EVENT_ALL, NULL);
        lv_group_add_obj(group, scene);
        lv_group_focus_freeze(group, true);
    } else {
        if (!event_config)
            return;
        event_config = false;
        lv_group_t *group = app_lv_driver_get_kb_group();
        lv_group_focus_freeze(group, false);
        lv_group_remove_obj(scene);
        lv_obj_remove_event_cb(scene, redirect);
    }
}

/*@brief 点击返回上一层
 */
void app_lv_event_ui_backtrack_cb(lv_event_t *e)
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

/*@brief 滚轮渐变蒙层回调
 */
void app_lv_event_ui_roller_mask_cb(lv_event_t * e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    /* 静态蒙层索引号 */
    static int16_t roller_mask_id_t = -1;   /* 顶部蒙层索引号 */
    static int16_t roller_mask_id_b = -1;   /* 底部蒙层索引号 */
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
        lv_draw_mask_fade_param_t * fade_mask_t = lv_mem_alloc(sizeof(lv_draw_mask_fade_param_t));
        lv_draw_mask_fade_init(fade_mask_t, &rect_area, LV_OPA_TRANSP, rect_area.y1, LV_OPA_COVER, rect_area.y2);
        roller_mask_id_t = lv_draw_mask_add(fade_mask_t, NULL);
        /* 重定位下部蒙层绘制域 */
        rect_area.x1 = roller_coords.x1;
        rect_area.x2 = roller_coords.x2;
        rect_area.y1 = rect_area.y2 + line_height + line_space - 1;
        rect_area.y2 = roller_coords.y2;
        /* 创建下部渐变蒙层 */
        lv_draw_mask_fade_param_t * fade_mask_b = lv_mem_alloc(sizeof(lv_draw_mask_fade_param_t));
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
        lv_mem_free(fade_mask_t);
        lv_mem_free(fade_mask_b);
        roller_mask_id_t = -1;
        roller_mask_id_b = -1;
        break;
    }
    default:
        break;
    }
}

/*@brief 折线图表下部渐变回调
 */
void app_lv_event_ui_chart_fade_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    /* 处理交互事件 */
    switch (lv_event_get_code(e)) {
    /* 控件绘制开始事件 */
    case LV_EVENT_DRAW_PART_BEGIN: {
        /* 获取事件绘制源 */
        lv_obj_draw_part_dsc_t * draw_part_dsc = lv_event_get_draw_part_dsc(e);
        /* 在绘制线条之前添加褪色区域 */
        if (draw_part_dsc->part == LV_PART_ITEMS) {
            /*  */
            if (draw_part_dsc->p1 == 0 || draw_part_dsc->p2 == 0)
                return;
            /* 使用绘制蒙版覆盖出渐变效果 */
            lv_coord_t height = lv_obj_get_height(obj);
            lv_draw_mask_line_param_t line_mask_param = {0};
            lv_draw_mask_fade_param_t fade_mask_param = {0};
            /* 添加一个线条蒙版,保持线条下方的区域 */
            lv_draw_mask_line_points_init(&line_mask_param,
                                          draw_part_dsc->p1->x,
                                          draw_part_dsc->p1->y,
                                          draw_part_dsc->p2->x,
                                          draw_part_dsc->p2->y, LV_DRAW_MASK_LINE_SIDE_BOTTOM);
            /* 添加一个渐隐蒙版,透明底覆盖顶部 */
            lv_draw_mask_fade_init(&fade_mask_param, &obj->coords,
                                    LV_OPA_COVER,  obj->coords.y1 + height / 8,
                                    LV_OPA_TRANSP, obj->coords.y2);
            /* 添加绘制蒙版 */
            int16_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);
            int16_t fade_mask_id = lv_draw_mask_add(&fade_mask_param, NULL);
            /* 绘制一个受蒙版影响的矩形 */
            lv_draw_rect_dsc_t draw_rect_dsc = {0};
            lv_draw_rect_dsc_init(&draw_rect_dsc);
            draw_rect_dsc.bg_color = draw_part_dsc->line_dsc->color;
            draw_rect_dsc.bg_opa = LV_OPA_COVER;
            /* 计算绘制域 */
            lv_area_t area = {
                .x1 = draw_part_dsc->p1->x,
                .x2 = draw_part_dsc->p2->x - 1,
                .y1 = LV_MIN(draw_part_dsc->p1->y, draw_part_dsc->p2->y),
                .y2 = obj->coords.y2,
            };
            #if 1
            /* 绘制区域 */
            lv_draw_rect(draw_part_dsc->draw_ctx, &draw_rect_dsc, &area);
            #else
            /* 绘制区域 */
            lv_draw_rect(&area, draw_part_dsc->clip_area, &draw_rect_dsc);
            #endif
            /* 移除绘制蒙版 */
            lv_draw_mask_free_param(&line_mask_param);
            lv_draw_mask_free_param(&fade_mask_param);
            lv_draw_mask_remove_id(line_mask_id);
            lv_draw_mask_remove_id(fade_mask_id);
        }
        /* 刷新背景图的分割线 */
        if(draw_part_dsc->part == LV_PART_MAIN) {
            /*  */
            if (draw_part_dsc->line_dsc == NULL || draw_part_dsc->p1 == NULL || draw_part_dsc->p2 == NULL)
                return;
            /* 垂直线 */
            if (draw_part_dsc->p1->x == draw_part_dsc->p2->x) {
                /* 虚线分界线,根据draw_part_dsc->id区分是哪一条虚线,这里统一不使用 */
                // if (draw_part_dsc->id == 0);
                draw_part_dsc->line_dsc->width = 1;
                draw_part_dsc->line_dsc->dash_gap = 1;
                draw_part_dsc->line_dsc->dash_width = 1;
                draw_part_dsc->line_dsc->color = lv_palette_main(LV_PALETTE_GREY);
                draw_part_dsc->line_dsc->opa = LV_OPA_COVER;
            /* 水平线 */
            } else {
                /* 虚线分界线,根据draw_part_dsc->id区分是哪一条虚线,这里统一不使用 */
                // if (draw_part_dsc->id == 0);
                draw_part_dsc->line_dsc->width = 1;
                draw_part_dsc->line_dsc->dash_gap = 1;
                draw_part_dsc->line_dsc->dash_width = 1;
                draw_part_dsc->line_dsc->color = lv_palette_main(LV_PALETTE_GREY);
                draw_part_dsc->line_dsc->opa = LV_OPA_COVER;
            }
        }
        break;
    }
    default:
        break;
    }
}

