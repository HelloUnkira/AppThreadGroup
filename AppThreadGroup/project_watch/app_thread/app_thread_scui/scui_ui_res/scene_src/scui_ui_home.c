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
void scui_ui_scene_float_s_event_proc(scui_event_t * event)
{
    // 特殊的固定调用
    scui_widget_event_shift(event);
    
    switch (event->type) {
    case scui_event_create:
        break;
    case scui_event_destroy:
        break;
        
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_handle_t handle_m  = SCUI_HANDLE_INVALID;
        scui_linear_m_get(event->object, &handle_m);
        
        scui_widget_draw_color(event->object, NULL, SCUI_COLOR32_WHITE);
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_m_event_proc(scui_event_t * event)
{
    // 特殊的固定调用
    scui_widget_event_shift(event);
    
    switch (event->type) {
    case scui_event_create:
        break;
    case scui_event_destroy:
        break;
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        scui_handle_t handle_s = SCUI_HANDLE_INVALID;
        scui_linear_m_get(event->object, &handle_s);
        
        scui_handle_t  custom = handle_s;
        scui_area_t  src_clip = scui_widget_clip(custom);
        scui_image_t img_inst = {
            .type           = scui_image_type_mem,
            .format         = scui_widget_surface(custom)->format,
            .pixel.width    = src_clip.w,
            .pixel.height   = src_clip.h,
            .pixel.data_bin = scui_widget_surface(custom)->pixel,
        };
        scui_handle_t image = scui_handle_find();
        scui_handle_linker(image, &img_inst);
        
        scui_widget_draw_image(event->object, NULL, image, NULL, SCUI_COLOR_UNUSED);
        
        scui_handle_clear(image);
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_event_proc(scui_event_t * event)
{
    switch (event->type) {
    case scui_event_create:
        break;
    case scui_event_destroy:
        break;
    }
    
    // 转移至控件调度
    scui_widget_event_shift(event);
}



/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_home_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        // cwf json 测试
        scui_cwf_json_make(&scui_ui_res_local->cwf_json_inst, cwf_json_bin[scui_ui_res_local->cwf_json_idx], event->object);
        
        
        
        scui_event_cb_t event_cb[3] = {
            scui_ui_scene_float_event_proc,
            scui_ui_scene_float_m_event_proc,
            scui_ui_scene_float_s_event_proc,
        };
        // scui_ui_scene_float_event(event, scui_opt_pos_l, event_cb);
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
    case scui_event_ptr_down:
        scui_ui_res_local->ptr_long_jump = false;
        break;
    case scui_event_ptr_hold:
        if (event->ptr_tick > 3000) {
            if (!scui_ui_res_local->ptr_long_jump) SCUI_LOG_WARN("ptr long hold");
            scui_ui_res_local->ptr_long_jump = true;
        }
        break;
    case scui_event_ptr_up:
        if (scui_ui_res_local->ptr_long_jump) {
            scui_ui_res_local->ptr_long_jump = true;
            #if SCUI_MEM_FEAT_MINI == 0
            scui_window_switch_type_t *cfg_type = NULL;
            scui_window_switch_cfg_type(&cfg_type);
            scui_window_switch_type_t type = *cfg_type;
            *cfg_type = scui_window_switch_circle;
            scui_window_stack_add(SCUI_UI_SCENE_LANTERN, false);
            *cfg_type = type;
            #endif
        }
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
        #if SCUI_MEM_FEAT_MINI == 0
        case scui_ui_scene_list_type_list_scale: { // 缩放列表
            #if 0   // 裁内存选择(去掉过场动画)
            scui_window_switch_type_cfg_set(scui_window_switch_none);
            scui_window_stack_add(SCUI_UI_SCENE_LIST_SCALE, false);
            scui_window_switch_type_cfg_set(switch_type);
            #else
            scui_window_stack_add(SCUI_UI_SCENE_LIST_SCALE, false);
            #endif
            break;
        }
        #endif
        case scui_ui_scene_list_type_list_arc: // 弧形列表
            scui_window_stack_add(SCUI_UI_SCENE_LIST_ARC, false);
            break;
        case scui_ui_scene_list_type_honeycomb: // 蜂窝
            scui_window_stack_add(SCUI_UI_SCENE_HONEYCOMB, false);
            break;
        case scui_ui_scene_list_type_waterfall: // 瀑布
            scui_window_stack_add(SCUI_UI_SCENE_WATERFALL, false);
            break;
        case scui_ui_scene_list_type_themewheel: // 波轮
            scui_window_stack_add(SCUI_UI_SCENE_THUMBWHEEL, false);
            break;
        case scui_ui_scene_list_type_spread: // 扩散
            scui_window_stack_add(SCUI_UI_SCENE_SPREAD, false);
            break;
        default:
            break;
        }
        
        scui_event_mask_over(event);
        break;
    }
    default:
        break;
    }
}
