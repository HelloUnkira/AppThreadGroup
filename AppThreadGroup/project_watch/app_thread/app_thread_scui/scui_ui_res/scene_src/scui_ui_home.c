/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static const char * cwf_json_bin[] = {
    "D10597001.bin",
    "D10598001.bin",
    "D10599001.bin",
    "D10600001.bin",
    "D10601001.bin",
    "D10602001.bin",
    "D10603001.bin",
    "D10604001.bin",
};

static struct {
    void   *cwf_json_inst;
    uint8_t cwf_json_idx;
    bool    ptr_long_jump;
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_home_event_proc(scui_event_t *event)
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
    case scui_event_ptr_hold: {
        SCUI_LOG_INFO("scui_event_ptr_hold");
        
        if (event->ptr_tick > 3000 && !scui_ui_res_local->ptr_long_jump) {
            scui_ui_res_local->ptr_long_jump = true;
            scui_window_switch_type_t *cfg_type = NULL;
            scui_window_switch_cfg_type(&cfg_type);
            scui_window_switch_type_t type = *cfg_type;
            *cfg_type = scui_window_switch_circle;
            scui_window_stack_add(SCUI_UI_SCENE_LANTERN, false);
            *cfg_type = type;
        }
        break;
    }
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
    case scui_event_key_click: {
        if (event->key_id != scui_event_key_val_enter)
            break;
        
        scui_window_switch_type_t *cfg_type = NULL;
        scui_window_switch_cfg_type(&cfg_type);
        scui_window_switch_type_t switch_type = *cfg_type;
        static scui_ui_scene_list_type_t type = 0;
        type++;
        
        if (type < scui_ui_scene_list_type_s + 1)
            type = scui_ui_scene_list_type_e - 1;
        if (type > scui_ui_scene_list_type_e - 1)
            type = scui_ui_scene_list_type_s + 1;
        
        SCUI_ASSERT(type > scui_ui_scene_list_type_s);
        SCUI_ASSERT(type < scui_ui_scene_list_type_e);
        
        switch (type) {
        case scui_ui_scene_list_type_list_scale: { // 缩放列表
            #if 0   // 裁内存选择
            scui_ui_scene_list_cfg(type);
            scui_window_switch_type_cfg_set(scui_window_switch_none);
            scui_window_stack_add(SCUI_UI_SCENE_LIST_SCALE, false);
            scui_window_switch_type_cfg_set(switch_type);
            #else
            scui_ui_scene_list_cfg(type);
            scui_window_stack_add(SCUI_UI_SCENE_LIST_SCALE, false);
            #endif
            break;
        }
        case scui_ui_scene_list_type_list_arc: // 弧形列表
            scui_ui_scene_list_cfg(type);
            scui_window_stack_add(SCUI_UI_SCENE_LIST_ARC, false);
            break;
        case scui_ui_scene_list_type_honeycomb: // 蜂窝
            scui_ui_scene_list_cfg(type);
            scui_window_stack_add(SCUI_UI_SCENE_HONEYCOMB, false);
            break;
        case scui_ui_scene_list_type_waterfall: // 瀑布
            scui_ui_scene_list_cfg(type);
            scui_window_stack_add(SCUI_UI_SCENE_WATERFALL, false);
            break;
        case scui_ui_scene_list_type_themewheel: // 波轮
            scui_ui_scene_list_cfg(type);
            scui_window_stack_add(SCUI_UI_SCENE_THUMBWHEEL, false);
            break;
        case scui_ui_scene_list_type_spread: // 扩散
            scui_ui_scene_list_cfg(type);
            scui_window_stack_add(SCUI_UI_SCENE_SPREAD, false);
            break;
        default:
            break;
        }
        
        scui_widget_event_mask_over(event);
        break;
    }
    default:
        if (event->type >= scui_event_ptr_s && event->type <= scui_event_ptr_e)
            scui_window_float_event_check_ptr(event);
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}