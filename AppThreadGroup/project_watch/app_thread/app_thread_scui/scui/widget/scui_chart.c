/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 图表控件创建
 *@param maker  图表控件创建参数
 *@param handle 图表控件句柄
 *@param layout 通过布局创建
 */
void scui_chart_create(scui_chart_maker_t *maker, scui_handle_t *handle, bool layout)
{
    /* 创建图表控件实例 */
    scui_chart_t *chart = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_chart_t));
    memset(chart, 0, sizeof(scui_chart_t));
    
    /* 创建基础控件实例 */
    scui_widget_maker_t widget_maker = maker->widget;
    scui_widget_create(&chart->widget, &widget_maker, handle, layout);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_chart));
    SCUI_ASSERT(widget_maker.parent != SCUI_HANDLE_INVALID);
    
    chart->type = maker->type;
    
    switch (chart->type) {
    case scui_chart_type_histogram: {
        chart->histogram = maker->histogram;
        /* 这里加点断言判断参数的有效性 */
        SCUI_ASSERT(chart->histogram.number != 0);
        SCUI_ASSERT(chart->histogram.height != 0);
        SCUI_ASSERT(chart->histogram.value_min <
                    chart->histogram.value_max);
        /* 限制 */
        if (chart->histogram.space <= 0)
            chart->histogram.space  = 1;
        /* 限制 */
        scui_coord_t height = scui_widget_clip(*handle).h;
        if (chart->histogram.height > height + chart->histogram.offset.y)
            chart->histogram.height = height - chart->histogram.offset.y;
        /* 创建数据存储空间 */
        uint32_t data_size = chart->histogram.number * sizeof(scui_coord_t);
        chart->histogram_data.vlist_min = SCUI_MEM_ALLOC(scui_mem_type_mix, data_size);
        chart->histogram_data.vlist_max = SCUI_MEM_ALLOC(scui_mem_type_mix, data_size);
        for (scui_coord_t idx = 0; idx < chart->histogram.number; idx++) {
            chart->histogram_data.vlist_min[idx] = chart->histogram.value_min;
            chart->histogram_data.vlist_max[idx] = chart->histogram.value_min;
        }
        break;
    }
    case scui_chart_type_line : {
        chart->line = maker->line;
        /* 这里加点断言判断参数的有效性 */
        SCUI_ASSERT(chart->line.number != 0);
        SCUI_ASSERT(chart->line.height != 0);
        SCUI_ASSERT(chart->line.value_min <
                    chart->line.value_max);
        /* 限制 */
        if (chart->line.width <= 0)
            chart->line.width  = 1;
        if (chart->line.space <= 0)
            chart->line.space  = 1;
        /* 限制 */
        scui_coord_t height = scui_widget_clip(*handle).h;
        if (chart->line.height > height + chart->line.offset.y)
            chart->line.height = height - chart->line.offset.y;
        /* 创建数据存储空间 */
        uint32_t data_size = chart->line.number * sizeof(scui_coord_t);
        chart->line_data.vlist = SCUI_MEM_ALLOC(scui_mem_type_mix, data_size);
        for (scui_coord_t idx = 0; idx < chart->line.number; idx++)
            chart->line_data.vlist[idx] = chart->line.value_min;
        break;
    }
    default:
        SCUI_LOG_ERROR("unknown type:%u", chart->type);
        SCUI_ASSERT(false);
    }
}

/*@brief 图表控件销毁
 *@param handle 图表控件句柄
 *@param parent_way 来自父控件的销毁
 */
void scui_chart_destroy(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_chart_t  *chart  = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    switch (chart->type) {
    case scui_chart_type_histogram: {
        SCUI_MEM_FREE(chart->histogram_data.vlist_min);
        SCUI_MEM_FREE(chart->histogram_data.vlist_max);
        break;
    }
    case scui_chart_type_line: {
        SCUI_MEM_FREE(chart->line_data.vlist);
        break;
    }
    default:
        SCUI_LOG_ERROR("unknown type:%u", chart->type);
        SCUI_ASSERT(false);
    }
    
    /* 销毁基础控件实例 */
    scui_widget_destroy(&chart->widget);
    
    /* 销毁图表控件实例 */
    SCUI_MEM_FREE(chart);
}

