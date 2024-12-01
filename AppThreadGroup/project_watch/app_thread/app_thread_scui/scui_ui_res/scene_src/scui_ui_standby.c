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
    scui_ui_scene_link_cfg(event);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        
        //cwf json 测试
        scui_cwf_json_anim(&scui_ui_res_local->cwf_json_inst);
        
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        /* 界面数据加载准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local == NULL);
            scui_ui_res_local = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(*scui_ui_res_local));
            memset(scui_ui_res_local, 0, sizeof(*scui_ui_res_local));
        }
        
        // cwf json 测试
        if (scui_widget_event_check_prepare(event)) {
            
            scui_cwf_json_make(&scui_ui_res_local->cwf_json_inst, cwf_json_bin[scui_ui_res_local->cwf_json_idx], event->object);
        }
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        
        // cwf json 测试
        if (scui_widget_event_check_finish(event)) {
            
            scui_cwf_json_burn(&scui_ui_res_local->cwf_json_inst);
        }
        
        /* 界面数据转存回收 */
        if (scui_widget_event_check_finish(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            SCUI_MEM_FREE(scui_ui_res_local);
            scui_ui_res_local = NULL;
        }
        break;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        break;
    case scui_event_enc_clockwise: {
        SCUI_LOG_INFO("scui_event_enc_clockwise");
        
        scui_ui_res_local->cwf_json_idx += 1;
        if (scui_ui_res_local->cwf_json_idx >= scui_arr_len(cwf_json_bin))
            scui_ui_res_local->cwf_json_idx  = 0;
        
        scui_cwf_json_burn(&scui_ui_res_local->cwf_json_inst);
        scui_cwf_json_make(&scui_ui_res_local->cwf_json_inst, cwf_json_bin[scui_ui_res_local->cwf_json_idx], event->object);
        scui_widget_draw(event->object, NULL, false);
        break;
    }
    case scui_event_enc_clockwise_anti: {
        SCUI_LOG_INFO("scui_event_enc_clockwise_anti");
        
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
        SCUI_LOG_INFO("scui_event_ptr_click");
        SCUI_LOG_INFO("scui_event_key_click");
        
        scui_window_switch_type_t *cfg_type = NULL;
        scui_window_switch_cfg_type(&cfg_type);
        scui_window_switch_type_t type = *cfg_type;
        *cfg_type = scui_window_switch_none;
        scui_window_stack_del();
        *cfg_type = type;
        
        scui_widget_event_mask_over(event);
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
