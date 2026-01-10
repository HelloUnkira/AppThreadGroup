/*实现目标:
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 事件类型检查
 *@param event_type 事件类型
 *@retval 类型匹配
 */
bool scui_event_type_sched(scui_event_type_t event_type)
{
    return event_type > scui_event_sched_s && event_type < scui_event_sched_e;
}

/*@brief 事件类型检查
 *@param event_type 事件类型
 *@retval 类型匹配
 */
bool scui_event_type_widget(scui_event_type_t event_type)
{
    return event_type > scui_event_widget_s && event_type < scui_event_widget_e;
}

/*@brief 事件类型检查
 *@param event_type 事件类型
 *@retval 类型匹配
 */
bool scui_event_type_ptr(scui_event_type_t event_type)
{
    return event_type > scui_event_ptr_s && event_type < scui_event_ptr_e;
}

/*@brief 事件类型检查
 *@param event_type 事件类型
 *@retval 类型匹配
 */
bool scui_event_type_enc(scui_event_type_t event_type)
{
    return event_type > scui_event_enc_s && event_type < scui_event_enc_e;
}

/*@brief 事件类型检查
 *@param event_type 事件类型
 *@retval 类型匹配
 */
bool scui_event_type_key(scui_event_type_t event_type)
{
    return event_type > scui_event_key_s && event_type < scui_event_key_e;
}

/*@brief 事件类型检查
 *@param event_type 事件类型
 *@retval 类型匹配
 */
bool scui_event_type_custom(scui_event_type_t event_type)
{
    return event_type > scui_event_custom_s && event_type < scui_event_custom_e;
}
