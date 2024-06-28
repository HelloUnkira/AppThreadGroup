/*实现目标:
 *    自定义插件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 自定义控件:插件:进度条,滚动条
 *@param event      自定义控件事件
 *@param clip       剪切域(绘制区域)
 *@param bar        图像句柄(背景图)
 *@param color_bar  图像源色调(调色板使用)
 *@param edge       图像句柄(边界点)
 *@param color_edge 图像源色调(调色板使用)
 *@param vmin       最小值(默认可为百分比:0)
 *@param vmax       最大值(默认可为百分比:100)
 *@param cmin       当前最小值
 *@param cmax       当前最大值
 *@param dist       宽度或高度
 *@param way        方向(0:水平方向;1:垂直方向)
 */
void scui_custom_draw_slider(scui_event_t *event, scui_area_t  *clip,
                             scui_handle_t bar,   scui_color_t  color_bar,
                             scui_handle_t edge,  scui_color_t  color_edge,
                             scui_coord_t  vmin,  scui_coord_t  vmax,
                             scui_coord_t  cmin,  scui_coord_t  cmax,
                             scui_handle_t dist,  bool way)
{
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
    
    scui_image_t *image_inst = scui_handle_get(edge);
    SCUI_ASSERT(image_inst != NULL);
    
    scui_area_t  dst_area = {0};
    scui_area_t  dst_clip = *clip;
    scui_area_t  src_clip = {
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
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
        dst_area = scui_widget_draw_clip(event->object);
        if (scui_area_inter(&dst_clip, &dst_area, clip))
        if (scui_area_limit_offset(&dst_clip, &offset_2))
            scui_widget_draw_image(event->object, &dst_clip, edge, &src_clip, color_edge);
        
        src_clip.x = src_clip.w;
        src_clip.y = 0;
        dst_area = scui_widget_draw_clip(event->object);
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
        dst_clip = scui_widget_draw_clip(event->object);
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
        dst_area = scui_widget_draw_clip(event->object);
        if (scui_area_inter(&dst_clip, &dst_area, clip))
        if (scui_area_limit_offset(&dst_clip, &offset_2))
            scui_widget_draw_image(event->object, &dst_clip, edge, &src_clip, color_edge);
        
        src_clip.x = 0;
        src_clip.y = src_clip.h / 2;
        dst_area = scui_widget_draw_clip(event->object);
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
        dst_clip = scui_widget_draw_clip(event->object);
        if (scui_area_inter(&dst_area, &dst_clip, clip))
        if (scui_area_inter(&dst_clip, &dst_area, &area))
            scui_widget_draw_color(event->object, &dst_clip, color_edge);
        
    }
}

/*@brief 自定义控件:插件:导航点
 *@param event       自定义控件事件
 *@param clip        剪切域(绘制区域)
 *@param wait        图像句柄(未选中)
 *@param color_wait  图像源色调(调色板使用)
 *@param focus       图像句柄(选中)
 *@param color_focus 图像源色调(调色板使用)
 *@param count       导航点数量
 *@param index       目标索引(选中)
 *@param span        间隙
 *@param dir_hor     水平方向
 */
void scui_custom_draw_indicator(scui_event_t *event, scui_area_t  *clip,
                                scui_handle_t wait,  scui_color_t  color_wait,
                                scui_handle_t focus, scui_color_t  color_focus,
                                scui_handle_t count, scui_handle_t index,
                                scui_handle_t span,  bool          dir_hor)
{
    SCUI_LOG_DEBUG("");
    SCUI_ASSERT(clip != NULL);
    
    scui_image_t *image_wait  = scui_handle_get(wait);
    scui_image_t *image_focus = scui_handle_get(focus);
    SCUI_ASSERT(image_wait  != NULL);
    SCUI_ASSERT(image_focus != NULL);
    
    scui_point_t offset = {0};
    for (scui_handle_t idx = 0; idx < count; idx++) {
        if (idx == index) {
            scui_area_t dst_clip = *clip;
            if (scui_area_limit_offset(&dst_clip, &offset))
                scui_widget_draw_image(event->object, &dst_clip, focus, NULL, color_focus);
            if (dir_hor)
                offset.x += image_focus->pixel.width  + span;
            else
                offset.y += image_focus->pixel.height + span;
        } else {
            scui_area_t dst_clip = *clip;
            if (scui_area_limit_offset(&dst_clip, &offset))
                scui_widget_draw_image(event->object, &dst_clip, wait, NULL, color_wait);
            if (dir_hor)
                offset.x += image_wait->pixel.width  + span;
            else
                offset.y += image_wait->pixel.height + span;
        }
    }
}

