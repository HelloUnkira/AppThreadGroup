/*实现目标:
 *    自定义事件响应
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 事件响应(custom)
 *@param event 事件包
 */
void scui_event_custom(scui_event_t *event)
{
    SCUI_LOG_INFO("event widget %u", event->object);
    switch (event->type) {
    default:
        break;
    }
}

/*@brief 事件类型转为字符串
 *@param 事件
 *@retval 字符串
 */
const char * scui_event_custom_to_str(scui_event_type_t type)
{
    const struct {
        uint32_t    type;
        const char *str;
    } to_str_list[] = {
        {.type = scui_event_invalid,                .str = "scui_event_invalid",},
        
        {.type = scui_event_custom_all,             .str = "scui_event_custom_all",},
        
        
    };
    
    for (uint32_t idx = 0; idx < scui_arr_len(to_str_list); idx++)
        if (to_str_list[idx].type == type)
            return to_str_list[idx].str;
    
    return to_str_list[scui_event_invalid].str;
}

/*@brief 事件响应
 *@param event 事件包
 */
void scui_event_custom_prepare(scui_event_t *event)
{
    SCUI_LOG_INFO("event widget %u", event->object);
    switch (event->type) {
    default:
        break;
    }
}

/*@brief 事件响应
 *@param event 事件包
 */
void scui_event_custom_finish(scui_event_t *event)
{
    SCUI_LOG_INFO("event widget %u", event->object);
    switch (event->type) {
    default:
        break;
    }
}
