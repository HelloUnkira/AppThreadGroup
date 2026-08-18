/*实现目标:
 *    窗口:scui_test_ui_chart
 *    用途: 测试图表(从 scui_ui_float_4 挖取)
 *    构件: xchart hist + line
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
        
        // chart hist
        scui_xchart_maker_t xchart1_maker = {0};
        scui_handle_t xchart1_handle = SCUI_HANDLE_INVALID;
        
        scui_widget_maker_def_cfg(&xchart1_maker, scui_widget_type_xchart);
        xchart1_maker.widget.style.indev_ptr = true;
        xchart1_maker.widget.style.indev_enc = true;
        xchart1_maker.widget.style.indev_key = true;
        xchart1_maker.widget.clip.x = SCUI_HOR_RES *  1 / 25;
        xchart1_maker.widget.clip.y = SCUI_VER_RES *  1 / 25;
        xchart1_maker.widget.clip.w = SCUI_HOR_RES * 11 / 25;
        xchart1_maker.widget.clip.h = SCUI_VER_RES * 11 / 25;
        xchart1_maker.widget.parent = SCUI_UI_SCENE_TEST_UI_CHART;
        xchart1_maker.widget.color.color.full = 0xFF4F4F4F;
        xchart1_maker.type = scui_xchart_type_hist;
        xchart1_maker.hist.edge = scui_image_prj_repeat_05_dotbmp;
        xchart1_maker.hist.value_min = 60;
        xchart1_maker.hist.value_max = 220;
        xchart1_maker.hist.offset.x  = 5;
        xchart1_maker.hist.offset.y  = 0;
        xchart1_maker.hist.number    = 20;
        xchart1_maker.hist.height    = xchart1_maker.widget.clip.h - 10;
        xchart1_maker.hist.space     = 4;
        
        xchart1_maker.widget.style.fully_bg = true;
        xchart1_maker.hist.color.color.full = 0xFFFF0000;
        scui_widget_create(&xchart1_maker, &xchart1_handle);
        scui_xchart_hist_data(xchart1_handle, vlist_min, vlist_max);
        
        // chart line
        scui_xchart_maker_t xchart2_maker = {0};
        scui_handle_t xchart2_handle = SCUI_HANDLE_INVALID;
        
        scui_widget_maker_def_cfg(&xchart2_maker, scui_widget_type_xchart);
        xchart2_maker.widget.style.indev_ptr = true;
        xchart2_maker.widget.style.indev_enc = true;
        xchart2_maker.widget.style.indev_key = true;
        xchart2_maker.widget.clip.x = SCUI_HOR_RES * 13 / 25;
        xchart2_maker.widget.clip.y = SCUI_VER_RES *  1 / 25;
        xchart2_maker.widget.clip.w = SCUI_HOR_RES * 11 / 25;
        xchart2_maker.widget.clip.h = SCUI_VER_RES * 11 / 25;
        xchart2_maker.widget.parent = SCUI_UI_SCENE_TEST_UI_CHART;
        xchart2_maker.widget.color.color.full = 0xFF4F4F4F;
        xchart2_maker.type = scui_xchart_type_line;
        xchart2_maker.line.edge = scui_image_prj_repeat_05_dotbmp;
        xchart2_maker.line.width     = 2;
        xchart2_maker.line.value_min = 60;
        xchart2_maker.line.value_max = 220;
        xchart2_maker.line.offset.x  = 5;
        xchart2_maker.line.offset.y  = 0;
        xchart2_maker.line.number    = 100;
        xchart2_maker.line.height    = xchart2_maker.widget.clip.h - 10;
        xchart2_maker.line.space     = 4;
        
        xchart2_maker.widget.style.fully_bg = true;
        xchart2_maker.line.color.color.full = 0xFFFF0000;
        scui_widget_create(&xchart2_maker, &xchart2_handle);
        
        for (uint8_t idx = 0; idx < xchart2_maker.line.number; idx++) {
            int32_t angle = scui_map(idx, 0, xchart2_maker.line.number, 0, 360);
            scui_coord_t base = xchart2_maker.line.value_min;
            scui_coord_t offset = xchart2_maker.line.value_max - base;
            vlist[idx] = base + scui_rand(100) * offset / 100;
        }
        scui_xchart_line_data(xchart2_handle, vlist);
        
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
