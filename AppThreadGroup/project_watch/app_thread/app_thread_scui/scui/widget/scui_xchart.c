/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_xchart_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_xchart_t *xchart = widget;
    scui_xchart_maker_t *xchart_maker = widget_maker;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_xchart));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    xchart->type = xchart_maker->type;
    
    switch (xchart->type) {
    case scui_xchart_type_hist: {
        xchart->hist = xchart_maker->hist;
        /* 这里加点断言判断参数的有效性 */
        SCUI_ASSERT(xchart->hist.number != 0);
        SCUI_ASSERT(xchart->hist.height != 0);
        SCUI_ASSERT(xchart->hist.value_min <
                    xchart->hist.value_max);
        /* 限制 */
        if (xchart->hist.space <= 0)
            xchart->hist.space  = 1;
        /* 限制 */
        scui_coord_t height = xchart->widget.clip.h;
        if (xchart->hist.height > height + xchart->hist.offset.y)
            xchart->hist.height = height - xchart->hist.offset.y;
        /* 创建数据存储空间 */
        uint32_t data_size = xchart->hist.number * sizeof(scui_coord_t);
        xchart->hist_data.vlist_min = SCUI_MEM_ALLOC(scui_mem_type_mix, data_size);
        xchart->hist_data.vlist_max = SCUI_MEM_ALLOC(scui_mem_type_mix, data_size);
        for (scui_coord_t idx = 0; idx < xchart->hist.number; idx++) {
            xchart->hist_data.vlist_min[idx] = xchart->hist.value_min;
            xchart->hist_data.vlist_max[idx] = xchart->hist.value_min;
        }
        break;
    }
    case scui_xchart_type_line : {
        xchart->line = xchart_maker->line;
        /* 这里加点断言判断参数的有效性 */
        SCUI_ASSERT(xchart->line.number != 0);
        SCUI_ASSERT(xchart->line.height != 0);
        SCUI_ASSERT(xchart->line.value_min <
                    xchart->line.value_max);
        /* 限制 */
        if (xchart->line.width <= 0)
            xchart->line.width  = 1;
        if (xchart->line.space <= 0)
            xchart->line.space  = 1;
        /* 限制 */
        scui_coord_t height = xchart->widget.clip.h;
        if (xchart->line.height > height + xchart->line.offset.y)
            xchart->line.height = height - xchart->line.offset.y;
        /* 创建数据存储空间 */
        uint32_t data_size = xchart->line.number * sizeof(scui_coord_t);
        xchart->line_data.vlist = SCUI_MEM_ALLOC(scui_mem_type_mix, data_size);
        for (scui_coord_t idx = 0; idx < xchart->line.number; idx++)
            xchart->line_data.vlist[idx] = xchart->line.value_min;
        break;
    }
    default:
        SCUI_LOG_ERROR("unknown type:%u", xchart->type);
        SCUI_ASSERT(false);
    }
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_xchart_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_xchart));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_xchart_t *xchart = (void *)widget;
    
    switch (xchart->type) {
    case scui_xchart_type_hist: {
        SCUI_MEM_FREE(xchart->hist_data.vlist_min);
        SCUI_MEM_FREE(xchart->hist_data.vlist_max);
        break;
    }
    case scui_xchart_type_line: {
        SCUI_MEM_FREE(xchart->line_data.vlist);
        break;
    }
    default:
        SCUI_LOG_ERROR("unknown type:%u", xchart->type);
        SCUI_ASSERT(false);
    }
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 图表控件数据列表更新(柱状图)
 *@param handle 图表控件句柄
 *@param vlist_min 数据列表
 *@param vlist_max 数据列表
 */
void scui_xchart_hist_data(scui_handle_t handle, scui_coord_t *vlist_min, scui_coord_t *vlist_max)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_xchart));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_xchart_t *xchart = (void *)widget;
    
    if (xchart->type != scui_xchart_type_hist) {
        SCUI_LOG_ERROR("xchart type unmatch");
        return;
    }
    
    scui_coord_t value_min = xchart->hist.value_min;
    scui_coord_t value_max = xchart->hist.value_max;
    for (scui_coord_t idx = 0; idx < xchart->hist.number; idx++) {
        scui_coord_t min = scui_min(value_max, scui_max(value_min, vlist_min[idx]));
        scui_coord_t max = scui_max(value_min, scui_min(value_max, vlist_max[idx]));
        if (min > max) {
            scui_coord_t tmp = 0;
            tmp = min;
            min = max;
            max = tmp;
        }
        xchart->hist_data.vlist_min[idx] = min;
        xchart->hist_data.vlist_max[idx] = max;
    }
}

/*@brief 图表控件数据列表更新(折线图)
 *@param handle 图表控件句柄
 *@param vlist  数据列表
 */