/*@brief 自定义控件:插件:绕圆旋转图像
 *@param event  自定义控件事件
 *@param center 旋转中心
 *@param image  图像句柄
 *@param color  图像源色调(调色板使用)
 *@param radius 旋转半径
 *@param angle  旋转角度(顺时针旋转:+,逆时针旋转:-)
 */
void scui_custom_draw_ring_edge(scui_event_t *event,  scui_point_t *center,
                                scui_handle_t image,  scui_color_t  color,
                                scui_coord_t  radius, scui_coord_t  angle)
{
    SCUI_LOG_DEBUG("");
    
    scui_image_t *image_inst = scui_handle_get(image);
    SCUI_ASSERT(image_inst != NULL);
    
    scui_multi_t point_x = radius * scui_cos4096((int32_t)angle) >> 12;
    scui_multi_t point_y = radius * scui_sin4096((int32_t)angle) >> 12;
    
    scui_area_t clip = {
        .x = center->x + point_x - image_inst->pixel.width  / 2,
        .y = center->y + point_y - image_inst->pixel.height / 2,
        .w = image_inst->pixel.width,
        .h = image_inst->pixel.height,
    };
    scui_widget_draw_image(event->object, &clip, image, NULL, color);
}

/*@brief 自定义控件:插件:图像连续绘制
 *       一般主要用于绘制连续数字符号图片
 *@param event 自定义控件事件
 *@param clip  剪切域(绘制区域)
 *@param image 图像句柄(背景图)
 *@param color 图像源色调(调色板使用)
 *@param span  图像间隙
 *@param num   图像数量
 *@param way   方向(0:水平方向;1:垂直方向)
 */
void scui_custom_draw_text(scui_event_t  *event, scui_area_t *clip,
                           scui_handle_t *image, scui_color_t color,
                           scui_coord_t   span,  scui_coord_t num,
                           bool way)
{
    SCUI_LOG_DEBUG("");
    SCUI_ASSERT(clip != NULL);
    SCUI_ASSERT(image != NULL);
    
    scui_point_t offset = {0};
    for (scui_handle_t idx = 0; idx < num; idx++) {
        if (image[idx] == SCUI_HANDLE_INVALID)
            continue;
        scui_image_t *image_inst = scui_handle_get(image[idx]);
        SCUI_ASSERT(image_inst != NULL);
        
        scui_area_t dst_clip = *clip;
        if (scui_area_limit_offset(&dst_clip, &offset))
            scui_widget_draw_image(event->object, &dst_clip, image[idx], NULL, color);
        
        if (way)
            offset.y += span + image_inst->pixel.height;
        else
            offset.x += span + image_inst->pixel.width;
    }
}

/*@brief 自定义控件:插件:区域绘制(四个角使用图像绘制)
 *       一般主要用于绘制纯色圆角矩形
 *@param event 自定义控件事件
 *@param clip  剪切域(绘制区域)
 *@param image 图像句柄(左上角,右上角,左下角,右下角)
 *@param color 图像源色调
 *@param delta 边界填充线(0:忽略填充(复杂图像集成);-1:全填充(全填充圆角矩形);其他:边界填充(空心圆角矩形))
 */
