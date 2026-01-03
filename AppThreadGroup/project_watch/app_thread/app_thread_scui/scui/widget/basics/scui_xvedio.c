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
void scui_xvedio_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_xvedio_t *xvedio = widget;
    scui_xvedio_maker_t *xvedio_maker = widget_maker;
    
    /* 必须标记anima事件 */
    widget_maker->style.sched_anima = true;
    
    /* 必须标记fully_bg风格 */
    widget_maker->style.fully_bg = true;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_xvedio));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    xvedio->iframe = xvedio_maker->iframe;
    SCUI_ASSERT(xvedio->iframe.handle != SCUI_HANDLE_INVALID);
    switch (xvedio->iframe.type) {
    case scui_image_type_gif:
    case scui_image_type_lottie: break;
    default: SCUI_ASSERT(false); break;
    }
    
    /* 图像帧数据创建 */
    /* 首次创建:更新一个图像帧 */
    scui_image_frame_make(&xvedio->iframe);
    scui_image_frame_data(&xvedio->iframe);
    
    /* 直接绑定到背景即可,通过绘制背景完成图像帧绘制 */
    scui_widget_image_set(*handle, xvedio->iframe.frame);
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_xvedio_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_xvedio));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_xvedio_t *xvedio = (void *)widget;
    
    /* 图像帧数据销毁 */
    scui_image_frame_burn(&xvedio->iframe);
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 图帧控件播放打断
 *@param handle 图表控件句柄
 */
void scui_xvedio_abort(scui_handle_t handle, bool abort)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_xvedio));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_xvedio_t *xvedio = (void *)widget;
    
    xvedio->abort = abort;
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_xvedio_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_xvedio_t *xvedio = (void *)widget;
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        /* 打断中, 不更新下一帧 */
        if (xvedio->abort)
            break;
        
        /* 更新一个图像帧, 注意:绘制前更新 */
        scui_image_frame_data(&xvedio->iframe);
        
        switch (xvedio->iframe.type) {
        case scui_image_type_lottie: {
            
            xvedio->iframe.lottie.index++;
            if (xvedio->iframe.lottie.index >= xvedio->iframe.lottie.frame)
                xvedio->iframe.lottie.index  = 0;
            
            break;
        }
        }
        
        scui_widget_draw(event->object, NULL, false);
        break;
    }
    default:
        break;
    }
}
