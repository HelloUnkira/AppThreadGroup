/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static const char * cwf_json_bin[] = {
    "D10705001.bin",
    "D10706001.bin",
    "D10707001.bin",
};

static struct {
    void   *cwf_json_inst;
    uint8_t cwf_json_idx;
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_standby_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        // cwf json 测试
        scui_cwf_json_make(&scui_ui_res_local->cwf_json_inst, cwf_json_bin[scui_ui_res_local->cwf_json_idx], event->object);
        break;
    case scui_event_destroy:
        // cwf json 测试
        scui_cwf_json_burn(&scui_ui_res_local->cwf_json_inst);
        break;
    case scui_event_focus_get:
        scui_ui_scene_link_cfg(event);
        break;
    case scui_event_focus_lost:
        break;
    case scui_event_enc_clockwise: {
        
        scui_ui_res_local->cwf_json_idx += 1;
        if (scui_ui_res_local->cwf_json_idx >= scui_arr_len(cwf_json_bin))
            scui_ui_res_local->cwf_json_idx  = 0;
        
        scui_cwf_json_burn(&scui_ui_res_local->cwf_json_inst);
        scui_cwf_json_make(&scui_ui_res_local->cwf_json_inst, cwf_json_bin[scui_ui_res_local->cwf_json_idx], event->object);
        scui_widget_draw(event->object, NULL, false);
        break;
    }
    case scui_event_enc_clockwise_anti: {
        
        scui_ui_res_local->cwf_json_idx -= 1;
        if (scui_ui_res_local->cwf_json_idx >= scui_arr_len(cwf_json_bin))
            scui_ui_res_local->cwf_json_idx  = scui_arr_len(cwf_json_bin) - 1;
        
        scui_cwf_json_burn(&scui_ui_res_local->cwf_json_inst);
        scui_cwf_json_make(&scui_ui_res_local->cwf_json_inst, cwf_json_bin[scui_ui_res_local->cwf_json_idx], event->object);
        scui_widget_draw(event->object, NULL, false);
        break;
    }
    case scui_event_ptr_click:
    case scui_event_key_click: {
        
        scui_window_switch_type_t *cfg_type = NULL;
        scui_window_switch_cfg_type(&cfg_type);
        scui_window_switch_type_t type = *cfg_type;
        *cfg_type = scui_window_switch_none;
        scui_window_stack_del(SCUI_UI_SCENE_STANDBY);
        *cfg_type = type;
        
        scui_event_mask_over(event);
        break;
    }
    default:
        break;
    }
}
