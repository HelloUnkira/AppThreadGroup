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
void scui_roller_make(void *inst, void *inst_maker, scui_handle_t *handle)
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
    
    /* 使用滚轮的默认滚动 */
    if (scroll_maker->anima_path[3] == NULL)
        scroll_maker->anima_path[3]  = scui_map_bounce;
    
    /* 使用滚轮的默认滚动速度 */
    scui_coord_t anima_speed[4] = SCUI_WIDGET_ROLLER_SPD_ANIM;
    for (scui_handle_t idx = 0; idx < 4; idx++) {
        if (scroll_maker->anima_speed[idx] == 0)
            scroll_maker->anima_speed[idx]  = anima_speed[idx];
    }
    
    /* 构造派生控件实例 */
    scui_linear_make(linear, linear_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_roller));
    
    /* 状态初始化 */
    roller->type   = roller_maker->type;
    roller->grad   = roller_maker->grad;
    roller->center = SCUI_HANDLE_INVALID;
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_roller_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_roller));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_roller_t *roller = (void *)widget;
    
    /* 析构派生控件实例 */
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
    case scui_event_create:
        SCUI_LOG_INFO("scui_event_create");
        break;
    case scui_event_destroy:
        SCUI_LOG_INFO("scui_event_destroy");
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
        
        // 列表控件是当前控件的父控件
        SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
        scui_handle_t  handle = scui_widget_parent(event->object);
        scui_widget_t *widget = scui_handle_source_check(handle);
        scui_linear_t *linear = (void *)widget;
        scui_roller_t *roller = (void *)widget;
        
        scui_linear_item_t linear_item = {.draw_idx = -1,};
        scui_linear_item_gets(widget->myself, &linear_item);
        
        scui_handle_t  custom = linear_item.handle_s;
        scui_area_t  src_clip = scui_widget_clip(custom);
        scui_image_t img_inst = {
            .type           = scui_image_type_mem,
            .format         = scui_widget_surface(custom)->format,
            .pixel.width    = src_clip.w,
            .pixel.height   = src_clip.h,
            .pixel.data_bin = scui_widget_surface(custom)->pixel,
        };
        scui_handle_t image = scui_handle_find();
        scui_handle_linker(image, &img_inst);
        
        
        
        scui_point_t offset  = {0};
        scui_multi_t percent = 100;
        // 计算当前控件中心到父控件中心距离
        scui_area_t clip_p = scui_widget_clip(handle);
        scui_area_t clip_w = scui_widget_clip(event->object);
        offset.x = scui_dist(clip_p.x + clip_p.w / 2, clip_w.x + clip_w.w / 2);
        offset.y = scui_dist(clip_p.y + clip_p.h / 2, clip_w.y + clip_w.h / 2);
        
        scui_multi_t rad_rr = clip_p.h / 2 - clip_w.h / 2;
        scui_multi_t dist_y = (1024 * scui_min(rad_rr, offset.y) / rad_rr);
        scui_multi_t cos_a2 = (1024 * 1024) - dist_y * dist_y;
        scui_multi_t cos_ia = 0, cos_fa = 0;
        scui_sqrt(cos_a2, &cos_ia, &cos_fa, 0x8000);
        scui_multi_t dist_x = (1024 - cos_ia) * (rad_rr) / 1024;
        SCUI_LOG_INFO("dist_y:%d cos_a2:%08x cos_ia:%d dist_x:%d", dist_y, cos_a2, cos_ia, dist_x);
        
        dist_x  = scui_min(dist_x, clip_p.w / 2);
        percent = (clip_p.w / 2 - dist_x) * 100 / (clip_p.w / 2);
        percent = scui_min(scui_max(percent, 10), 100);
        SCUI_LOG_INFO("<%d, %d>:%u", offset.x, offset.y, percent);
        
        scui_alpha_t alpha_w = scui_alpha_cover;
        scui_alpha_t alpha_c = scui_alpha_cover;
        scui_widget_alpha_get(widget->myself, &alpha_w);
        scui_widget_alpha_get(event->object, &alpha_c);
        // 更新alpha通道
        if (roller->grad) {
            // 更新alpha通道
            scui_alpha_t alpha = scui_alpha_pct(percent);
            scui_widget_alpha_set(widget->myself, alpha, false);
            scui_widget_alpha_set(event->object, alpha, false);
        }
        SCUI_LOG_INFO("widget %u draw sub idx: %u", widget->myself,
            scui_widget_child_to_index(widget->myself, event->object));
        
        
        
        switch (roller->type) {
        case scui_roller_type_simple: {
            scui_widget_draw_image(event->object, NULL, image, NULL, SCUI_COLOR_UNUSED);
            break;
        }
        case scui_roller_type_scale: {
            scui_point_t img_scale = {0};
            scui_opt_pos_t img_pos = scui_opt_pos_c;
            img_scale.x = 1024 * (scui_multi_t)percent / 100;
            img_scale.y = 1024 * (scui_multi_t)percent / 100;
            scui_widget_draw_image_scale(event->object, NULL, image, NULL, img_scale, img_pos);
            break;
        }
        case scui_roller_type_spin: {
            scui_coord_t c_px = clip_p.x + clip_p.w / 2;
            scui_coord_t c_py = clip_p.y + clip_p.h / 2;
            scui_coord_t c_wx = clip_w.x + clip_w.w / 2;
            scui_coord_t c_wy = clip_w.y + clip_w.h / 2;
            scui_coord3_t dist_x3 = ((scui_coord3_t)cos_ia) / 1024;
            scui_coord3_t dist_y3 = ((scui_coord3_t)dist_y) / 1024;
            scui_coord3_t dist_yf = c_wy < c_py ? +1.0f : -1.0f;
            
            scui_matrix_t x_matrix = {0};
            scui_matrix_identity(&x_matrix);
            scui_point2_t chord = {.x = dist_x3, .y = dist_yf * dist_y3,};
            scui_matrix_rotate_c(&x_matrix, &chord, 0x01);
            /* 视点在控件中心 */
            scui_view3_t view3 = {
                .x = +c_px,
                .y = +c_py,
                .z = +rad_rr * 8,
            };
            scui_point3_t offset3 = {.x = c_px, .y = c_py,};
            /* 创建面, 平行xy平面, 距离z轴一个cos距离 */
            scui_face3_t face3 = {
                .point3[0] = {-clip_w.w / 2, -clip_w.h / 2, rad_rr},
                .point3[1] = {+clip_w.w / 2, -clip_w.h / 2, rad_rr},
                .point3[2] = {+clip_w.w / 2, +clip_w.h / 2, rad_rr},
                .point3[3] = {-clip_w.w / 2, +clip_w.h / 2, rad_rr},
            };
            scui_area3_transform_by_matrix(&face3, &x_matrix);
            scui_area3_offset(&face3, &offset3);
            
            scui_matrix_t matrix = {0};
            scui_size2_t size2 = {.w = scui_image_w(image),.h = scui_image_h(image),};
            scui_matrix_perspective_view_blit(&matrix, &size2, &face3, &view3);
            scui_matrix_inverse(&matrix);
            
            // 这里暂时不分为三个步调, 都在execute执行完毕
            // 此外, 这里仅仅替父控件计算绘制的实际内容, 子控件本身不做额外绘制
            scui_widget_draw_image_matrix(widget->myself, NULL, image, NULL, &matrix);
            break;
        }
        default:
            break;
        }
        
        
        
        scui_widget_alpha_set(widget->myself, alpha_w, false);
        scui_widget_alpha_set(event->object, alpha_c, false);
        scui_handle_clear(image);
        break;
    }
    case scui_event_ptr_click: {
        scui_event_mask_over(event);
        scui_handle_t handle_p = scui_widget_parent(event->object);
        scui_handle_t index = scui_widget_child_to_index(handle_p, event->object);
        
        scui_linear_item_t linear_item = {.draw_idx = -1,};
        scui_linear_item_gets(handle_p, &linear_item);
        
        scui_handle_t custom = linear_item.handle_m;
        SCUI_LOG_INFO("click idx:%d", index);
        
        // 聚焦它到中心
        scui_scroll_center(handle_p, event->object);
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
    scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
    custom_maker.widget         = maker->widget;
    custom_maker.widget.type    = scui_widget_type_custom;
    custom_maker.widget.parent  = SCUI_HANDLE_INVALID;
    
    // 创建子控件(继承父控件部分参数)
    custom_maker.widget.style.trans     = true;
    custom_maker.widget.style.indev_ptr = true;
    custom_maker.widget.parent          = widget->myself;
    custom_maker.widget.event_cb        = scui_roller_m_event;
    scui_widget_create(&custom_maker, &custom_handle);
    scui_handle_t idx = scui_widget_child_to_index(widget->myself, custom_handle);
    
    scui_linear_item_t linear_item = {.draw_idx = idx,};
    scui_linear_item_gets(widget->myself, &linear_item);
    linear_item.handle_m = custom_handle;
    
    // 创建子控件树
    custom_maker.widget.style.trans     = false;
    custom_maker.widget.style.indev_ptr = false;
    custom_maker.widget.parent          = SCUI_HANDLE_INVALID;
    custom_maker.widget.event_cb        = scui_roller_s_event;
    custom_maker.widget.child_num       = 1;
    scui_widget_create(&custom_maker, &custom_handle);
    
    linear_item.handle_s = custom_handle;
    scui_linear_item_sets(widget->myself, &linear_item);
    
    scui_handle_t *handle_m = NULL;
    scui_custom_handle_m(linear_item.handle_s, &handle_m);
    *handle_m = linear_item.handle_m;
    
    // 使用预制的构造器构造对象
    scui_string_maker_t string_maker = *maker;
    scui_handle_t string_handle = SCUI_HANDLE_INVALID;
    string_maker.widget.parent  = custom_handle;
    scui_widget_create(&string_maker, &string_handle);
    scui_string_update_str(string_handle, str_utf8);
    
    // 添加控件重绘
    scui_widget_draw(handle, NULL, false);
}

/*@brief 滚轮控件中心对齐子控件
 *@param handle 滚轮控件句柄
 *@param target 中心对齐子控件
 */
void scui_roller_center_get(scui_handle_t handle, scui_handle_t *target)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_roller));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_roller_t *roller = (void *)widget;
    
    *target = roller->center;
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_roller_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_roller_t *roller = (void *)widget;
    
    switch (event->type) {
    case scui_event_widget_scroll_start:
    case scui_event_widget_scroll_over:
    case scui_event_widget_scroll_keep:
    case scui_event_widget_scroll_layout: {
        scui_handle_t center = NULL;
        scui_scroll_center_get(widget->myself, &center);
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
    
    scui_linear_invoke(event);
}
