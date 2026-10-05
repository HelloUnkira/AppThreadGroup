/*实现目标:
 *    测试(draw mask)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_mask_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_key_click:
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    default:
        break;
    }
}

/*@brief mask_1 控件事件响应回调(半平面直线)
 *@param event 事件
 */
void scui_test_ui_mask_1_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.叠加蒙版(多层叠加得到复杂蒙版) */
            scui_point_t pos_1 = {.x = 0,          .y = clip.h - 1,};
            scui_point_t pos_2 = {.x = clip.w - 1, .y = 0,};
            scui_image_mask_line(handle, NULL, pos_1, pos_2, 0, scui_alpha_cover);
        }
        
        /* 3.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_2 控件事件响应回调(双直线交叉楔形)
 *@param event 事件
 */
void scui_test_ui_mask_2_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.叠加蒙版(多层叠加得到复杂蒙版) */
            scui_point_t pos_1 = {.x = 0,          .y = 0,};
            scui_point_t pos_2 = {.x = clip.w - 1, .y = clip.h - 1,};
            scui_image_mask_line(handle, NULL, pos_1, pos_2, 0, scui_alpha_cover);
            scui_point_t pos_3 = {.x = 0,          .y = clip.h - 1,};
            scui_point_t pos_4 = {.x = clip.w - 1, .y = 0,};
            scui_image_mask_line(handle, NULL, pos_3, pos_4, 0, scui_alpha_cover);
        }
        
        /* 3.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_3 控件事件响应回调(四直线菱形(凸多边形))
 *@param event 事件
 */
void scui_test_ui_mask_3_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.叠加蒙版(多层叠加得到复杂蒙版) */
            scui_point_t pos_1 = {.x = 0,          .y = clip.h / 2,};
            scui_point_t pos_2 = {.x = clip.w / 2, .y = 0,};
            scui_image_mask_line(handle, NULL, pos_1, pos_2, 1, scui_alpha_cover);
            scui_point_t pos_3 = {.x = clip.w / 2,     .y = 0,};
            scui_point_t pos_4 = {.x = clip.w - 1,     .y = clip.h / 2,};
            scui_image_mask_line(handle, NULL, pos_3, pos_4, 1, scui_alpha_cover);
            scui_point_t pos_5 = {.x = clip.w - 1,     .y = clip.h / 2,};
            scui_point_t pos_6 = {.x = clip.w / 2,     .y = clip.h - 1,};
            scui_image_mask_line(handle, NULL, pos_5, pos_6, 1, scui_alpha_cover);
            scui_point_t pos_7 = {.x = clip.w / 2,     .y = clip.h - 1,};
            scui_point_t pos_8 = {.x = 0,              .y = clip.h / 2,};
            scui_image_mask_line(handle, NULL, pos_7, pos_8, 1, scui_alpha_cover);
        }
        
        /* 3.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_4 控件事件响应回调(圆(保留圆内))
 *@param event 事件
 */
void scui_test_ui_mask_4_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.叠加蒙版(多层叠加得到复杂蒙版) */
            scui_point_t center = {.x = clip.w / 2, .y = clip.h / 2,};
            scui_image_mask_radius(handle, NULL, center, clip.w / 2 - 2, 0, scui_alpha_cover);
        }
        
        /* 3.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_5 控件事件响应回调(圆(保留圆外))
 *@param event 事件
 */
void scui_test_ui_mask_5_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.叠加蒙版(多层叠加得到复杂蒙版) */
            scui_point_t center = {.x = clip.w / 2, .y = clip.h / 2,};
            scui_image_mask_radius(handle, NULL, center, clip.w / 2 - 6, 1, scui_alpha_cover);
        }
        
        /* 3.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_6 控件事件响应回调(圆环(外圆内交内圆外))
 *@param event 事件
 */