void scui_custom_draw_rect4(scui_event_t *event,    scui_area_t *clip,
                            scui_handle_t image[4], scui_color_t color,
                            scui_coord_t  delta)
{
    SCUI_LOG_DEBUG("");
    SCUI_ASSERT(clip != NULL);
    SCUI_ASSERT(image != NULL);
    
    scui_area_t image_clip = {.w = -1, .h = -1,};
    scui_image_t *image_inst[4] = {0};
    for (uint8_t idx = 0; idx < 4; idx++) {
        if (image[idx] != SCUI_HANDLE_INVALID) {
            image_inst[idx] = scui_handle_get(image[idx]);
            SCUI_ASSERT(image_inst[idx] != NULL);
            /* 断言检查,所有有效图片应该保持同一宽与高 */
            SCUI_ASSERT(image_clip.w == -1 || image_clip.w == image_inst[idx]->pixel.width);
            SCUI_ASSERT(image_clip.h == -1 || image_clip.h == image_inst[idx]->pixel.height);
            
            if (image_clip.w == -1)
                image_clip.w  = image_inst[idx]->pixel.width;
            if (image_clip.h == -1)
                image_clip.h  = image_inst[idx]->pixel.height;
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
    
    scui_coord_t delta_w = clip->w - clip_a.w;
    scui_coord_t delta_h = clip->h - clip_a.h;
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

/*@brief 自定义控件:插件:区域绘制
 *@param event 自定义控件事件
 *@param clip  剪切域(绘制区域)
 *@param color 颜色(.color_s,.color_e,)
 *@param mix   绘制模式(0:常规绘制;1:渐变绘制;)
 *@param way   绘制方向(渐变绘制)(0:水平渐变;1:垂直渐变;)
 */
void scui_custom_draw_rect(scui_event_t *event, scui_area_t *clip,
                           scui_color_t  color, uint8_t mix, bool way)
{
    SCUI_LOG_DEBUG("");
    SCUI_ASSERT(clip != NULL);
    
    /* 常规填色 */
    if (mix == 0) {
        scui_widget_draw_color(event->object, clip, color);
        return;
    }
    
    /* 渐变填色 */
    if (mix == 1) {
        /* 无渐变填色 */
        if (color.color_s.full == color.color_e.full) {
            scui_widget_draw_color(event->object, clip, color);
            return;
        }
        
        scui_widget_t  *widget  = scui_handle_get(event->object);
        scui_surface_t *surface = widget->surface;
        uint32_t  pixel_byte = scui_pixel_bits(surface->format) / 8;
        uintptr_t pixel_size = pixel_byte * clip->w * clip->h;
        uint8_t  *pixel   = SCUI_MEM_ALLOC(scui_mem_type_graph, pixel_size);
        scui_color_limit_t color_s = 0;
        scui_color_limit_t color_e = 0;
        scui_color_limit_t pixel_a = 0;
        scui_pixel_by_color(surface->format, &color_s, color.color_s);
        scui_pixel_by_color(surface->format, &color_e, color.color_e);
        
        if (pixel == NULL) {
            SCUI_LOG_WARN("memory deficit was caught");
            scui_image_cache_clear();
            pixel  = SCUI_MEM_ALLOC(scui_mem_type_graph, pixel_size);
        if (pixel == NULL) {
            scui_image_cache_visit();
            SCUI_ASSERT(false);
        }
        }
        
        if (way) {
            /* 垂直渐变 */
            for (scui_coord_t y = 0; y < clip->h; y++) {
                uint8_t percent = (scui_multi_t)y * 100 / clip->h;
                scui_alpha_t alpha_1 = scui_alpha_pct(100 - percent);
                scui_alpha_t alpha_2 = scui_alpha_cover - alpha_1;
                pixel_a = color_s;
                scui_pixel_mix_with(surface->format, &pixel_a, alpha_1,
                                    surface->format, &color_e, alpha_2);
            for (scui_coord_t x = 0; x < clip->w; x++)
                scui_pixel_by_cf(surface->format, &pixel[(y * clip->w + x) * pixel_byte], &pixel_a);
            }
        } else {
            /* 水平渐变 */
            for (scui_coord_t x = 0; x < clip->w; x++) {
                uint8_t percent = (scui_multi_t)x * 100 / clip->w;
                scui_alpha_t alpha_1 = scui_alpha_pct(100 - percent);
                scui_alpha_t alpha_2 = scui_alpha_cover - alpha_1;
                pixel_a = color_s;
                scui_pixel_mix_with(surface->format, &pixel_a, alpha_1,
                                    surface->format, &color_e, alpha_2);
            for (scui_coord_t y = 0; y < clip->h; y++)
                scui_pixel_by_cf(surface->format, &pixel[(y * clip->w + x) * pixel_byte], &pixel_a);
            }
        }
        
        scui_area_t dst_clip = {0};
        scui_area_t dst_area = scui_widget_draw_clip(event->object);
        if (scui_area_inter(&dst_clip, &dst_area, clip)) {
            
            scui_image_t image_inst = {
                .status         = scui_image_status_mem,
                .pixel.width    = clip->w,
                .pixel.height   = clip->h,
                .pixel.data_mem = pixel,
            };
            scui_image_cf_by_pixel_cf(&image_inst.format, &surface->format);
            scui_handle_t image = scui_handle_find();
            scui_handle_set(image, &image_inst);
            scui_widget_draw_image(event->object, &dst_clip, image, NULL, (scui_color_t){0});
            scui_handle_set(image, NULL);
        }
        
        SCUI_MEM_FREE(pixel);
        return;
    }
    
    SCUI_ASSERT(false);
}