/*@brief 图表控件数据列表更新(柱状图)
 *@param handle 图表控件句柄
 *@param vlist_min 数据列表
 *@param vlist_max 数据列表
 */
void scui_chart_histogram_data(scui_handle_t handle, scui_coord_t *vlist_min, scui_coord_t *vlist_max)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_chart));
    scui_widget_t *widget = scui_handle_get(handle);
    scui_chart_t  *chart  = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    if (chart->type != scui_chart_type_histogram) {
        SCUI_LOG_ERROR("chart type unmatch");
        return;
    }
    
    scui_coord_t value_min = chart->histogram.value_min;
    scui_coord_t value_max = chart->histogram.value_max;
    for (scui_coord_t idx = 0; idx < chart->histogram.number; idx++) {
        scui_coord_t min = scui_min(value_max, scui_max(value_min, vlist_min[idx]));
        scui_coord_t max = scui_max(value_min, scui_min(value_max, vlist_max[idx]));
        if (min > max) {
            scui_coord_t tmp = 0;
            tmp = min;
            min = max;
            max = tmp;
        }
        chart->histogram_data.vlist_min[idx] = min;
        chart->histogram_data.vlist_max[idx] = max;
    }
}

/*@brief 图表控件数据列表更新(折线图)
 *@param handle 图表控件句柄
 *@param vlist  数据列表
 */
void scui_chart_line_data(scui_handle_t handle, scui_coord_t *vlist)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_chart));
    scui_widget_t *widget = scui_handle_get(handle);
    scui_chart_t  *chart  = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    if (chart->type != scui_chart_type_line) {
        SCUI_LOG_ERROR("chart type unmatch");
        return;
    }
    
    scui_coord_t value_min = chart->line.value_min;
    scui_coord_t value_max = chart->line.value_max;
    for (scui_coord_t idx = 0; idx < chart->line.number; idx++) {
        scui_coord_t val = scui_min(value_max, scui_max(value_min, vlist[idx]));
        chart->line_data.vlist[idx] = val;
    }
}

/*@brief 图表控件事件处理回调
 *@param event 事件
 */