void scui_test_ui_mask_6_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.叠加蒙版(多层叠加得到复杂蒙版) */
            scui_point_t center = {.x = clip.w / 2, .y = clip.h / 2,};
            scui_image_mask_radius(handle, NULL, center, clip.w / 2 - 2, 0, scui_alpha_cover);
            scui_image_mask_radius(handle, NULL, center, clip.w / 4, 1, scui_alpha_cover);
        }
        
        /* 3.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_7 控件事件响应回调(圆角矩形(保留内部))
 *@param event 事件
 */
void scui_test_ui_mask_7_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.叠加蒙版(多层叠加得到复杂蒙版) */
            scui_point_t center  = {.x = clip.w / 2,       .y = clip.h / 2,};
            scui_point_t extents = {.x = clip.w / 2 - 2,   .y = clip.h / 2 - 2,};
            scui_image_mask_rect(handle, NULL, center, extents, 14, 0, scui_alpha_cover);
        }
        
        /* 3.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_8 控件事件响应回调(圆角矩形(保留外部))
 *@param event 事件
 */
void scui_test_ui_mask_8_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.叠加蒙版(多层叠加得到复杂蒙版) */
            scui_point_t center  = {.x = clip.w / 2,       .y = clip.h / 2,};
            scui_point_t extents = {.x = clip.w / 2 - 6,   .y = clip.h / 2 - 6,};
            scui_image_mask_rect(handle, NULL, center, extents, 14, 1, scui_alpha_cover);
        }
        
        /* 3.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_9 控件事件响应回调(扇形0至90)
 *@param event 事件
 */
void scui_test_ui_mask_9_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.叠加蒙版(多层叠加得到复杂蒙版) */
            scui_point_t vertex = {.x = clip.w / 2, .y = clip.h / 2,};
            scui_image_mask_angle(handle, NULL, vertex, 0, 90, scui_alpha_cover);
        }
        
        /* 3.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_10 控件事件响应回调(优角扇形90至270)
 *@param event 事件
 */
void scui_test_ui_mask_10_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.叠加蒙版(多层叠加得到复杂蒙版) */
            scui_point_t vertex = {.x = clip.w / 2, .y = clip.h / 2,};
            scui_image_mask_angle(handle, NULL, vertex, 90, 270, scui_alpha_cover);
        }
        
        /* 3.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_11 控件事件响应回调(渐变(上透下实))
 *@param event 事件
 */
void scui_test_ui_mask_11_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.叠加蒙版(多层叠加得到复杂蒙版) */
            scui_image_mask_fade(handle, NULL, 0, clip.h - 1, 0, scui_alpha_cover, scui_alpha_cover);
        }
        
        /* 3.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_12 控件事件响应回调(渐变(上实下透)交圆)
 *@param event 事件
 */
void scui_test_ui_mask_12_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.叠加蒙版(多层叠加得到复杂蒙版) */
            scui_image_mask_fade(handle, NULL, 0, clip.h - 1, scui_alpha_cover, 0, scui_alpha_cover);
            scui_point_t center = {.x = clip.w / 2, .y = clip.h / 2,};
            scui_image_mask_radius(handle, NULL, center, clip.w / 2 - 2, 0, scui_alpha_cover);
        }
        
        /* 3.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_13 控件事件响应回调(蒙版相乘(圆环交半平面))
 *@param event 事件
 */
