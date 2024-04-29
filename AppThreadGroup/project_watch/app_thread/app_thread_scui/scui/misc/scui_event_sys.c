/*实现目标:
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 事件类型转为字符串
 *@param 事件
 *@retval 字符串
 */
const char * scui_event_sys_to_str(scui_event_type_t type)
{
    const struct {
        uint32_t    type;
        const char *str;
    } to_str_list[] = {
        {.type = scui_event_invalid,                .str = "scui_event_invalid",},
        
        {.type = scui_event_sched_all,              .str = "scui_event_sched_all",},
        {.type = scui_event_sched_delay,            .str = "scui_event_sched_delay",},
        {.type = scui_event_anima_elapse,           .str = "scui_event_anima_elapse",},
        {.type = scui_event_focus_lost,             .str = "scui_event_focus_lost",},
        {.type = scui_event_focus_get,              .str = "scui_event_focus_get",},
        {.type = scui_event_hide,                   .str = "scui_event_hide",},
        {.type = scui_event_show,                   .str = "scui_event_show",},
        {.type = scui_event_draw,                   .str = "scui_event_draw",},
        {.type = scui_event_refr,                   .str = "scui_event_refr",},
        {.type = scui_event_widget_scroll_s,        .str = "scui_event_widget_scroll_s",},
        {.type = scui_event_widget_scroll_c,        .str = "scui_event_widget_scroll_c",},
        {.type = scui_event_widget_scroll_e,        .str = "scui_event_widget_scroll_e",},
        
        {.type = scui_event_ptr_all,                .str = "scui_event_ptr_all",},
        {.type = scui_event_ptr_cover,              .str = "scui_event_ptr_cover",},
        {.type = scui_event_ptr_down,               .str = "scui_event_ptr_down",},
        {.type = scui_event_ptr_hold,               .str = "scui_event_ptr_hold",},
        {.type = scui_event_ptr_click,              .str = "scui_event_ptr_click",},
        {.type = scui_event_ptr_fling,              .str = "scui_event_ptr_fling",},
        {.type = scui_event_ptr_move,               .str = "scui_event_ptr_move",},
        {.type = scui_event_ptr_up,                 .str = "scui_event_ptr_up",},
        
        {.type = scui_event_enc_all,                .str = "scui_event_enc_all",},
        {.type = scui_event_enc_clockwise,          .str = "scui_event_enc_clockwise",},
        {.type = scui_event_enc_clockwise_anti,     .str = "scui_event_enc_clockwise_anti",},
        
        {.type = scui_event_key_all,                .str = "scui_event_key_all",},
        {.type = scui_event_key_down,               .str = "scui_event_key_down",},
        {.type = scui_event_key_hold,               .str = "scui_event_key_hold",},
        {.type = scui_event_key_click,              .str = "scui_event_key_click",},
        {.type = scui_event_key_up,                 .str = "scui_event_key_up",},
    };
    
    for (uint32_t idx = 0; idx < scui_arr_len(to_str_list); idx++)
        if (to_str_list[idx].type == type)
            return to_str_list[idx].str;
    
    return to_str_list[scui_event_invalid].str;
}
