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
void scui_custom_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_custom_t *custom = widget;
    scui_custom_maker_t *custom_maker = widget_maker;
    
    /* 必须标记anima事件 */
    SCUI_ASSERT(custom_maker->type < scui_custom_type_num);
    if (custom_maker->type != scui_custom_type_none)
        widget_maker->style.sched_anima = true;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_custom));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    /* 资源构造与同步 */
    custom->type = custom_maker->type;
    if (custom->type != scui_custom_type_none) {
        custom->data  = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_custom_data_t));
        memcpy(custom->data, &custom_maker->data, sizeof(scui_custom_data_t));
    }
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_custom_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_custom));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_custom_t *custom = (void *)widget;
    
    /* 回收可能因为文本绘制而存留在控件内的资源 */
    scui_custom_text_recycle(handle);
    
    /* 回收可能存在的自定义参数集实例 */
    SCUI_MEM_FREE(custom->data);
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_custom_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_custom_t *custom = (void *)widget;
    
    /* 其他事件重载实现 */
    switch (event->type) {
    case scui_event_draw: {
        if (scui_event_check_execute(event)) {
            /* 简单打个表整理一下:不是ctx */
            static const void (*scui_custom_draw_cb[scui_custom_type_num])
            (scui_handle_t handle, scui_area_t *clip, scui_custom_data_t *data) = {
                [scui_custom_type_slider]       = scui_custom_draw_slider,
                [scui_custom_type_spinner]      = scui_custom_draw_spinner,
                [scui_custom_type_indicator]    = scui_custom_draw_indicator,
                [scui_custom_type_ring_edge]    = scui_custom_draw_ring_edge,
                [scui_custom_type_image_text]   = scui_custom_draw_image_text,
                [scui_custom_type_image_crect4] = scui_custom_draw_image_crect4,
            };
            
            if (custom->type > scui_custom_type_none && custom->type < scui_custom_type_num)
                scui_custom_draw_cb[custom->type](widget->myself, &widget->clip, custom->data);
        }
        if (scui_event_check_prepare(event)) {
            /* 回收可能因为文本绘制而存留在控件内的资源 */
            scui_custom_text_recycle(widget->myself);
        }
        if (scui_event_check_finish(event)) {
            /* 回收可能因为文本绘制而存留在控件内的资源 */
            scui_custom_text_recycle(widget->myself);
        }
        break;
    }
    case scui_event_size_auto:
    case scui_event_size_adjust:
    case scui_event_lang_change:
        /* 回收可能因为文本绘制而存留在控件内的资源 */
        scui_custom_text_recycle(widget->myself);
        scui_widget_draw(widget->myself, NULL, false);
        break;
    default:
        break;
    }
}

/*@brief 自定义控件文本资源回收
 *@param handle 控件句柄
 */
void scui_custom_text_recycle(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_custom));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_custom_t *custom = (void *)widget;
    
    /* 回收可能因为文本绘制而存留在控件内的资源 */
    if (custom->str_args != NULL) {
        
        for (scui_coord_t idx = 0; idx < SCUI_CUSTOM_STR_ITEM_LIMIT; idx++) {
            scui_string_args_t *str_args = custom->str_args[idx];
            if (str_args == NULL)
                continue;
            
            /* 在回收实例之前回收布局 */
            str_args->update = true;
            str_args->utf8   = NULL;
            str_args->name   = SCUI_HANDLE_INVALID;
            scui_string_args_process(str_args);
            
            SCUI_MEM_FREE(custom->str_args[idx]);
            SCUI_MEM_FREE(custom->str_utf8[idx]);
        }
        SCUI_MEM_FREE(custom->str_args);
        SCUI_MEM_FREE(custom->str_utf8);
    }
    custom->str_args = NULL;
    custom->str_utf8 = NULL;
}

/*@brief 自定义控件获得自定义参数集实例
 *@param handle 控件句柄
 *@param data   自定义参数集实例
 */
void scui_custom_data_inst(scui_handle_t handle, scui_custom_data_t **data)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_custom));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_custom_t *custom = (void *)widget;
    
    *data = custom->data;
}

/*@brief 自定义控件文本绘制(string控件更合适)
 *       备注:偷懒用的,不建议使用,性能低下
 *@param handle 控件句柄
 *@param args   绘制参数(scui_string_args_t)
 *@param text   文本句柄
 */