void scui_test_ui_mask_13_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    static scui_handle_t map_handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            scui_image_mask_burn(map_handle);
            map_handle = SCUI_HANDLE_INVALID;
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.蒙版A: 圆环 */
            scui_image_mask_make(&handle, &clip);
            scui_point_t center = {.x = clip.w / 2, .y = clip.h / 2,};
            scui_image_mask_radius(handle, NULL, center, clip.w / 2 - 2, 0, scui_alpha_cover);
            scui_image_mask_radius(handle, NULL, center, clip.w / 4, 1, scui_alpha_cover);
            
            /* 2.蒙版B: 半平面(直线一侧, 保留侧随直线方向而定) */
            scui_image_mask_make(&map_handle, &clip);
            scui_point_t pos_1 = {.x = clip.w / 2, .y = 0,};
            scui_point_t pos_2 = {.x = clip.w / 2, .y = clip.h - 1,};
            scui_image_mask_line(map_handle, NULL, pos_1, pos_2, 0, scui_alpha_cover);
            
            /* 3.蒙版相乘: 先将A镜像出可写副本, 再将B就地作用其上(alpha8×alpha8字节相乘) */
            scui_handle_t handle_mask = SCUI_HANDLE_INVALID;
            scui_image_mirror(handle, &handle_mask);
            scui_image_mask_apply(handle_mask, map_handle);
            scui_image_mask_burn(handle);
            scui_image_mask_burn(map_handle);
            map_handle = SCUI_HANDLE_INVALID;
            handle = handle_mask;
        }
        
        /* 4.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_14 控件事件响应回调(双圆透镜交渐变)
 *@param event 事件
 */
void scui_test_ui_mask_14_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.叠加蒙版(多层叠加得到复杂蒙版) */
            scui_point_t center_1 = {.x = clip.w / 2 - 10, .y = clip.h / 2,};
            scui_image_mask_radius(handle, NULL, center_1, clip.w / 2 - 10, 0, scui_alpha_cover);
            scui_point_t center_2 = {.x = clip.w / 2 + 10, .y = clip.h / 2,};
            scui_image_mask_radius(handle, NULL, center_2, clip.w / 2 - 10, 0, scui_alpha_cover);
            scui_image_mask_fade(handle, NULL, 0, clip.h - 1, 40, scui_alpha_cover, scui_alpha_cover);
        }
        
        /* 3.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_15 控件事件响应回调(球面渐变(双渐变交圆内圆128))
 *@param event 事件
 */
void scui_test_ui_mask_15_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.叠加蒙版(多层叠加得到复杂蒙版) */
            scui_image_mask_fade(handle, NULL, 0, clip.h / 2, 0, scui_alpha_cover, scui_alpha_cover);
            scui_image_mask_fade(handle, NULL, clip.h / 2, clip.h - 1, scui_alpha_cover, 0, scui_alpha_cover);
            scui_point_t center = {.x = clip.w / 2, .y = clip.h / 2,};
            scui_image_mask_radius(handle, NULL, center, clip.w / 2 - 2, 0, scui_alpha_cover);
            scui_image_mask_radius(handle, NULL, center, clip.w / 5, 0, 128);
        }
        
        /* 3.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_16 控件事件响应回调(四层复合(圆交扇形交渐变交圆角矩形))
 *@param event 事件
 */
void scui_test_ui_mask_16_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.叠加蒙版(多层叠加得到复杂蒙版; 保持与去map前一致的乘法顺序) */
            scui_point_t center  = {.x = clip.w / 2,     .y = clip.h / 2,};
            scui_point_t extents = {.x = clip.w / 5,     .y = clip.h / 2 - 2,};
            scui_image_mask_fade(handle, NULL, 0, clip.h - 1, 40, scui_alpha_cover, scui_alpha_cover);
            scui_point_t vertex = {.x = clip.w / 2, .y = clip.h / 2,};
            scui_image_mask_angle(handle, NULL, vertex, 200, 340, scui_alpha_cover);
            scui_image_mask_radius(handle, NULL, center, clip.w / 2 - 4, 0, scui_alpha_cover);
            scui_image_mask_rect(handle, NULL, center, extents, 8, 0, scui_alpha_cover);
        }
        
        /* 3.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_17 控件事件响应回调(图像蒙版应用(mood表情交圆交外圆交渐变))
 *@param event 事件
 */
