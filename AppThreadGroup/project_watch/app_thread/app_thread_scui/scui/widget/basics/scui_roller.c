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
 *@param layout     通过布局创建
 */
void scui_roller_make(void *inst, void *inst_maker, scui_handle_t *handle, bool layout)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 继承对象 */
    scui_scroll_t *scroll = widget;
    scui_scroll_maker_t *scroll_maker = widget_maker;
    /* 继承对象 */
    scui_linear_t *linear = widget;
    scui_linear_maker_t *linear_maker = widget_maker;
    /* 本类对象 */
    scui_roller_t *roller = widget;
    scui_roller_maker_t *roller_maker = widget_maker;
    
    /* 构造派生控件实例 */
    scui_linear_make(linear, linear_maker, handle, layout);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_roller));
    
    /* 状态初始化 */
    roller->center = SCUI_HANDLE_INVALID;
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_roller_burn(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_roller_t *roller = (void *)widget;
    
    /* 析构派生控件实例 */
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_roller));
    scui_linear_burn(widget->myself);
}

/*@brief 事件处理回调
 *@param event 事件
 */
static void scui_roller_s_event(scui_event_t *event)
{
    // 特殊的固定调用
    scui_linear_s_event(event);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 事件处理回调
 *@param event 事件
 */
static void scui_roller_m_event(scui_event_t *event)
{
    // 特殊的固定调用
    scui_linear_m_event(event);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
        scui_widget_t *widget = scui_handle_source_check(event->object);
        // 列表控件是当前控件的父控件
        widget = scui_handle_source_check(widget->parent);
        scui_linear_t *linear = (void *)widget;
        
        scui_point_t offset  = {0};
        scui_multi_t percent = 100;
        scui_opt_pos_t pos = scui_opt_pos_c;
        
        #if 1
        {   // 计算当前控件中心到父控件中心距离
            scui_area_t clip_p = scui_widget_clip(scui_widget_parent(event->object));
            scui_area_t clip_w = scui_widget_clip(event->object);
            offset.x = scui_dist(clip_p.x + clip_p.w / 2, clip_w.x + clip_w.w / 2);
            offset.y = scui_dist(clip_p.y + clip_p.h / 2, clip_w.y + clip_w.h / 2);
            
            scui_multi_t rad_rr = clip_p.h / 2 - clip_w.h / 2;
            scui_multi_t dist_y = scui_min(rad_rr, offset.y);
            
            scui_multi_t cos_a2 = (1024 * 1024) - (1024 * dist_y / rad_rr) * (1024 * dist_y / rad_rr);
            scui_multi_t cos_ia = 0;
            scui_multi_t cos_fa = 0;
            scui_sqrt(cos_a2, &cos_ia, &cos_fa, 0x8000);
            scui_multi_t dist_x = (1024 - cos_ia) * (rad_rr) / 1024;
            SCUI_LOG_INFO("dist_y:%d cos_a2:%08x cos_ia:%d dist_x:%d", dist_y, cos_a2, cos_ia, dist_x);
            
            dist_x  = scui_min(dist_x, clip_p.w / 2);
            percent = (clip_p.w / 2 - dist_x) * 100 / (clip_p.w / 2);
            percent = scui_min(percent, 100);
            percent = scui_max(percent, 50);
            SCUI_LOG_INFO("<%d, %d>:%u", offset.x, offset.y, percent);
        }
        #else
        {   // 计算当前控件中心到父控件中心距离
            scui_area_t clip_p = scui_widget_clip(scui_widget_parent(event->object));
            scui_area_t clip_w = scui_widget_clip(event->object);
            offset.x = scui_dist(clip_p.x + clip_p.w / 2, clip_w.x + clip_w.w / 2);
            offset.y = scui_dist(clip_p.y + clip_p.h / 2, clip_w.y + clip_w.h / 2);
            
            scui_multi_t rad_rr = clip_p.h / 2 - clip_w.h / 2;
            scui_multi_t dist_y = scui_min(rad_rr, offset.y);
            
            percent = (clip_p.h / 2 - dist_y) * 100 / (clip_p.h / 2);
            percent = scui_min(percent, 100);
            percent = scui_max(percent, 50);
            SCUI_LOG_INFO("<%d, %d>:%u", offset.x, offset.y, percent);
        }
        #endif
        
        #if 1   // 更新alpha通道
        scui_alpha_t alpha = scui_alpha_pct(percent);
        scui_widget_alpha_set(event->object, scui_alpha_cover, true);
        scui_widget_draw_color(event->object, NULL, (scui_color_t){0});
        scui_widget_alpha_set(event->object, alpha, true);
        #endif
        
        scui_linear_item_t linear_item = {.draw_idx = -1,};
        scui_linear_item_gets(widget->myself, &linear_item);
        
        scui_handle_t  custom  = linear_item.handle_s;
        scui_area_t  src_clip  = scui_widget_clip(custom);
        scui_point_t img_scale = {
            .x = 1024 * (scui_multi_t)percent / 100,
            .y = 1024 * (scui_multi_t)percent / 100,
        };
        
        scui_image_t img_inst = {
            .type           = scui_image_type_mem,
            .format         = scui_widget_surface(custom)->format,
            .pixel.width    = src_clip.w,
            .pixel.height   = src_clip.h,
            .pixel.data_bin = scui_widget_surface(custom)->pixel,
        };
        
        scui_handle_t image = scui_handle_find();
        scui_handle_linker(image, &img_inst);
        scui_widget_draw_image_scale(event->object, NULL, image, NULL, img_scale, pos);
        scui_handle_clear(image);
        
        break;
    }
    case scui_event_ptr_click: {
        
        scui_alpha_t alpha = scui_widget_alpha_get(event->object);
        if (alpha <= scui_alpha_pct20)
            break;
        
        scui_event_mask_over(event);
        scui_handle_t parent = scui_widget_parent(event->object);
        scui_handle_t index  = scui_widget_child_to_index(parent, event->object);
        
        scui_linear_item_t linear_item = {.draw_idx = -1,};
        scui_linear_item_gets(parent, &linear_item);
        
        scui_handle_t custom = linear_item.handle_m;
        SCUI_LOG_WARN("click idx:%d", index);
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 滚轮控件设置字符串
 *@param handle   滚轮控件句柄
 *@param maker    字符控件构造器
 *@param str_utf8 字符串(utf8)
 */
void scui_roller_string_str(scui_handle_t handle, scui_string_maker_t *maker, uint8_t *str_utf8)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_roller));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_roller_t *roller = (void *)widget;
    
    // 基类对象同步
    scui_custom_maker_t custom_maker = {0};
    scui_handle_t custom_handle  = SCUI_HANDLE_INVALID;
    custom_maker.widget             = maker->widget;
    custom_maker.widget.type        = scui_widget_type_custom;
    custom_maker.widget.style.trans = false;
    custom_maker.widget.parent      = SCUI_HANDLE_INVALID;
    
    // 创建子控件
    custom_maker.widget.parent   = widget->myself;
    custom_maker.widget.event_cb = scui_roller_m_event;
    scui_widget_create(&custom_maker, &custom_handle, false);
    scui_handle_t idx = scui_widget_child_to_index(widget->myself, custom_handle);
    
    scui_linear_item_t linear_item = {.draw_idx = idx,};
    scui_linear_item_gets(widget->myself, &linear_item);
    linear_item.handle_m = custom_handle;
    
    // 创建子控件树
    custom_maker.widget.parent    = SCUI_HANDLE_INVALID;
    custom_maker.widget.event_cb  = scui_roller_s_event;
    custom_maker.widget.child_num = 1;
    scui_widget_create(&custom_maker, &custom_handle, false);
    
    linear_item.handle_s = custom_handle;
    scui_linear_item_sets(widget->myself, &linear_item);
    scui_linear_s_linker(handle, custom_handle);
    
    // 使用预制的构造器构造对象
    scui_string_maker_t string_maker = *maker;
    scui_handle_t string_handle = SCUI_HANDLE_INVALID;
    string_maker.widget.parent  = custom_handle;
    scui_widget_create(&string_maker, &string_handle, false);
    scui_string_update_str(string_handle, str_utf8);
    
    // 添加控件重绘
    scui_widget_draw(handle, NULL, false);
}

/*@brief 滚轮控件中心对齐子控件
 *@param handle 滚轮控件句柄
 *@param target 中心对齐子控件
 */
void scui_roller_center_target_get(scui_handle_t handle, scui_handle_t *target)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_roller));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_roller_t *roller = (void *)widget;
    
    *target = roller->center;
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_roller_event(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_roller_t *roller = (void *)widget;
    
    switch (event->type) {
    case scui_event_widget_scroll_s:
    case scui_event_widget_scroll_e:
    case scui_event_widget_scroll_c:
    case scui_event_widget_scroll_layout: {
        scui_handle_t center = NULL;
        scui_scroll_center_target_get(widget->myself, &center);
        for (scui_handle_t idx = 0; idx < roller->linear.list_num; idx++)
            if (center == roller->linear.list_widget_m[idx]) {
                scui_handle_t handle_s = roller->linear.list_widget_s[idx];
                roller->center = scui_widget_child_by_index(handle_s, 0);
                break;
            }
        break;
    }
    default:
        break;
    }
    
    scui_linear_event(event);
}
