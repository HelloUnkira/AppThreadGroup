/*实现目标:
 *    窗口:scui_test_ui_main
 *    测试入口主界面: 动态创建两个string条目, 水平居中垂直镜像对齐
 *    条目1: 进入主界面
 *    条目2: 进入测试列表
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调(条目1: 进入主界面)
 *@param event 事件
 */
void scui_test_ui_main_home_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_ptr_click:
        scui_event_mask_over(event);
        SCUI_LOG_WARN("test ui main click Home");
        scui_event_define(event_ui, SCUI_HANDLE_SYSTEM, false,
            scui_event_ui_start, NULL);
        scui_event_notify(&event_ui);
        break;
    default:
        break;
    }
}

/*@brief 控件事件响应回调(条目2: 进入测试列表)
 *@param event 事件
 */
void scui_test_ui_main_test_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_ptr_click:
        scui_event_mask_over(event);
        SCUI_LOG_WARN("test ui main click Test List");
        scui_window_stack_cover_by(SCUI_UI_SCENE_TEST_UI_LIST,
            scui_window_switch_none, scui_opt_dir_none);
        break;
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_main_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        scui_area_t widget_clip = scui_widget_clip(event->object);
        
        scui_string_maker_t string_maker = {0};
        scui_handle_t string_handle = SCUI_HANDLE_INVALID;
        
        /* 条目1: 进入主界面 (水平居中, y=1/4) */
        scui_widget_maker_def_cfg(&string_maker, scui_widget_type_string);
        string_maker.widget.parent              = event->object;
        string_maker.widget.style.indev_ptr     = true;
        string_maker.widget.style.fully_bg      = true;
        string_maker.widget.color.color.full    = 0xFF404040;
        string_maker.widget.clip.x              = widget_clip.w / 2 - 100;
        string_maker.widget.clip.y              = widget_clip.h / 4 - 30;
        string_maker.widget.clip.w              = 200;
        string_maker.widget.clip.h              = 60;
        string_maker.widget.event_cb            = scui_test_ui_main_home_event_proc;
        string_maker.args.align_hor             = 2;
        string_maker.args.align_ver             = 2;
        string_maker.args.color.color_s.full    = 0xFF00FF00;
        string_maker.args.color.color_e.full    = 0xFF00FF00;
        string_maker.args.color.filter          = true;
        string_maker.args.lang                  = scui_lang_type_en;
        string_maker.font_idx                   = SCUI_FONT_IDX_36;
        scui_widget_create(&string_maker, &string_handle);
        scui_string_update_str(string_handle, (uint8_t *)"Home");
        
        /* 条目2: 进入测试列表 (水平居中, y=3/4) */
        scui_widget_maker_def_cfg(&string_maker, scui_widget_type_string);
        string_maker.widget.parent              = event->object;
        string_maker.widget.style.indev_ptr     = true;
        string_maker.widget.style.fully_bg      = true;
        string_maker.widget.color.color.full    = 0xFF404040;
        string_maker.widget.clip.x              = widget_clip.w / 2 - 100;
        string_maker.widget.clip.y              = widget_clip.h * 3 / 4 - 30;
        string_maker.widget.clip.w              = 200;
        string_maker.widget.clip.h              = 60;
        string_maker.widget.event_cb            = scui_test_ui_main_test_event_proc;
        string_maker.args.align_hor             = 2;
        string_maker.args.align_ver             = 2;
        string_maker.args.color.color_s.full    = 0xFF00FFFF;
        string_maker.args.color.color_e.full    = 0xFF00FFFF;
        string_maker.args.color.filter          = true;
        string_maker.args.lang                  = scui_lang_type_en;
        string_maker.font_idx                   = SCUI_FONT_IDX_36;
        scui_widget_create(&string_maker, &string_handle);
        scui_string_update_str(string_handle, (uint8_t *)"Test");
        break;
    }
    default:
        break;
    }
}