void scui_custom_draw_text(scui_handle_t handle, void *args, scui_handle_t text)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_custom));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_custom_t *custom = (void *)widget;
    
    SCUI_ASSERT(args != NULL);
    /* 在custom申请全局实例空间 */
    /* 以扩充其绘制资源的生命周期 */
    if (custom->str_args == NULL) {
        scui_multi_t str_args_size = sizeof(void *) * SCUI_CUSTOM_STR_ITEM_LIMIT;
        custom->str_args = SCUI_MEM_ZALLOC(scui_mem_type_mix, str_args_size);
    }
    if (custom->str_utf8 == NULL) {
        scui_multi_t str_utf8_size = sizeof(void *) * SCUI_CUSTOM_STR_ITEM_LIMIT;
        custom->str_utf8 = SCUI_MEM_ZALLOC(scui_mem_type_mix, str_utf8_size);
    }
    
    /* 找到一个空闲的位置, 存放本地资源 */
    for (scui_coord_t idx = 0; idx < SCUI_CUSTOM_STR_ITEM_LIMIT; idx++) {
        scui_string_args_t *str_args = custom->str_args[idx];
        if (str_args != NULL)
            continue;
        
        str_args = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_string_args_t));
        memcpy(str_args, args, sizeof(scui_string_args_t));
        custom->str_args[idx] = str_args;
        
        if (text != SCUI_HANDLE_INVALID) {
            str_args->utf8 = scui_multi_lang_str(text, str_args->lang);
        } else {
            scui_coord_t len_utf8 = strlen(str_args->utf8) + 1;
            void *str_utf8 = SCUI_MEM_ALLOC(scui_mem_type_mix, len_utf8);
            strcpy(str_utf8, str_args->utf8);
            custom->str_utf8[idx] = str_utf8;
            str_args->utf8 = str_utf8;
        }
        
        /* 绘制剪切域同步 */
        if (scui_area_empty(&str_args->clip))
            str_args->clip = widget->clip;
        
        /* 绘制时本地布局 */
        str_args->update = true;
        scui_string_args_process(str_args);
        scui_widget_draw_string(handle, &str_args->clip, str_args);
        return;
    }
    
    SCUI_LOG_ERROR("str item limit too small");
}

/*@brief 自定义控件:插件:进度条,滚动条
 *@param handle 自定义控件句柄
 *@param clip   绘制区域
 *@param data   自定义参数集
 */
