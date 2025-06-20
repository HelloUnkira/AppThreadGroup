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
void scui_button_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_button_t *button = widget;
    scui_button_maker_t *button_maker = widget_maker;
    
    /* 必须标记anima,widget,ptr事件 */
    widget_maker->style.sched_anima  = true;
    widget_maker->style.sched_widget = true;
    widget_maker->style.indev_ptr    = true;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_button));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    button->mode        = button_maker->mode;
    button->image[0]    = button_maker->image[0];
    button->image[1]    = button_maker->image[1];
    button->image[2]    = button_maker->image[2];
    button->image[3]    = button_maker->image[3];
    button->color       = button_maker->color;
    button->delta       = button_maker->delta;
    
    if (button->mode == scui_button_mode_scale) {
        button->btn1_lim  = SCUI_WIDGET_BUTTON_BTN1_PCT;
        button->btn1_way  = -1;
        button->btn1_pct  = SCUI_WIDGET_BUTTON_BTN1_PCT;
        button->btn1_hold = false;
    }
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_button_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_button));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_button_t *button = (void *)widget;
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_button_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_button_t *button = (void *)widget;
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
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
                    scui_event_t event = {
                        .object     = widget->myself,
                        .style.sync = true,
                        .type       = scui_event_widget_button_click,
                    };
                    scui_event_notify(&event);
                }
            }
        }
        
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             return;
        
        if (button->mode == scui_button_mode_static) {
            scui_custom_draw_image_crect4(event, &widget->clip, button->image, button->color, button->delta);
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
            scui_custom_draw_image_crect4(event, &scale_clip, button->image, button->color, button->delta);
            break;
        }
        break;
    }
    case scui_event_ptr_down: {
        
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
        
        if (button->mode == scui_button_mode_scale) {
            SCUI_LOG_INFO("");
            button->btn1_hold = false;
            break;
        }
        break;
    }
    case scui_event_ptr_click: {
        
        if (button->mode == scui_button_mode_static) {
            scui_event_t event = {
                .object     = widget->myself,
                .style.sync = true,
                .type       = scui_event_widget_button_click,
            };
            scui_event_notify(&event);
            break;
        }
        if (button->mode == scui_button_mode_scale) {
            scui_event_mask_over(event);
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
