/*实现目标:
 *    自定义插件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 自定义控件:插件:加载圆环
 *@param draw_dsc 绘制参数实例
 */
void scui_custom_draw_p_spinner(scui_custom_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_event_t *event     = draw_dsc->event;
    scui_area_t  *clip      = draw_dsc->clip;
    scui_handle_t spinner   = draw_dsc->spinner.spinner;
    scui_handle_t edge      = draw_dsc->spinner.edge;
    scui_color_t  color     = draw_dsc->spinner.color;
    scui_coord_t  percent   = draw_dsc->spinner.percent;
    scui_coord_t  angle_s   = draw_dsc->spinner.angle_s;
    scui_coord_t  angle_l   = draw_dsc->spinner.angle_l;
    scui_coord_t  way       = draw_dsc->spinner.way;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("");
    SCUI_ASSERT(clip != NULL);
    
    /* 绘制背景 */
    if (color.color_d.full != 0x0) {
        scui_color_t color_bg = {
            .color   = color.color_d,
            .color_f = color.color_f,
            .filter  = color.filter
        };
        scui_widget_draw_image(event->object, clip, spinner, NULL, color_bg);
    }
    
    scui_coord_t  angle_c = scui_map(percent % 100, 0, 100, 0, 360);
    scui_map_cb_t path_map = scui_map_ease_out;
    if (scui_mabs(angle_c / 180, 2) == 1)
        path_map = scui_map_ease_in;
    
    scui_coord_t adj_p = scui_mabs(angle_c % 180, 180);
    scui_coord_t adj_s = angle_s + way * path_map(adj_p, 0, 180, 0, 360);
    scui_coord_t adj_e = angle_s + way * scui_map(adj_p, 0, 180, 0, 360) + way * angle_l;
    
    /* 绘制圆环 */
    scui_color_t color_fg = {
        .color_s = color.color_l,
        .color_e = color.color_l,
        .color_f = color.color_f,
        .filter  = color.filter
    };
    scui_widget_draw_ring(event->object, clip, spinner, NULL, adj_s, color_fg, adj_e, 100, edge);
}

/*@brief 自定义控件:插件:进度条,滚动条
 *@param draw_dsc 绘制参数实例
 */
void scui_custom_draw_p_slider(scui_custom_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_event_t *event         = draw_dsc->event;
    scui_area_t  *clip          = draw_dsc->clip;
    scui_handle_t bar           = draw_dsc->slider.bar;
    scui_handle_t edge          = draw_dsc->slider.edge;
    scui_color_t  color_bar     = draw_dsc->slider.color_bar;
    scui_color_t  color_edge    = draw_dsc->slider.color_edge;
    scui_coord_t  vmin          = draw_dsc->slider.vmin;
    scui_coord_t  vmax          = draw_dsc->slider.vmax;
    scui_coord_t  cmin          = draw_dsc->slider.cmin;
    scui_coord_t  cmax          = draw_dsc->slider.cmax;
    scui_handle_t dist          = draw_dsc->slider.dist;
    bool          way           = draw_dsc->slider.way;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("");
    SCUI_ASSERT(clip != NULL);
    
    /* 绘制背景进度条 */
    scui_widget_draw_image(event->object, clip, bar, NULL, color_bar);
    
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
        dst_area = scui_widget_clip(event->object);
        if (scui_area_inter(&dst_clip, &dst_area, clip))
        if (scui_area_limit_offset(&dst_clip, &offset_2))
            scui_widget_draw_image(event->object, &dst_clip, edge, &src_clip, color_edge);
        
        src_clip.x = src_clip.w;
        src_clip.y = 0;
        dst_area = scui_widget_clip(event->object);
        if (scui_area_inter(&dst_clip, &dst_area, clip))
        if (scui_area_limit_offset(&dst_clip, &offset_1))
            scui_widget_draw_image(event->object, &dst_clip, edge, &src_clip, color_edge);
        
        /* 填充这块区域 */
        scui_area_t area = {
            .x = clip->x + offset_2.x + src_clip.h,
            .w = offset_1.x - offset_2.x - src_clip.w,
            .h = src_clip.h,
            .y = clip->y,
        };
        dst_clip = scui_widget_clip(event->object);
        if (scui_area_inter(&dst_area, &dst_clip, clip))
        if (scui_area_inter(&dst_clip, &dst_area, &area))
            scui_widget_draw_color(event->object, &dst_clip, color_edge);
        
    } else {
        scui_coord_t offset_d1 = scui_map(cmin, vmin, vmax, dist - src_clip.h, 0);
        scui_coord_t offset_d2 = scui_map(cmax, vmin, vmax, dist - src_clip.h, 0);
        scui_point_t offset_1 = {.y = offset_d1};
        scui_point_t offset_2 = {.y = offset_d2};
        
        src_clip.h /= 2;
        /* 绘制俩个edge */
        src_clip.x = 0;
        src_clip.y = 0;
        dst_area = scui_widget_clip(event->object);
        if (scui_area_inter(&dst_clip, &dst_area, clip))
        if (scui_area_limit_offset(&dst_clip, &offset_2))
            scui_widget_draw_image(event->object, &dst_clip, edge, &src_clip, color_edge);
        
        src_clip.x = 0;
        src_clip.y = src_clip.h / 2;
        dst_area = scui_widget_clip(event->object);
        if (scui_area_inter(&dst_clip, &dst_area, clip))
        if (scui_area_limit_offset(&dst_clip, &offset_1))
            scui_widget_draw_image(event->object, &dst_clip, edge, &src_clip, color_edge);
        
        /* 填充这块区域 */
        scui_area_t area = {
            .x = clip->x,
            .w = src_clip.w,
            .y = clip->y + offset_2.y + src_clip.h,
            .h = offset_1.y - offset_2.y - src_clip.h,
        };
        dst_clip = scui_widget_clip(event->object);
        if (scui_area_inter(&dst_area, &dst_clip, clip))
        if (scui_area_inter(&dst_clip, &dst_area, &area))
            scui_widget_draw_color(event->object, &dst_clip, color_edge);
        
    }
}