void scui_chart_event(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_handle_t  handle = event->object;
    scui_widget_t *widget = scui_handle_get(handle);
    scui_chart_t   *chart = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    switch (event->type) {
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        switch (chart->type) {
        case scui_chart_type_histogram: {
            scui_color_t  color     = chart->histogram.color;
            scui_coord_t  space     = chart->histogram.space;
            scui_point_t  offset    = chart->histogram.offset;
            scui_coord_t  height    = chart->histogram.height;
            scui_coord_t  value_min = chart->histogram.value_min;
            scui_coord_t  value_max = chart->histogram.value_max;
            scui_coord_t *vlist_min = chart->histogram_data.vlist_min;
            scui_coord_t *vlist_max = chart->histogram_data.vlist_max;
            scui_handle_t image     = chart->histogram.edge;
            
            scui_area_t dst_clip = {0};
            scui_area_t src_clip = {
                .w = scui_image_w(image),
                .h = scui_image_h(image) / 2,
            };
            scui_coord_t histogram_h = height - scui_image_h(image);
            scui_color_t color_edge  = color;
            color_edge.color_f.full  = 0xFF000000;
            color_edge.filter        = true;
            
            for (scui_coord_t idx = 0; idx < chart->histogram.number; idx++) {
                scui_coord_t offset_1y = scui_map(vlist_min[idx], value_min, value_max, histogram_h, 0);
                scui_coord_t offset_2y = scui_map(vlist_max[idx], value_min, value_max, histogram_h, 0);
                scui_point_t offset_1  = {.x = offset.x, .y = offset.y + offset_1y};
                scui_point_t offset_2  = {.x = offset.x, .y = offset.y + offset_2y - src_clip.h};
                
                /* 绘制edge */
                src_clip.y = 0;
                dst_clip = scui_widget_clip(handle);
                dst_clip.x += offset.x;
                dst_clip.y += offset.y + offset_2y - src_clip.h;
                scui_widget_draw_image(handle, &dst_clip, image, &src_clip, color_edge);
                
                /* 绘制edge */
                src_clip.y = src_clip.h;
                dst_clip = scui_widget_clip(handle);
                dst_clip.x += offset.x;
                dst_clip.y += offset.y + offset_1y;
                scui_widget_draw_image(handle, &dst_clip, image, &src_clip, color_edge);
                
                /* 填充这块区域 */
                dst_clip = scui_widget_clip(handle);
                scui_area_t area = {
                    .x = dst_clip.x + offset.x,
                    .w = src_clip.w,
                    .y = dst_clip.y + offset_2.y + src_clip.h,
                    .h = offset_1.y - offset_2.y - src_clip.h,
                };
                scui_widget_draw_color(handle, &area, color);
                
                offset.x += scui_image_h(image) + space;
            }
            
            break;
        }
        case scui_chart_type_line: {
            scui_color_t  color     = chart->line.color;
            scui_coord_t  space     = chart->line.space;
            scui_coord_t  width     = chart->line.width;
            scui_point_t  offset    = chart->line.offset;
            scui_coord_t  height    = chart->line.height;
            scui_coord_t  value_min = chart->line.value_min;
            scui_coord_t  value_max = chart->line.value_max;
            scui_coord_t *vlist     = chart->line_data.vlist;
            scui_handle_t image     = chart->histogram.edge;
            
            scui_area_t src_clip = {
                .w = scui_image_w(image) / 2,
                .h = scui_image_h(image) / 2,
            };
            scui_color_t color_edge  = color;
            color_edge.color_f.full  = 0xFF000000;
            color_edge.filter        = true;
            
            for (scui_coord_t idx = 0; idx + 1 < chart->line.number; idx++) {
                scui_area_t  dst_clip  = scui_widget_clip(handle);
                scui_coord_t offset_1y = scui_map(vlist[idx + 0], value_min, value_max, height, 0);
                scui_coord_t offset_2y = scui_map(vlist[idx + 1], value_min, value_max, height, 0);
                scui_point_t offset_1 = {.x = dst_clip.x + offset.x, .y = dst_clip.y + offset.y + offset_1y};
                scui_point_t offset_2 = {.x = dst_clip.x + offset.x + space, .y = dst_clip.y + offset.y + offset_2y};
                
                
                /* 绘制edge */
                if (idx == 0) {
                    dst_clip = scui_widget_clip(handle);
                    dst_clip.x += offset_1.x - src_clip.w;
                    dst_clip.y += offset_1.y - src_clip.h;
                    scui_widget_draw_image(handle, &dst_clip, image, NULL, color_edge);
                }
                
                /* 绘制edge */
                if (1) {
                    dst_clip = scui_widget_clip(handle);
                    dst_clip.x += offset_2.x - src_clip.w;
                    dst_clip.y += offset_2.y - src_clip.h;
                    scui_widget_draw_image(handle, &dst_clip, image, NULL, color_edge);
                }
                
                scui_draw_graph_dsc_t draw_graph = {
                    .type = scui_draw_graph_type_line,
                    .src_color = color,
                    .line.src_width = width,
                    .line.src_pos_1 = offset_1,
                    .line.src_pos_2 = offset_2,
                };
                scui_widget_draw_graph(handle, NULL, &draw_graph);
                offset.x += space;
            }
            
            break;
        }
        default:
            SCUI_LOG_ERROR("unknown type:%u", chart->type);
            SCUI_ASSERT(false);
        }
        
        break;
    }
    default:
        break;
    }
}