void scui_custom_draw_slider(scui_handle_t handle, scui_area_t *clip, scui_custom_data_t *data)
{
    /* draw data<s> */
    scui_handle_t bar           = data->slider.bar;
    scui_handle_t edge          = data->slider.edge;
    scui_color_t  color_bar     = data->slider.color_bar;
    scui_color_t  color_edge    = data->slider.color_edge;
    scui_coord_t  vmin          = data->slider.vmin;
    scui_coord_t  vmax          = data->slider.vmax;
    scui_coord_t  cmin          = data->slider.cmin;
    scui_coord_t  cmax          = data->slider.cmax;
    scui_handle_t dist          = data->slider.dist;
    bool          way           = data->slider.way;
    /* draw data<e> */
    SCUI_LOG_DEBUG("");
    SCUI_ASSERT(clip != NULL);
    
    /* 绘制背景进度条 */
    scui_widget_draw_image(handle, clip, bar, NULL, color_bar);
    
    cmin = scui_min(vmax, scui_max(vmin, cmin));
    cmax = scui_max(vmin, scui_min(vmax, cmax));
    if (cmin > cmax) {
        scui_coord_t temp = 0;
        temp = cmin;
        cmin = cmax;
        cmax = temp;
    }
    
    scui_area_t  dst_area = {0};
    scui_area_t  dst_clip = *clip;
    scui_area_t  src_clip = {
        .w = scui_image_w(edge),
        .h = scui_image_h(edge),
    };
    
    if (way) {
        scui_coord_t offset_d1 = scui_map(cmin, vmin, vmax, dist - src_clip.w, 0);
        scui_coord_t offset_d2 = scui_map(cmax, vmin, vmax, dist - src_clip.w, 0);
        scui_point_t offset_1 = {.x = offset_d1};
        scui_point_t offset_2 = {.x = offset_d2};
        
        src_clip.w /= 2;
        /* 绘制俩个edge */
        src_clip.x = 0;
        src_clip.y = 0;
        dst_area = scui_widget_clip(handle);
        if (scui_area_inter(&dst_clip, &dst_area, clip))
        if (scui_area_limit_offset(&dst_clip, &offset_2))
            scui_widget_draw_image(handle, &dst_clip, edge, &src_clip, color_edge);
        
        src_clip.x = src_clip.w;
        src_clip.y = 0;
        dst_area = scui_widget_clip(handle);
        if (scui_area_inter(&dst_clip, &dst_area, clip))
        if (scui_area_limit_offset(&dst_clip, &offset_1))
            scui_widget_draw_image(handle, &dst_clip, edge, &src_clip, color_edge);
        
        /* 填充这块区域 */
        scui_area_t area = {
            .x = clip->x + offset_2.x + src_clip.h,
            .w = offset_1.x - offset_2.x - src_clip.w,
            .h = src_clip.h,
            .y = clip->y,
        };
        dst_clip = scui_widget_clip(handle);
        if (scui_area_inter(&dst_area, &dst_clip, clip))
        if (scui_area_inter(&dst_clip, &dst_area, &area))
            scui_widget_draw_color(handle, &dst_clip, color_edge);
        
    } else {
        scui_coord_t offset_d1 = scui_map(cmin, vmin, vmax, dist - src_clip.h, 0);
        scui_coord_t offset_d2 = scui_map(cmax, vmin, vmax, dist - src_clip.h, 0);
        scui_point_t offset_1 = {.y = offset_d1};
        scui_point_t offset_2 = {.y = offset_d2};
        
        src_clip.h /= 2;
        /* 绘制俩个edge */
        src_clip.x = 0;
        src_clip.y = 0;
        dst_area = scui_widget_clip(handle);
        if (scui_area_inter(&dst_clip, &dst_area, clip))
        if (scui_area_limit_offset(&dst_clip, &offset_2))
            scui_widget_draw_image(handle, &dst_clip, edge, &src_clip, color_edge);
        
        src_clip.x = 0;
        src_clip.y = src_clip.h / 2;
        dst_area = scui_widget_clip(handle);
        if (scui_area_inter(&dst_clip, &dst_area, clip))
        if (scui_area_limit_offset(&dst_clip, &offset_1))
            scui_widget_draw_image(handle, &dst_clip, edge, &src_clip, color_edge);
        
        /* 填充这块区域 */
        scui_area_t area = {
            .x = clip->x,
            .w = src_clip.w,
            .y = clip->y + offset_2.y + src_clip.h,
            .h = offset_1.y - offset_2.y - src_clip.h,
        };
        dst_clip = scui_widget_clip(handle);
        if (scui_area_inter(&dst_area, &dst_clip, clip))
        if (scui_area_inter(&dst_clip, &dst_area, &area))
            scui_widget_draw_color(handle, &dst_clip, color_edge);
        
    }
}

/*@brief 自定义控件:插件:加载圆环
 *@param handle 自定义控件句柄
 *@param clip   绘制区域
 *@param data   自定义参数集
 */
void scui_custom_draw_spinner(scui_handle_t handle, scui_area_t *clip, scui_custom_data_t *data)
{
    /* draw data<s> */
    scui_handle_t spinner   = data->spinner.spinner;
    scui_handle_t edge      = data->spinner.edge;
    scui_color_t  color     = data->spinner.color;
    scui_coord_t  percent   = data->spinner.percent;
    scui_coord_t  angle_s   = data->spinner.angle_s;
    scui_coord_t  angle_l   = data->spinner.angle_l;
    scui_coord_t  way       = data->spinner.way;
    /* draw data<e> */
    SCUI_LOG_DEBUG("");
    SCUI_ASSERT(clip != NULL);
    
    /* 绘制背景 */
    if (color.color_d.full != 0x0) {
        scui_color_t color_bg = SCUI_COLOR_MAKE(color.filter, color.color_f, color.color_d);
        scui_widget_draw_image(handle, clip, spinner, NULL, color_bg);
    }
    
    scui_coord_t  angle_c = scui_map(percent % 100, 0, 100, 0, 360);
    scui_map_cb_t path_map = scui_map_ease_out;
    if (scui_mabs(angle_c / 180, 2) == 1)
        path_map = scui_map_ease_in;
    
    scui_coord_t adj_p = scui_mabs(angle_c % 180, 180);
    scui_coord_t adj_s = angle_s + way * path_map(adj_p, 0, 180, 0, 360);
    scui_coord_t adj_e = angle_s + way * scui_map(adj_p, 0, 180, 0, 360) + way * angle_l;
    
    /* 绘制圆环 */
    scui_color_t color_fg = SCUI_COLOR_MAKE_SE(color.filter, color.color_f, color.color_l, color.color_l);
    scui_widget_draw_ring(handle, clip, spinner, NULL, adj_s, color_fg, adj_e, 100, edge);
}