/*@brief 自定义控件:插件:导航点
 *@param draw_dsc 绘制参数实例
 */
void scui_custom_draw_p_indicator(scui_custom_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_event_t *event         = draw_dsc->event;
    scui_area_t  *clip          = draw_dsc->clip;
    scui_handle_t wait          = draw_dsc->indicator.wait;
    scui_handle_t focus         = draw_dsc->indicator.focus;
    scui_color_t  color_wait    = draw_dsc->indicator.color_wait;
    scui_color_t  color_focus   = draw_dsc->indicator.color_focus;
    scui_handle_t count         = draw_dsc->indicator.count;
    scui_handle_t index         = draw_dsc->indicator.index;
    scui_handle_t span          = draw_dsc->indicator.span;
    bool          way           = draw_dsc->indicator.way;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("");
    SCUI_ASSERT(clip != NULL);
    
    scui_point_t offset = {0};
    for (scui_handle_t idx = 0; idx < count; idx++) {
        if (idx == index) {
            scui_area_t dst_clip = *clip;
            if (scui_area_limit_offset(&dst_clip, &offset))
                scui_widget_draw_image(event->object, &dst_clip, focus, NULL, color_focus);
            if (way)
                offset.y += scui_image_h(focus) + span;
            else
                offset.x += scui_image_w(focus) + span;
        } else {
            scui_area_t dst_clip = *clip;
            if (scui_area_limit_offset(&dst_clip, &offset))
                scui_widget_draw_image(event->object, &dst_clip, wait, NULL, color_wait);
            if (way)
                offset.y += scui_image_h(wait) + span;
            else
                offset.x += scui_image_w(wait) + span;
        }
    }
}

/*@brief 自定义控件:插件:绕圆旋转图像
 *@param draw_dsc 绘制参数实例
 */
void scui_custom_draw_p_ring_edge(scui_custom_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_event_t *event         = draw_dsc->event;
    scui_area_t  *clip          = draw_dsc->clip;
    scui_handle_t image         = draw_dsc->ring_edge.image;
    scui_color_t  color         = draw_dsc->ring_edge.color;
    scui_point_t *center        = draw_dsc->ring_edge.center;
    scui_coord_t  radius        = draw_dsc->ring_edge.radius;
    scui_coord_t  angle         = draw_dsc->ring_edge.angle;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("");
    
    scui_multi_t point_x = radius * scui_cos4096((int32_t)angle) >> 12;
    scui_multi_t point_y = radius * scui_sin4096((int32_t)angle) >> 12;
    
    scui_area_t clip_edge = {
        .x = center->x + point_x - scui_image_w(image) / 2,
        .y = center->y + point_y - scui_image_h(image) / 2,
        .w = scui_image_w(image),
        .h = scui_image_h(image),
    };
    scui_widget_draw_image(event->object, &clip_edge, image, NULL, color);
}

/*@brief 自定义控件:插件:图像连续绘制
 *       一般主要用于绘制连续数字符号图片
 *@param draw_dsc 绘制参数实例
 */
