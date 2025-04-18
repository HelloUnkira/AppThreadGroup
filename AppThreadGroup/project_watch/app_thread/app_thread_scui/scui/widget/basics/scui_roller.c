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
    /* 本类对象 */
    scui_roller_t *roller = widget;
    scui_roller_maker_t *roller_maker = widget_maker;
    
    // 包装专属事件通知
    roller->notify_cb = scroll_maker->notify_cb;
    scroll_maker->notify_cb = scui_roller_scroll_notify_cb;
    
    /* 构造派生控件实例 */
    scui_scroll_make(scroll, scroll_maker, handle, layout);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_roller));
    
    /* 状态初始化 */
    roller->string = roller_maker->string;
    roller->string.widget.parent = widget->myself;
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
    scui_scroll_burn(widget->myself);
}

/*@brief 控件布局更新
 *@param handle 控件句柄
 */
void scui_roller_layout(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_roller));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_roller_t *roller = (void *)widget;
    
    
    
    scui_widget_map_t *widget_map = NULL;
    scui_widget_map_find(widget->type, &widget_map);
    scui_widget_map_find(widget_map->base, &widget_map);
    widget_map->layout(handle);
}

/*@brief 滚轮控件设置字符串
 *@param handle   滚轮控件句柄
 *@param str_utf8 字符串(utf8)
 */
void scui_roller_string_str(scui_handle_t handle, uint8_t *str_utf8)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_roller));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_roller_t *roller = (void *)widget;
    
    // 使用预制的构造器构造对象
    scui_handle_t string_handle = SCUI_HANDLE_INVALID;
    scui_widget_create(&roller->string, &string_handle, false);
    scui_string_update_str(string_handle, str_utf8);
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

/*@brief 专属事件通知回调
 *@param event 事件
 */
void scui_roller_scroll_notify_cb(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_roller_t *roller = (void *)widget;
    
    switch (event->type) {
    case scui_event_widget_scroll_s:
    case scui_event_widget_scroll_e:
    case scui_event_widget_scroll_c:
    case scui_event_widget_scroll_layout:
        scui_scroll_center_target_get(widget->myself, &roller->center);
        break;
    default:
        break;
    }
    
    // 进行层级冒泡
    if (roller->notify_cb != NULL)
        roller->notify_cb(event);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_roller_event(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_roller_t *roller = (void *)widget;
    
    
    
    scui_widget_map_t *widget_map = NULL;
    scui_widget_map_find(widget->type, &widget_map);
    scui_widget_map_find(widget_map->base, &widget_map);
    widget_map->invoke(event);
}
