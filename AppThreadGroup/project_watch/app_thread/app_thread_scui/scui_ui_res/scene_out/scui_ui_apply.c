/*实现目标:
 *    控件样式应用
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件应用样式回调
 *@param handle 控件句柄
 */
void scui_ui_apply(scui_handle_t handle)
{
    switch (scui_widget_type(handle)) {
    case scui_widget_type_menial: {
        scui_menial_type_t  type = scui_menial_type_none;
        scui_menial_data_t *data = NULL;
        scui_menial_type_sub(handle, &type);
        scui_menial_data_inst(handle, &data);
        
        switch (type) {
        case scui_menial_type_btn: {
            /* 常规 res: 天蓝圆角, 点击变红 */
            scui_menial_btn_res_t res = {0};
            res.color[0].color_l.full = 0xFF2196F3;
            res.color[0].color_d.full = 0xFFF44336;
            res.radius = -1;
            
            res.part = scui_object_part_rect_bg;
            scui_menial_btn_style(handle, &res);
            break;
        }
        case scui_menial_type_arc: {
            /* 常规 res: 背景灰, 前景天蓝, 0~360度 */
            scui_menial_arc_res_t res = {0};
            res.color[0].color_s.full = 0xFF9E9E9E;
            res.color[0].color_e.full = 0xFF9E9E9E;
            res.color[1].color_s.full = 0xFF2196F3;
            res.color[1].color_e.full = 0xFF2196F3;
            res.round = true;
            
            res.part = scui_object_part_arc_bg;
            scui_menial_arc_style(handle, &res);
            res.part = scui_object_part_arc_fg;
            scui_menial_arc_style(handle, &res);
            break;
        }
        case scui_menial_type_bar: {
            /* 常规 res: 背景浅灰, 前景天蓝 */
            scui_menial_bar_res_t res = {0};
            res.color[0].color_s.full = 0xFFE0E0E0;
            res.color[0].color_e.full = 0xFFE0E0E0;
            res.color[1].color_s.full = 0xFF2196F3;
            res.color[1].color_e.full = 0xFF2196F3;
            
            res.part = scui_object_part_rect_bg;
            scui_menial_bar_style(handle, &res);
            res.part = scui_object_part_rect_fg;
            scui_menial_bar_style(handle, &res);
            break;
        }
        case scui_menial_type_cht: {
            /* 常规 res: 天蓝 */
            scui_menial_cht_res_t res = {0};
            res.color.color.full = 0xFF2196F3;
            
            if (data->cht.type == 0) {
                res.part = scui_object_part_rect_item;
                scui_menial_cht_style(handle, &res);
            } else {
                res.part = scui_object_part_line_item;
                scui_menial_cht_style(handle, &res);
            }
            break;
        }
        default:
            break;
        }
        
        break;
    }
    default:
        break;
    }
}
