/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 按钮控件创建
 *@param maker  按钮控件创建参数
 *@param handle 按钮控件句柄
 *@param layout 通过布局创建
 */
void scui_button_create(scui_button_maker_t *maker, scui_handle_t *handle, bool layout)
{
    SCUI_ASSERT(maker->widget.type == scui_widget_type_button);
    SCUI_ASSERT(maker->widget.parent != SCUI_HANDLE_INVALID);
    
    /* 创建按钮控件实例 */
    scui_button_t *button = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_button_t));
    memset(button, 0, sizeof(scui_button_t));
    
    /* 创建基础控件实例 */
    scui_widget_maker_t widget_maker = maker->widget;
    
    /* 必须标记ptr事件 */
    widget_maker.style.indev_ptr = true;
    /* 必须标记anima事件 */
    if (maker->mode == scui_button_mode_scale)
        widget_maker.style.sched_anima = true;
    
    /* 创建基础控件实例 */
    scui_widget_create(&button->widget, &widget_maker, handle, layout);
    
    button->mode        = maker->mode;
    button->image[0]    = maker->image[0];
    button->image[1]    = maker->image[1];
    button->image[2]    = maker->image[2];
    button->image[3]    = maker->image[3];
    button->color       = maker->color;
    button->delta       = maker->delta;
    button->notify_cb   = maker->notify_cb;
    
    if (button->mode == scui_button_mode_scale) {
        button->btn1_lim  = SCUI_WIDGET_BUTTON_BTN1_PCT;
        button->btn1_way  = -1;
        button->btn1_pct  = SCUI_WIDGET_BUTTON_BTN1_PCT;
        button->btn1_hold = false;
    }
}

/*@brief 按钮控件销毁
 *@param handle 按钮控件句柄
 */
void scui_button_destroy(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_button_t *button = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_button);
    
    /* 销毁基础控件实例 */
    scui_widget_destroy(&button->widget);
    
    /* 销毁按钮控件实例 */
    SCUI_MEM_FREE(button);
}

/*@brief 按钮控件绘制(四个角使用图像绘制)
 *@param event 自定义控件事件
 *@param clip  剪切域(绘制区域)
 *@param image 图像句柄(左上角,右上角,左下角,右下角)
 *@param color 图像源色调
 *@param delta 边界填充线(0:忽略填充(复杂图像集成);-1:全填充(全填充圆角矩形);其他:边界填充(空心圆角矩形))
 */
void scui_button_draw_rect4(scui_event_t *event,    scui_area_t *clip,
                            scui_handle_t image[4], scui_color_t color,
                            scui_coord_t  delta)
{
    // 本接口允许在custom回调中调用
    SCUI_ASSERT(event->type != scui_widget_type_custom &&
                event->type != scui_widget_type_button);
    
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

/*@brief 按钮控件事件处理回调
 *@param event 事件
 */
void scui_button_event(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_handle_t  handle = event->object;
    scui_widget_t *widget = scui_handle_get(handle);
    scui_button_t *button = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        if (!scui_widget_event_check_execute(event))
             return;
        
        if (button->mode == scui_button_mode_scale) {
            if (!(button->btn1_pct <= button->btn1_lim && button->btn1_way == -1)) {
                if (!button->btn1_hold && (button->btn1_pct == 100 || button->btn1_pct == button->btn1_lim))
                     button->btn1_way  = -button->btn1_way;
                if ((button->btn1_pct  < 100 && button->btn1_pct > button->btn1_lim) ||
                    (button->btn1_pct == 100 && button->btn1_way == -1) ||
                    (button->btn1_pct == button->btn1_lim && button->btn1_way == +1)) {
                     button->btn1_pct += button->btn1_way;
                     scui_widget_draw(widget->myself, NULL, false);
                }
                SCUI_LOG_INFO("<%d, %d>", button->btn1_pct, button->btn1_way);
            } else {
                if (button->btn_click) {
                    button->btn_click = false;
                    if (button->notify_cb != NULL) {
                        scui_event_t event = {
                            .object = widget->myself,
                            .type   = scui_event_widget_button_click,
                        };
                        button->notify_cb(&event);
                    }
                }
            }
        }
        
        break;
    }
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             return;
        
        if (button->mode == scui_button_mode_static) {
            scui_button_draw_rect4(event, &widget->clip, button->image, button->color, button->delta);
            break;
        }
        if (button->mode == scui_button_mode_scale) {
            SCUI_ASSERT(button->btn1_pct >= button->btn1_lim && button->btn1_pct <= 100);
            scui_area_t  scale_clip = widget->clip;
            scui_multi_t scale_x = scale_clip.w * (100 - button->btn1_pct) / 100 / 2;
            scui_multi_t scale_y = scale_clip.h * (100 - button->btn1_pct) / 100 / 2;
            scale_clip.x += scale_x;
            scale_clip.y += scale_y;
            scale_clip.w -= scale_x * 2;
            scale_clip.h -= scale_y * 2;
            scui_button_draw_rect4(event, &scale_clip, button->image, button->color, button->delta);
            break;
        }
        break;
    }
    case scui_event_ptr_down: {
        if (!scui_widget_event_check_execute(event))
             return;
        
        scui_handle_t handle = scui_widget_root(widget->myself);
        scui_widget_t  *root = scui_handle_get(handle);
        
        scui_area_t clip = widget->clip;
        if (widget != root) {
            clip.x += root->clip.x;
            clip.y += root->clip.y;
        }
        if (!scui_area_point(&clip, &event->ptr_c))
             break;
        
        if (button->mode == scui_button_mode_scale) {
            SCUI_LOG_INFO("");
            button->btn1_hold = true;
            button->btn1_pct  = button->btn1_lim;
            button->btn1_way  = +1;
            break;
        }
        break;
    }
    case scui_event_ptr_up: {
        if (!scui_widget_event_check_execute(event))
             return;
        
        if (button->mode == scui_button_mode_scale) {
            SCUI_LOG_INFO("");
            button->btn1_hold = false;
            break;
        }
        break;
    }
    case scui_event_ptr_click: {
        if (!scui_widget_event_check_execute(event))
             return;
        
        if (button->mode == scui_button_mode_static) {
            if (button->notify_cb != NULL) {
                scui_event_t event = {
                    .object = widget->myself,
                    .type   = scui_event_widget_button_click,
                };
                button->notify_cb(&event);
            }
            break;
        }
        if (button->mode == scui_button_mode_scale) {
            scui_widget_event_mask_over(event);
            SCUI_LOG_INFO("");
            button->btn1_hold = true;
            button->btn_click = true;
            break;
        }
        
        break;
    }
    default:
        break;
    }
}