/*@brief 自定义控件:插件:导航点
 *@param handle 自定义控件句柄
 *@param clip   绘制区域
 *@param data   自定义参数集
 */
void scui_custom_draw_indicator(scui_handle_t handle, scui_area_t *clip, scui_custom_data_t *data)
{
    /* draw data<s> */
    scui_handle_t wait          = data->indicator.wait;
    scui_handle_t focus         = data->indicator.focus;
    scui_color_t  color_wait    = data->indicator.color_wait;
    scui_color_t  color_focus   = data->indicator.color_focus;
    scui_handle_t count         = data->indicator.count;
    scui_handle_t index         = data->indicator.index;
    scui_handle_t span          = data->indicator.span;
    bool          way           = data->indicator.way;
    /* draw data<e> */
    SCUI_LOG_DEBUG("");
    SCUI_ASSERT(clip != NULL);
    
    scui_point_t offset = {0};
    for (scui_multi_t idx = 0; idx < count; idx++) {
        if (idx == index) {
            scui_area_t dst_clip = *clip;
            if (scui_area_limit_offset(&dst_clip, &offset))
                scui_widget_draw_image(handle, &dst_clip, focus, NULL, color_focus);
            
            if (way) offset.y += scui_image_h(focus) + span;
            else offset.x += scui_image_w(focus) + span;
        } else {
            scui_area_t dst_clip = *clip;
            if (scui_area_limit_offset(&dst_clip, &offset))
                scui_widget_draw_image(handle, &dst_clip, wait, NULL, color_wait);
            
            if (way) offset.y += scui_image_h(wait) + span;
            else offset.x += scui_image_w(wait) + span;
        }
    }
}

/*@brief 自定义控件:插件:绕圆旋转图像
 *@param handle 自定义控件句柄
 *@param clip   绘制区域
 *@param data   自定义参数集
 */
void scui_custom_draw_ring_edge(scui_handle_t handle, scui_area_t *clip, scui_custom_data_t *data)
{
    /* draw data<s> */
    scui_handle_t image         = data->ring_edge.image;
    scui_color_t  color         = data->ring_edge.color;
    scui_point_t  center        = data->ring_edge.center;
    scui_coord_t  radius        = data->ring_edge.radius;
    scui_coord_t  angle         = data->ring_edge.angle;
    /* draw data<e> */
    SCUI_LOG_DEBUG("");
    
    scui_multi_t point_x = radius * scui_cos4096((int32_t)angle) >> 12;
    scui_multi_t point_y = radius * scui_sin4096((int32_t)angle) >> 12;
    
    scui_area_t clip_edge = {
        .x = center.x + point_x - scui_image_w(image) / 2,
        .y = center.y + point_y - scui_image_h(image) / 2,
        .w = scui_image_w(image),
        .h = scui_image_h(image),
    };
    scui_widget_draw_image(handle, &clip_edge, image, NULL, color);
}

/*@brief 自定义控件:插件:图像连续绘制
 *       一般主要用于绘制连续数字符号图片
 *@param handle 自定义控件句柄
 *@param clip   绘制区域
 *@param data   自定义参数集
 */
void scui_custom_draw_image_text(scui_handle_t handle, scui_area_t *clip, scui_custom_data_t *data)
{
    /* draw data<s> */
    scui_handle_t *image        = data->image_text.image;
    scui_color_t   color        = data->image_text.color;
    scui_coord_t   span         = data->image_text.span;
    scui_coord_t   num          = data->image_text.num;
    bool           way          = data->image_text.way;
    /* draw data<e> */
    SCUI_LOG_DEBUG("");
    SCUI_ASSERT(clip != NULL);
    SCUI_ASSERT(image != NULL);
    
    scui_point_t offset = {0};
    for (scui_multi_t idx = 0; idx < num; idx++) {
        if (image[idx] == SCUI_HANDLE_INVALID)
            continue;
        
        scui_area_t dst_clip = *clip;
        if (scui_area_limit_offset(&dst_clip, &offset))
            scui_widget_draw_image(handle, &dst_clip, image[idx], NULL, color);
        
        if (way) offset.y += span + scui_image_h(image[idx]);
        else offset.x += span + scui_image_w(image[idx]);
    }
}

/*@brief 按钮控件绘制(四个角使用图像绘制)
 *@param handle 自定义控件句柄
 *@param clip   绘制区域
 *@param data   自定义参数集
 */
