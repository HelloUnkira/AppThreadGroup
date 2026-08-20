/*实现目标:
 *    窗口:scui_test_ui_chart
 *    用途: 测试图表(从 scui_ui_float_4 挖取, 驱逐 xchart 后改为 menial cht)
 *    构件: menial cht hist + line
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_chart_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        
        scui_coord_t vlist[100] = {0};
        scui_coord_t vlist_min[100] = {0};
        scui_coord_t vlist_max[100] = {0};
        for (uint32_t idx = 0; idx < 100; idx++) {
            vlist_min[idx] =  60 + (uint32_t)scui_rand(0xFF) % 40;   //统一底点就是柱状图,否则为股价图
            vlist_max[idx] = 220 - (uint32_t)scui_rand(0xFF) % 40;
            vlist[idx] = 60 + (uint32_t)scui_rand(0xFF) % ((220 - 60));
        }
        
        scui_menial_maker_define(menial_maker);
        scui_handle_t       menial_handle = SCUI_HANDLE_INVALID;
        
        menial_maker.widget.parent = event->object;
        
        #if 1
        /* chart hist (柱状图) */
        menial_maker.widget.style.fully_bg = true;
        menial_maker.widget.color.color.full = 0xFF4F4F4F;
        menial_maker.widget.clip.x = SCUI_HOR_RES *  1 / 25;
        menial_maker.widget.clip.y = SCUI_VER_RES *  1 / 25;
        menial_maker.widget.clip.w = SCUI_HOR_RES * 11 / 25;
        menial_maker.widget.clip.h = SCUI_VER_RES * 11 / 25;
        menial_maker.type = scui_menial_type_cht;
        menial_maker.data.cht.area.x = 5;
        menial_maker.data.cht.area.y = 0;
        menial_maker.data.cht.area.w = menial_maker.widget.clip.w - 10;
        menial_maker.data.cht.area.h = menial_maker.widget.clip.h - 10;
        menial_maker.data.cht.value_min = 60;
        menial_maker.data.cht.value_max = 220;
        menial_maker.data.cht.type   = 0;
        menial_maker.data.cht.number = 20;
        menial_maker.data.cht.space  = 4;
        scui_widget_create(&menial_maker, &menial_handle);
        
        scui_menial_cht_res_t cht_res = {0};
        cht_res.round  = true;
        cht_res.width  = 6;
        cht_res.color.color.full = 0xFFFF0000;
        cht_res.part = scui_object_part_rect_item;
        scui_menial_cht_style(menial_handle, &cht_res);
        scui_menial_cht_hist_data(menial_handle, vlist_min, vlist_max);
        #endif
        
        #if 1
        /* chart line (折线图) */
        menial_maker.widget.clip.x = SCUI_HOR_RES * 13 / 25;
        menial_maker.widget.clip.y = SCUI_VER_RES *  1 / 25;
        menial_maker.data.cht.type   = 1;
        menial_maker.data.cht.number = 100;
        menial_maker.data.cht.space  = 4;
        scui_widget_create(&menial_maker, &menial_handle);
        
        cht_res.width  = 2;
        cht_res.part = scui_object_part_line_item;
        scui_menial_cht_style(menial_handle, &cht_res);
        
        for (uint8_t idx = 0; idx < 100; idx++) {
            scui_coord_t base = 60;
            scui_coord_t offset = 220 - base;
            vlist[idx] = base + scui_rand(100) * offset / 100;
        }
        scui_menial_cht_line_data(menial_handle, vlist);
        #endif
        
        break;
    }
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        break;
    case scui_event_focus_lost:
        break;
    default:
        break;
    }
}
