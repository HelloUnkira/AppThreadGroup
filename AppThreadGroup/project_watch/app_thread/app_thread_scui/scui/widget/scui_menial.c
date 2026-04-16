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
void scui_menial_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_menial_t *menial = widget;
    scui_menial_maker_t *menial_maker = widget_maker;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_menial));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    /* menial必须依赖SVG */
    #if SCUI_DRAW_USE_THORVG == 0
    SCUI_LOG_ERROR("menial must rely on SVG");
    SCUI_ASSERT(false);
    #endif
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_menial_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_menial_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_menial_t *menial = (void *)widget;
    
}