void scui_xchart_line_data(scui_handle_t handle, scui_coord_t *vlist)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_xchart));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_xchart_t *xchart = (void *)widget;
    
    if (xchart->type != scui_xchart_type_line) {
        SCUI_LOG_ERROR("xchart type unmatch");
        return;
    }
    
    scui_coord_t value_min = xchart->line.value_min;
    scui_coord_t value_max = xchart->line.value_max;
    for (scui_coord_t idx = 0; idx < xchart->line.number; idx++) {
        scui_coord_t val = scui_min(value_max, scui_max(value_min, vlist[idx]));
        xchart->line_data.vlist[idx] = val;
    }
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_xchart_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_xchart_t *xchart = (void *)widget;
    
    switch (event->type) {
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        switch (xchart->type) {
        case scui_xchart_type_hist: {
            scui_color_t  color     = xchart->hist.color;
            scui_coord_t  space     = xchart->hist.space;
            scui_point_t  offset    = xchart->hist.offset;
            scui_coord_t  height    = xchart->hist.height;
            scui_coord_t  value_min = xchart->hist.value_min;
            scui_coord_t  value_max = xchart->hist.value_max;
            scui_coord_t *vlist_min = xchart->hist_data.vlist_min;
            scui_coord_t *vlist_max = xchart->hist_data.vlist_max;
            scui_handle_t image     = xchart->hist.edge;
            
            scui_area_t dst_clip = {0};
            scui_area_t src_clip = {
                .w = scui_image_w(image),
                .h = scui_image_h(image) / 2,
            };
            scui_color_t color_edge  = color;
            color_edge.color_s.full  = color.color.full;
            color_edge.color_e.full  = color.color.full;
            color_edge.color_f.full  = 0xFF000000;
            color_edge.filter        = true;
            
            for (scui_coord_t idx = 0; idx < xchart->hist.number; idx++) {
                scui_coord_t offset_1y = scui_map(vlist_min[idx], value_min, value_max, height, 0);
                scui_coord_t offset_2y = scui_map(vlist_max[idx], value_min, value_max, height, 0);
                
                // 值为0, 不进行绘制
                if (offset_1y - offset_2y < src_clip.h * 2) {
                    offset.x += scui_image_w(image) + space;
                    continue;
                }
                
                /* 绘制edge */
                src_clip.y = 0;
                dst_clip = xchart->widget.clip;
                dst_clip.x += offset.x;
                dst_clip.y += offset.y + offset_2y;
                scui_widget_draw_image(widget->myself, &dst_clip, image, &src_clip, color_edge);
                
                /* 绘制edge */
                src_clip.y = src_clip.h;
                dst_clip = xchart->widget.clip;
                dst_clip.x += offset.x;
                dst_clip.y += offset.y + offset_1y - src_clip.h;
                scui_widget_draw_image(widget->myself, &dst_clip, image, &src_clip, color_edge);
                
                /* 填充这块区域 */
                dst_clip = xchart->widget.clip;
                scui_area_t area = {
                    .x = dst_clip.x + offset.x,
                    .w = src_clip.w,
                    .y = dst_clip.y + offset.y + offset_2y + src_clip.h,
                    .h = offset_1y - offset_2y - src_clip.h * 2,
                };
                scui_widget_draw_color(widget->myself, &area, color);
                
                offset.x += scui_image_w(image) + space;
            }
            
            break;
        }
        case scui_xchart_type_line: {
            scui_color_t  color     = xchart->line.color;
            scui_coord_t  space     = xchart->line.space;
            scui_coord_t  width     = xchart->line.width;
            scui_point_t  offset    = xchart->line.offset;
            scui_coord_t  height    = xchart->line.height;
            scui_coord_t  value_min = xchart->line.value_min;
            scui_coord_t  value_max = xchart->line.value_max;
            scui_coord_t *vlist     = xchart->line_data.vlist;
            scui_handle_t image     = xchart->hist.edge;
            
            scui_area_t src_clip = {
                .w = scui_image_w(image) / 2,
                .h = scui_image_h(image) / 2,
            };
            scui_color_t color_edge  = color;
            color_edge.color_f.full  = 0xFF000000;
            color_edge.filter        = true;
            
            for (scui_coord_t idx = 0; idx + 1 < xchart->line.number; idx++) {
                scui_area_t  dst_clip  = xchart->widget.clip;
                scui_coord_t offset_1y = scui_map(vlist[idx + 0], value_min, value_max, height, 0);
                scui_coord_t offset_2y = scui_map(vlist[idx + 1], value_min, value_max, height, 0);
                scui_point_t offset_1 = {.x = dst_clip.x + offset.x, .y = dst_clip.y + offset.y + offset_1y};
                scui_point_t offset_2 = {.x = dst_clip.x + offset.x + space, .y = dst_clip.y + offset.y + offset_2y};
                
                
                /* 绘制edge */
                if (idx == 0) {
                    dst_clip = xchart->widget.clip;
                    dst_clip.x += offset_1.x - src_clip.w;
                    dst_clip.y += offset_1.y - src_clip.h;
                    scui_widget_draw_image(widget->myself, &dst_clip, image, NULL, color_edge);
                }
                
                /* 绘制edge */
                if (1) {
                    dst_clip = xchart->widget.clip;
                    dst_clip.x += offset_2.x - src_clip.w;
                    dst_clip.y += offset_2.y - src_clip.h;
                    scui_widget_draw_image(widget->myself, &dst_clip, image, NULL, color_edge);
                }
                
                scui_draw_dsc_t draw_graph = {
                    .type = scui_draw_type_pixel_line,
                    .src_color = color,
                    .src_width = width,
                    .src_pos_1 = offset_1,
                    .src_pos_2 = offset_2,
                };
                scui_widget_draw_graph(widget->myself, NULL, &draw_graph);
                offset.x += space;
            }
            
            break;
        }
        default:
            SCUI_LOG_ERROR("unknown type:%u", xchart->type);
            SCUI_ASSERT(false);
        }
        
        break;
    }
    default:
        break;
    }
}