void scui_custom_draw_image_crect4(scui_handle_t handle, scui_area_t *clip, scui_custom_data_t *data)
{
    /* draw data<s> */
    scui_handle_t *image        = data->image_crect4.image;
    scui_color_t   color        = data->image_crect4.color;
    scui_coord_t   delta        = data->image_crect4.delta;
    /* draw data<e> */
    SCUI_LOG_DEBUG("");
    SCUI_ASSERT(clip != NULL);
    SCUI_ASSERT(image != NULL);
    
    scui_area_t image_clip = {.w = -1, .h = -1,};
    
    /* 断言检查,所有有效图片应该保持同一宽与高 */
    for (scui_multi_t idx = 0; idx < 4; idx++) {
        if (image[idx] != SCUI_HANDLE_INVALID) {
            SCUI_ASSERT(image_clip.w == -1 || image_clip.w == scui_image_w(image[idx]));
            SCUI_ASSERT(image_clip.h == -1 || image_clip.h == scui_image_h(image[idx]));
            if (image_clip.w == -1) image_clip.w  = scui_image_w(image[idx]);
            if (image_clip.h == -1) image_clip.h  = scui_image_h(image[idx]);
        }
    }
    
    scui_area_t clip_f = {0};
    scui_area_t clip_a = {0};
    /* 无图片,全填充或者不填充 */
    if (image_clip.w == -1 || image_clip.h == -1) {
        if (delta == -1) {
            scui_widget_draw_color(handle, clip, color);
            return;
        }
        clip_a.w = delta;
        clip_a.h = delta;
    } else {
        clip_a.w = image_clip.w;
        clip_a.h = image_clip.h;
    }
    
    scui_coord_t delta_w = clip->w - clip_a.w * 2;
    scui_coord_t delta_h = clip->h - clip_a.h * 2;
    /* 画俩个填充矩形(填充),画四个填充矩形(空心) */
    if (delta != 0) {
        if (delta_w > 0) {
            clip_f.w = delta_w;
            clip_f.h = delta != -1 ? delta : clip->h;
            /* 画第一个矩形 */
            clip_f.x = clip->x + clip_a.w;
            clip_f.y = clip->y;
            scui_area_t dst_clip = {0};
            if (scui_area_inter(&dst_clip, clip, &clip_f))
                scui_widget_draw_color(handle, &dst_clip, color);
            /* 画第二个矩形 */
            if (delta != -1) {
                clip_f.x = clip->x + clip_a.w;
                clip_f.y = clip->y + clip->h - delta;
                scui_area_t dst_clip = {0};
                if (scui_area_inter(&dst_clip, clip, &clip_f))
                    scui_widget_draw_color(handle, &dst_clip, color);
            }
        }
        if (delta_h > 0) {
            clip_f.w = delta != -1 ? delta : clip->w;
            clip_f.h = delta_h;
            /* 画第一个矩形 */
            clip_f.x = clip->x;
            clip_f.y = clip->y + clip_a.h;
            scui_area_t dst_clip = {0};
            if (scui_area_inter(&dst_clip, clip, &clip_f))
                scui_widget_draw_color(handle, &dst_clip, color);
            /* 画第二个矩形 */
            if (delta != -1) {
                clip_f.x = clip->x + clip->w - delta;
                clip_f.y = clip->y + clip_a.h;
                scui_area_t dst_clip = {0};
                if (scui_area_inter(&dst_clip, clip, &clip_f))
                    scui_widget_draw_color(handle, &dst_clip, color);
            }
        }
    }
    /* 无图片 */
    if (image_clip.w == -1 || image_clip.h == -1)
        return;
    
    /* 画四个角(最多) */
    scui_point_t dst_point[4] = {0};
    for (scui_coord_t idx = 0; idx < 4; idx++) {
        dst_point[idx].x = clip->x;
        dst_point[idx].y = clip->y;
    }
    dst_point[1].x = clip->x + clip->w - clip_a.w;
    dst_point[2].y = clip->y + clip->h - clip_a.h;
    dst_point[3].x = dst_point[1].x;
    dst_point[3].y = dst_point[2].y;
    
    scui_area_t dst_clip = {0};
    for (scui_coord_t idx = 0; idx < 4; idx++) {
        clip_a.x = dst_point[idx].x;
        clip_a.y = dst_point[idx].y;
        
        if (image[idx] == SCUI_HANDLE_INVALID) {
            if (scui_area_inter(&dst_clip, clip, &clip_a))
                scui_widget_draw_color(handle, &dst_clip, color);
        } else {
            if (scui_area_inter(&dst_clip, clip, &clip_a))
                scui_widget_draw_image(handle, &dst_clip, image[idx], NULL, color);
        }
    }
    /* ... */
}

