/*实现目标:
 *    字体
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_handle_t scui_font_name = SCUI_HANDLE_INVALID;

/*@brief 字库名称设置
 *@param name 字库名称句柄
 */
void scui_font_name_set(scui_handle_t name)
{
    if (scui_font_name == name)
        return;
    
    scui_font_name = name;
    SCUI_ASSERT(scui_font_name != SCUI_HANDLE_INVALID);
    
    scui_event_t event = {
        .object = SCUI_HANDLE_SYSTEM,
        .type   = scui_event_font_change,
        .absorb = scui_event_absorb_none,
    };
    scui_event_notify(&event);
}

/*@brief 字库名称获取
 *@param font_idx 字库名称句柄偏移索引
 *@retval 字库名称句柄
 */
scui_handle_t scui_font_name_get(scui_handle_t font_idx)
{
    SCUI_ASSERT(scui_font_name != SCUI_HANDLE_INVALID);
    return scui_font_name + font_idx;
}