void scui_custom_draw_p_image_text(scui_custom_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_event_t  *event        = draw_dsc->event;
    scui_area_t   *clip         = draw_dsc->clip;
    scui_handle_t *image        = draw_dsc->image_text.image;
    scui_color_t   color        = draw_dsc->image_text.color;
    scui_coord_t   span         = draw_dsc->image_text.span;
    scui_coord_t   num          = draw_dsc->image_text.num;
    bool           way          = draw_dsc->image_text.way;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("");
    SCUI_ASSERT(clip != NULL);
    SCUI_ASSERT(image != NULL);
    
    scui_point_t offset = {0};
    for (scui_handle_t idx = 0; idx < num; idx++) {
        if (image[idx] == SCUI_HANDLE_INVALID)
            continue;
        
        scui_area_t dst_clip = *clip;
        if (scui_area_limit_offset(&dst_clip, &offset))
            scui_widget_draw_image(event->object, &dst_clip, image[idx], NULL, color);
        
        if (way)
            offset.y += span + scui_image_h(image[idx]);
        else
            offset.x += span + scui_image_w(image[idx]);
    }
}

/*@brief 按钮控件绘制(四个角使用图像绘制)
 *@param draw_dsc 绘制参数实例
 */
void scui_custom_draw_p_image_crect4(scui_custom_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_event_t  *event        = draw_dsc->event;
    scui_area_t   *clip         = draw_dsc->clip;
    scui_handle_t *image        = draw_dsc->image_crect4.image;
    scui_color_t   color        = draw_dsc->image_crect4.color;
    scui_coord_t   delta        = draw_dsc->image_crect4.delta;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("");
    SCUI_ASSERT(clip != NULL);
    SCUI_ASSERT(image != NULL);
    
    scui_area_t image_clip = {.w = -1, .h = -1,};
    
    /* 断言检查,所有有效图片应该保持同一宽与高 */
    for (uint8_t idx = 0; idx < 4; idx++) {
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
            scui_widget_draw_color(event->object, clip, color);
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
                scui_widget_draw_color(event->object, &dst_clip, color);
            /* 画第二个矩形 */
            if (delta != -1) {
                clip_f.x = clip->x + clip_a.w;
                clip_f.y = clip->y + clip->h - delta;
                scui_area_t dst_clip = {0};
                if (scui_area_inter(&dst_clip, clip, &clip_f))
                    scui_widget_draw_color(event->object, &dst_clip, color);
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
                scui_widget_draw_color(event->object, &dst_clip, color);
            /* 画第二个矩形 */
            if (delta != -1) {
                clip_f.x = clip->x + clip->w - delta;
                clip_f.y = clip->y + clip_a.h;
                scui_area_t dst_clip = {0};
                if (scui_area_inter(&dst_clip, clip, &clip_f))
                    scui_widget_draw_color(event->object, &dst_clip, color);
            }
        }
    }
    /* 无图片 */
    if (image_clip.w == -1 || image_clip.h == -1)
        return;
    /* 画一个角 */
    clip_a.x = clip->x;
    clip_a.y = clip->y;
    if (image[0] == SCUI_HANDLE_INVALID) {
        scui_area_t dst_clip = {0};
        if (scui_area_inter(&dst_clip, clip, &clip_a))
            scui_widget_draw_color(event->object, &dst_clip, color);
    } else {
        scui_area_t dst_clip = {0};
        if (scui_area_inter(&dst_clip, clip, &clip_a))
            scui_widget_draw_image(event->object, &dst_clip, image[0], NULL, color);
    }
    /* 画一个角 */
    clip_a.x = clip->x + clip->w - clip_a.w;
    clip_a.y = clip->y;
    if (image[1] == SCUI_HANDLE_INVALID) {
        scui_area_t dst_clip = {0};
        if (scui_area_inter(&dst_clip, clip, &clip_a))
            scui_widget_draw_color(event->object, &dst_clip, color);
    } else {
        scui_area_t dst_clip = {0};
        if (scui_area_inter(&dst_clip, clip, &clip_a))
            scui_widget_draw_image(event->object, &dst_clip, image[1], NULL, color);
    }
    /* 画一个角 */
    clip_a.x = clip->x;
    clip_a.y = clip->y + clip->h - clip_a.h;
    if (image[2] == SCUI_HANDLE_INVALID) {
        scui_area_t dst_clip = {0};
        if (scui_area_inter(&dst_clip, clip, &clip_a))
            scui_widget_draw_color(event->object, &dst_clip, color);
    } else {
        scui_area_t dst_clip = {0};
        if (scui_area_inter(&dst_clip, clip, &clip_a))
            scui_widget_draw_image(event->object, &dst_clip, image[2], NULL, color);
    }
    /* 画一个角 */
    clip_a.x = clip->x + clip->w - clip_a.w;
    clip_a.y = clip->y + clip->h - clip_a.h;
    if (image[3] == SCUI_HANDLE_INVALID) {
        scui_area_t dst_clip = {0};
        if (scui_area_inter(&dst_clip, clip, &clip_a))
            scui_widget_draw_color(event->object, &dst_clip, color);
    } else {
        scui_area_t dst_clip = {0};
        if (scui_area_inter(&dst_clip, clip, &clip_a))
            scui_widget_draw_image(event->object, &dst_clip, image[3], NULL, color);
    }
    /* ... */
}

