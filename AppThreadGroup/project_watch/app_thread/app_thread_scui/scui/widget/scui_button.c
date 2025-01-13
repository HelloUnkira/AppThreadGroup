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
void scui_button_make(scui_button_maker_t *maker, scui_handle_t *handle, bool layout)
{
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
    scui_widget_make(&button->widget, &widget_maker, handle, layout);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_button));
    SCUI_ASSERT(widget_maker.parent != SCUI_HANDLE_INVALID);
    
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
void scui_button_burn(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_button_t *button = (void *)widget;
    
    /* 销毁基础控件实例 */
    scui_widget_burn(&button->widget);
    
    /* 销毁按钮控件实例 */
    SCUI_MEM_FREE(button);
}

/*@brief 按钮控件事件处理回调
 *@param event 事件
 */
void scui_button_event(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_button_t *button = (void *)widget;
    
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
            
            scui_custom_draw_dsc_t draw_dsc = {
                .event = event,
                .clip  = &widget->clip,
                .image_crect4.image = button->image,
                .image_crect4.color = button->color,
                .image_crect4.delta = button->delta,
            };
            scui_custom_draw_image_crect4(&draw_dsc);
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
            
            scui_custom_draw_dsc_t draw_dsc = {
                .event = event,
                .clip  = &scale_clip,
                .image_crect4.image = button->image,
                .image_crect4.color = button->color,
                .image_crect4.delta = button->delta,
            };
            scui_custom_draw_image_crect4(&draw_dsc);
            break;
        }
        break;
    }
    case scui_event_ptr_down: {
        if (!scui_widget_event_check_execute(event))
             return;
        
        scui_handle_t handle = scui_widget_root(widget->myself);
        scui_widget_t  *root = scui_handle_source_check(handle);
        
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
    case scui_event_ptr_move:
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
