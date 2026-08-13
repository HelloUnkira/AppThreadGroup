/*实现目标:
 *    窗口:scui_test_ui_list
 *    测试列表: 动态创建占位标题, 后续逐步填充
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_test_ui_list_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        scui_area_t widget_clip = scui_widget_clip(event->object);
        
        scui_string_maker_t string_maker = {0};
        scui_handle_t string_handle = SCUI_HANDLE_INVALID;
        
        /* 占位标题: 水平居中, 垂直居中 */
        scui_widget_maker_def_cfg(&string_maker, scui_widget_type_string);
        string_maker.widget.parent              = event->object;
        string_maker.widget.style.fully_bg      = true;
        string_maker.widget.color.color.full    = 0xFF404040;
        string_maker.widget.clip.x              = widget_clip.w / 2 - 100;
        string_maker.widget.clip.y              = widget_clip.h / 2 - 30;
        string_maker.widget.clip.w              = 200;
        string_maker.widget.clip.h              = 60;
        string_maker.args.align_hor             = 2;
        string_maker.args.align_ver             = 2;
        string_maker.args.color.color_s.full    = 0xFF00FF00;
        string_maker.args.color.color_e.full    = 0xFF00FF00;
        string_maker.args.color.filter          = true;
        string_maker.args.lang                  = scui_lang_type_en;
        string_maker.font_idx                   = SCUI_FONT_IDX_36;
        scui_widget_create(&string_maker, &string_handle);
        scui_string_update_str(string_handle, (uint8_t *)"Test List");
        break;
    }
    case scui_event_ptr_click: {
        scui_event_mask_over(event);
        scui_ui_scene_return();
        break;
    }
    default:
        break;
    }
}