void scui_test_ui_mask_17_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.构建复杂蒙版(圆内交圆外交渐变, 作用尺寸=图片尺寸) */
            scui_area_t area = {.w = 98, .h = 98,};
            scui_handle_t mask = SCUI_HANDLE_INVALID;
            scui_image_mask_make(&mask, &area);
            scui_point_t center_1 = {.x = 49, .y = 49,};
            scui_image_mask_radius(mask, NULL, center_1, 46, 0, scui_alpha_cover);
            scui_point_t center_2 = {.x = 72, .y = 28,};
            scui_image_mask_radius(mask, NULL, center_2, 22, 1, scui_alpha_cover);
            scui_image_mask_fade(mask, NULL, 20, 90, 32, scui_alpha_cover, scui_alpha_cover);
            
            /* 2.将图片镜像出可写内存副本(原图不被修改), 再就地作用蒙版 */
            scui_image_mirror(scui_image_prj_mood_01_retry, &handle);
            scui_image_mask_apply(handle, mask);
            scui_image_mask_burn(mask);
        }
        
        /* 3.绘制被蒙版后的图片 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_18 控件事件响应回调(原生多边形(凸六边形, 单次调用))
 *@param event 事件
 */
void scui_test_ui_mask_18_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.单次调用绘制凸六边形(上下两条水平边, 其余四条斜边) */
            scui_point_t points[6] = {
                {.x = clip.w / 4,     .y = 2,},
                {.x = clip.w * 3 / 4, .y = 2,},
                {.x = clip.w - 3,     .y = clip.h / 2,},
                {.x = clip.w * 3 / 4, .y = clip.h - 3,},
                {.x = clip.w / 4,     .y = clip.h - 3,},
                {.x = 3,              .y = clip.h / 2,},
            };
            scui_image_mask_polygon(handle, NULL, points, 6, 0, scui_alpha_cover);
        }
        
        /* 3.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief mask_19 控件事件响应回调(原生多边形(保留外部)交圆环)
 *@param event 事件
 */
void scui_test_ui_mask_19_event_proc(scui_event_t *event)
{
    static scui_handle_t handle = SCUI_HANDLE_INVALID;
    
    switch (event->type) {
    case scui_event_destroy:
        if (handle != SCUI_HANDLE_INVALID) {
            scui_image_mask_burn(handle);
            handle = SCUI_HANDLE_INVALID;
        }
        break;
    case scui_event_draw_graph: {
        scui_area_t clip = {
            .x = 5,
            .y = 5,
            .w = scui_widget_area(event->object).w - 5 * 2,
            .h = scui_widget_area(event->object).h - 5 * 2,
        };
        
        if (handle == SCUI_HANDLE_INVALID) {
            /* 1.创建alpha格式内存图(只拿句柄) */
            scui_image_mask_make(&handle, &clip);
            
            /* 2.凸六边形保留外部(单次调用取补) */
            scui_point_t points[6] = {
                {.x = clip.w / 4,     .y = 2,},
                {.x = clip.w * 3 / 4, .y = 2,},
                {.x = clip.w - 3,     .y = clip.h / 2,},
                {.x = clip.w * 3 / 4, .y = clip.h - 3,},
                {.x = clip.w / 4,     .y = clip.h - 3,},
                {.x = 3,              .y = clip.h / 2,},
            };
            scui_image_mask_polygon(handle, NULL, points, 6, 1, scui_alpha_cover);
            
            /* 3.交圆环(外圆内交内圆外) */
            scui_point_t center = {.x = clip.w / 2, .y = clip.h / 2,};
            scui_image_mask_radius(handle, NULL, center, clip.w / 2 - 2, 0, scui_alpha_cover);
            scui_image_mask_radius(handle, NULL, center, clip.w / 4, 1, scui_alpha_cover);
        }
        
        /* 4.将蒙版图可视化 */
        scui_widget_draw_image(event->object, &clip, handle, NULL, SCUI_COLOR_WHITE);
        break;
    }
    default:
        break;
    }
}
